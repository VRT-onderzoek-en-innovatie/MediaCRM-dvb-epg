#include <unistd.h>
#include "section_buffer.hpp"
#include "eit_processor.hpp"

int main(int argc, char *argv[]) {
	unsigned char ts_packet[188];
	Section_Buffer sb;
	EIT_processor eit;

	sb.add_processor(0x12, &eit);
	sb.add_processor(0x14, &eit);

	while( read(0, ts_packet, TS_PACKET_SIZE) == TS_PACKET_SIZE ) {
		sb.process_ts_packet(ts_packet);
	}
}
