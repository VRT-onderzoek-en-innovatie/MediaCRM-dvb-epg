#include "eit_processor.hpp"
#include "CRC32.hpp"
#include "Event.hpp"
#include "debug.h"
#include <stdint.h>
#include <stdio.h>

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

	const unsigned char *section_end = section+3 + section_length - 4; // "immediately following the length field" + length - CRC32
	section += 14;

	while( section < section_end ) {
		Event *e = new Event(&section);

		delete e;
	}
}

EIT_channel_processor::~EIT_channel_processor() {
}

void EIT_processor::process_sections(const unsigned char *sections, size_t nsections) {
	assert(sections != NULL);
	const unsigned char *sections_end = sections + nsections;
	
	while( sections < sections_end ) {
		assert(sections_end-sections >= 14+4); // 0 events

		//uint8_t table_id = section[0];
		assert( (sections[1]&0x80) == 0x80 ); // sections syntax indicator
		uint16_t section_length = ((sections[1]&0x0f)<<8) | sections[2];
		uint16_t service_id = (sections[3]<<8) | sections[4];
		//uint8_t version_number = (sections[5]&0x3e)>>1;
		//bool current_next_indicator = (sections[5]&0x01);
		//uint8_t sections_number = sections[6];
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
