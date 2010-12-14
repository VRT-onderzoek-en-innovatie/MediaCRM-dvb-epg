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

	const unsigned char *section_end = section+3 + section_length - 4; // "immediately following the length field" + legth - CRC32
	section += 14;

	while( section < section_end ) {
		Event *e = new Event(&section);

		delete e;
	}
}

EIT_channel_processor::~EIT_channel_processor() {
}

void EIT_processor::process_section(const unsigned char *section, size_t nsection) {
	assert(section != NULL);
	assert(nsection >= 14+4); // 0 events

	//uint8_t table_id = section[0];
	assert( (section[1]&0x80) == 0x80 ); // section syntax indicator
	uint16_t section_length = ((section[1]&0x0f)<<8) | section[2];
	uint16_t service_id = (section[3]<<8) | section[4];
	//uint8_t version_number = (section[5]&0x3e)>>1;
	//bool current_next_indicator = (section[5]&0x01);
	//uint8_t section_number = section[6];
	//uint8_t last_section_number = section[7];
	uint16_t transport_stream_id = (section[8]<<8) | section[9];
	uint16_t original_network_id = (section[10]<<8) | section[11];
	//uint8_t segment_last_section_number = section[12];
	//uint8_t last_table_id = section[13];
	uint32_t crc = (section[3+section_length-4] << 24)
			| (section[3+section_length-3] << 16)
			| (section[3+section_length-2] << 8)
			| (section[3+section_length-1]);

	{
		CRC32 c(0x04C11DB7, 0xffffffff, 0x00000000, 0, 0);
		c.append(section, 3+section_length-4);
		if( c.crc() != crc ) {
			fprintf(stderr, "CRC mismatch, ignoring section\n");
			return;
		}
	}

	struct channel_id chan_id = { original_network_id, transport_stream_id, service_id };
	std::pair< typeof(m_channels.begin()), bool > chan_insert
		= m_channels.insert( std::pair< struct channel_id, EIT_channel_processor >
			(chan_id, EIT_channel_processor() ) ); // insert or find
	chan_insert.first->second.parse_segment(section, nsection);
}
