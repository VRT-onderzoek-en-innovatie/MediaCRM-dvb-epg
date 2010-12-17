#include "eit_processor.hpp"
#include "CRC32.hpp"
#include "debug.h"
#include <stdint.h>
#include <math.h>
#include <stdio.h>

inline int cyclic_compare(int a, int b, int wrap) {
	if( abs(a-b) < wrap/2 ) return a-b;
	if( abs(a-b) == wrap/2 ) return 1; // arbitrary
	return b-a;
}

void EIT_processor::process_sections(const unsigned char *sections, size_t nsections) {
	assert(sections != NULL);
	const unsigned char *sections_end = sections + nsections;
	
	// A single Section Payload Unit may contain multiple tables
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
		uint8_t version_number = (sections[5]&0x3e)>>1;
		bool current_next_indicator = (sections[5]&0x01);
		uint8_t section_number = sections[6];
		uint8_t last_sections_number = sections[7];
		uint16_t transport_stream_id = (sections[8]<<8) | sections[9];
		uint16_t original_network_id = (sections[10]<<8) | sections[11];
		uint8_t segment_last_sections_number = sections[12];
		uint8_t last_table_id = sections[13];
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
			= m_channels.insert( std::pair< struct channel_id, EIT_processor_channel >
				(chan_id, EIT_processor_channel(this) ) ); // insert or find
		if( chan_insert.second ) {
			fprintf(stderr, "Found new channel %04x/%04x/%04x\n",
				original_network_id, transport_stream_id, service_id);
		}
		chan_insert.first->second.parse_table(sections, 3+section_length);

		sections += 3 + section_length;
	}
}

EIT_processor_channel::EIT_processor_channel(EIT_processor *parent) :
	m_parent(parent),
	m_last_table_id(0) {
}

void EIT_processor_channel::parse_table(const unsigned char *table, size_t ntable) {
	uint8_t table_id = table[0];
	uint16_t section_length = ((table[1]&0x0f)<<8) | table[2];
	uint16_t service_id = (table[3]<<8) | table[4];
	uint8_t version_number = (table[5]&0x3e)>>1;
	bool current_next_indicator = (table[5]&0x01);
	uint8_t section_number = table[6];
	uint8_t last_table_number = table[7];
	uint16_t transport_stream_id = (table[8]<<8) | table[9];
	uint16_t original_network_id = (table[10]<<8) | table[11];
	uint8_t segment_last_table_number = table[12];
	uint8_t last_table_id = table[13];

	m_chan.original_network_id = original_network_id;
	m_chan.transport_stream_id = transport_stream_id;
	m_chan.service_id = service_id;
	
	if( !current_next_indicator ) return; // We're only interested in current info

	if( last_table_id != m_last_table_id ) {
		fprintf(stderr, "%04x/%04x/%04x : number of tables changes %d -> %d\n", 
			original_network_id, transport_stream_id, service_id,
			(m_last_table_id & 0x0f)+1, (last_table_id & 0x0f)+1);
		m_last_table_id = last_table_id;
		// TODO: what to flush?
	}

	typeof(m_tables.begin()) t = m_tables.find(table_id);
	if( t != m_tables.end() ) {
		if( cyclic_compare(version_number, t->second->m_version, 0x20) <= 0 ) {
			// This or a more recent version is already fully processed
			// we're done
			return;
		}
	}
	// This is a newer version of the one we already have
	std::pair< typeof(m_tables_processing.begin()) , bool > table_insert
		= m_tables_processing.insert( std::pair< uint8_t, EIT_processor_channel_table* > (
			table_id, new EIT_processor_channel_table(this, version_number) ));
	table_insert.first->second->parse_table(table, ntable);
}

EIT_processor_channel::~EIT_processor_channel() {
	for( typeof(m_tables_processing.begin()) it = m_tables_processing.begin(); it != m_tables_processing.end(); ++it) {
		delete it->second;
	}
	for( typeof(m_tables.begin()) it = m_tables.begin(); it != m_tables.end(); ++it) {
		delete it->second;
	}
}

