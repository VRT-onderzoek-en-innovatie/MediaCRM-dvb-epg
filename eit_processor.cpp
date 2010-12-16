#include "eit_processor.hpp"
#include "CRC32.hpp"
#include "debug.h"
#include <stdint.h>
#include <math.h>
#include <stdio.h>

EIT_channel_processor::EIT_channel_processor() {
	for( int i = 0; i < 16; i++ ) { m_version[i] = 0x20; } // Impossible value, anything will differ
}

void EIT_channel_processor::parse_segment(const unsigned char *section, size_t nsection) {
	assert(section != NULL);
	assert(nsection >= 14+4); // 0 events

	uint8_t table_id = section[0];
	assert( (section[1]&0x80) == 0x80 ); // section syntax indicator
	uint16_t section_length = ((section[1]&0x0f)<<8) | section[2];
	//uint16_t service_id = (section[3]<<8) | section[4];
	uint8_t version_number = (section[5]&0x3e)>>1;
	bool current_next_indicator = (section[5]&0x01);
	uint8_t section_number = section[6];
	uint8_t last_section_number = section[7];
	//uint16_t transport_stream_id = (section[8]<<8) | section[9];
	//uint16_t original_network_id = (section[10]<<8) | section[11];
	uint8_t segment_last_section_number = section[12];
	uint8_t last_table_id = section[13];

	if( !current_next_indicator ) return; // table not yet applicable, skip

	if( version_number != m_version[table_id&0x0f] ) {
		m_version[table_id&0x0f] = version_number;
		// Something changed, flush everything
		for( typeof(m_events.begin()) i = m_events.begin(); i != m_events.end(); ++i ) {
			delete *i;
		}
		m_events.clear();
		// wait for the first section of the first segment of the first sub-table
		m_waiting_for_table = table_id&0xf0; // 0x50 or 0x60
		m_waiting_for_section = 0x00;
		m_section_gap_allowed = false;
	}

	if( table_id != m_waiting_for_table ) return;
	if( !m_section_gap_allowed && section_number != m_waiting_for_section ) return;
	if( m_section_gap_allowed && section_number < m_waiting_for_section ) return;

	const unsigned char *section_end = section+3 + section_length - 4; // "immediately following the length field" + length - CRC32
	section += 14;
	while( section < section_end ) {
		Event *e = new Event(&section);
		m_events.push_back(e);
	}

	if( section_number < segment_last_section_number ) {
		// wait for next section of this segment
		m_waiting_for_section = section_number+1;
		m_section_gap_allowed = false;
	} else if( section_number == segment_last_section_number ) {
		// wait for next segment
		if( section_number < last_section_number ) {
			// in this table
			m_waiting_for_section = section_number+1;
			m_section_gap_allowed = true;
		} else if( section_number == last_section_number ) {
			// in next table
			if( table_id < last_table_id ) {
				m_waiting_for_table = table_id+1;
				m_waiting_for_section = 0;
				m_section_gap_allowed = false;
			} else if( table_id == last_table_id ) {
				// We're done
				m_waiting_for_table = 0xff; // stop listening
				// Will be reset on a version change
				full_table_received();
			} else {
				assert( table_id <= last_table_id ); // abort()
			}
		} else {
			assert( section_number <= last_section_number ); // abort()
		}
	} else {
		assert(section_number <= segment_last_section_number); // abort()
	}
}

EIT_channel_processor::~EIT_channel_processor() {
	for( typeof(m_events.begin()) i = m_events.begin(); i != m_events.end(); ++i ) {
		delete *i;
	}
}

void EIT_channel_processor::full_table_received() const {
	printf("Got %lu events\n", m_events.size());
}

void EIT_processor::process_sections(const unsigned char *sections, size_t nsections) {
	assert(sections != NULL);
	const unsigned char *sections_end = sections + nsections;
	
	while( sections < sections_end ) {
		uint8_t table_id = sections[0];
		if( table_id == 0xff && sections_end - sections < 3 ) {
			// incomplete stuffing table, skip everything till end
			break;
		}

		assert(sections_end-sections >= 3); // only tag & length

		assert( (sections[1]&0x80) == 0x80 ); // sections syntax indicator
		uint16_t section_length = ((sections[1]&0x0f)<<8) | sections[2];
		if( table_id == 0xff ) { // padding table, skip
			sections += 3 + section_length;
			continue;
		}
		if( table_id == 0x4e || table_id == 0x4f ) {
			// now/next tables, we want more!
			sections += 3 + section_length;
			continue;
		}
		if( table_id < 0x50 || table_id > 0x6f ) {
			// shouldn't be here
			fprintf(stderr, "Unexpected table (id 0x%02x) ignored\n", table_id);
			sections += 3 + section_length;
			continue;
		}

		assert( table_id >= 0x50 && table_id <= 0x6f );

		uint16_t service_id = (sections[3]<<8) | sections[4];
		//uint8_t version_number = (sections[5]&0x3e)>>1;
		//bool current_next_indicator = (sections[5]&0x01);
		//uint8_t section_number = sections[6];
		//uint8_t last_sections_number = sections[7];
		uint16_t transport_stream_id = (sections[8]<<8) | sections[9];
		uint16_t original_network_id = (sections[10]<<8) | sections[11];
		//uint8_t segment_last_sections_number = sections[12];
		//uint8_t last_table_id = sections[13];
		uint32_t crc = (sections[3+section_length-4] << 24)
				| (sections[3+section_length-3] << 16)
				| (sections[3+section_length-2] << 8)
				| (sections[3+section_length-1]);
	
		{
			CRC32 c(0x04C11DB7, 0xffffffff, 0x00000000, 0, 0);
			c.append(sections, 3+section_length-4);
			if( c.crc() != crc ) {
				fprintf(stderr, "CRC mismatch, ignoring sections\n");
				// we can't recover by skipping only this section:
				// section_length might be corrupt
				// throw away everything
				return;
			}
		}

		struct channel_id chan_id = { original_network_id, transport_stream_id, service_id };
		std::pair< typeof(m_channels.begin()), bool > chan_insert
			= m_channels.insert( std::pair< struct channel_id, EIT_channel_processor >
				(chan_id, EIT_channel_processor() ) ); // insert or find
		chan_insert.first->second.parse_segment(sections, 3+section_length);

		sections += 3 + section_length;
	}
}
