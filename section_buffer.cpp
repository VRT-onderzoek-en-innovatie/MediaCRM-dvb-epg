#include "section_buffer.hpp"
#include <assert.h>

void Section_Buffer::process_ts_packet(const unsigned char *ts_packet) {
	assert( ts_packet[0] == 0x47 ); // TS sync byte
	uint16_t pid = ((ts_packet[1]&0x1f)<<8) | ts_packet[2];

	if( pid_map.find(pid) == pid_map.end() ) {
		// no-one is interested in this PID, throw away
		return;
	}
	
	bool payload_present = ts_packet[3] & 0x10;
	if( ! payload_present ) return;

	size_t payload_start = 4;
	bool adaptation_field_present = ts_packet[3] & 0x20;
	if( adaptation_field_present ) {
		payload_start += 1 + ts_packet[4]; // Adaptation field length
	}

	Buffer *pid_buf = NULL;
	typeof(section_buffer.begin()) it;
	if( (it = section_buffer.find(pid) ) != section_buffer.end() ) {
		pid_buf = it->second;
	}

	bool payload_unit_start_indicator = ts_packet[1] & 0x40;
	if( payload_unit_start_indicator ) {
		uint8_t pointer = ts_packet[payload_start];

		if( pid_buf && pointer != 0) {
			// add the first few bytes to the previous section
			pid_buf->append( ts_packet+payload_start, pointer );
		}
		payload_start += 1 + pointer;

		if( pid_buf ) {
			// process the previous section
			std::pair< typeof(pid_map.begin()), typeof(pid_map.begin()) > processors =
				pid_map.equal_range(pid);

			for( typeof(pid_map.begin()) it = processors.first; it != processors.second; ++it ) {
				it->second->process_section( pid_buf->buf, pid_buf->length() );
			}
			pid_buf->flush();
		} else {
			// create a fresh buffer
			pid_buf = new Buffer(4096);
			section_buffer.insert( std::pair<uint16_t, Buffer*>(pid, pid_buf) );
		}
	}
	pid_buf->append( ts_packet+payload_start, TS_PACKET_SIZE-payload_start );
}

void Section_Buffer::add_processor(uint16_t pid, Section_Processor *proc) {
	pid_map.insert( std::pair<uint16_t, Section_Processor*>(pid, proc) );
}