EIT_processor_channel_table::EIT_processor_channel_table(EIT_processor_channel *parent, uint8_t ver) :
	m_parent(parent),
	m_version(ver),
	m_waiting_for_section(0),
	m_waiting_for_section_gap_allowed(false) {
}

void EIT_processor_channel_table::parse_table(const unsigned char *table, size_t ntable) {
	uint8_t table_id = table[0];
	uint16_t section_length = ((table[1]&0x0f)<<8) | table[2];
	uint16_t service_id = (table[3]<<8) | table[4];
	uint8_t version_number = (table[5]&0x3e)>>1;
	bool current_next_indicator = (table[5]&0x01);
	uint8_t section_number = table[6];
	uint8_t last_section_number = table[7];
	uint16_t transport_stream_id = (table[8]<<8) | table[9];
	uint16_t original_network_id = (table[10]<<8) | table[11];
	uint8_t segment_last_section_number = table[12];
	uint8_t last_table_id = table[13];

	if( cyclic_compare( version_number, m_version, 0x20 ) < 0 ) {
		// older version
		return;
	}
	if( cyclic_compare( version_number, m_version, 0x20 ) != 0 ) {
		fprintf(stderr, "Warning for %04x/%04x/%04x table 0x%02x: "
			"Version change before full table could be received: %d -> %d\n",
			original_network_id, transport_stream_id, service_id, table_id,
			m_version, version_number);
		// TODO: flush everything
		m_version = version_number;
	}

	if( !m_waiting_for_section_gap_allowed && section_number != m_waiting_for_section ) return;
	if( m_waiting_for_section_gap_allowed && section_number < m_waiting_for_section ) return;

	// We have the section we want
	const unsigned char *table_end = table + 3 + section_length - 4; // "immediately following the length field" + length - CRC32
	table += 14;
	while( table < table_end ) {
		Event *e = new Event(&table);
		m_events.push_back(e);
	}

	if( section_number < segment_last_section_number ) {
		m_waiting_for_section = section_number+1;
		m_waiting_for_section_gap_allowed = false;
	} else if( section_number == segment_last_section_number ) {
		if( section_number < last_section_number ) {
			m_waiting_for_section = section_number+1;
			m_waiting_for_section_gap_allowed = true;
		} else if( section_number == last_section_number ) {
			// Finished!
			m_parent->table_done(table_id);
		}
	}
}

EIT_processor_channel_table::~EIT_processor_channel_table() {
	for( typeof(m_events.begin()) it = m_events.begin(); it != m_events.end(); ++it) {
		delete *it;
	}
}

void EIT_processor_channel::table_done(uint8_t table_id) {
	typeof(m_tables_processing.begin()) new_it = m_tables_processing.find(table_id);
	m_tables.erase(table_id);
	m_tables.insert( std::pair<uint8_t, EIT_processor_channel_table* >( table_id, new_it->second ) );
	m_tables_processing.erase(new_it);

	fprintf(stderr, "%04x/%04x/%04x updated data in table %02x (v%d)\n",
		m_chan.original_network_id, m_chan.transport_stream_id, m_chan.service_id,
		table_id, new_it->second->m_version);

	m_parent->channel_done(m_chan);
}

void EIT_processor::channel_done(struct channel_id chan) {
	typeof( m_channels.begin() ) it = m_channels.find( chan );
	assert( it != m_channels.end() );

	size_t events = 0;
	for( typeof(it->second.m_tables.begin()) table = it->second.m_tables.begin();
	    table != it->second.m_tables.end(); table++) {
		events += table->second->m_events.size();
	}

	printf("%04x/%04x/%04x has updated data (%lu events in total)\n",
		it->second.m_chan.original_network_id,
		it->second.m_chan.transport_stream_id,
		it->second.m_chan.service_id,
		events);
}
