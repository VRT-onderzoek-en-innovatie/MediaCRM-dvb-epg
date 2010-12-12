#include "Event.hpp"
#include <stdio.h>

size_t Event::parse_event(const unsigned char *events) {
	size_t length = 0;

	m_event_id = (events[0]<<8) | events[1];
	unsigned char start_time[5] = {events[2], events[3], events[4], events[5], events[6]};
	unsigned char duration[3] = {events[7], events[8], events[9]};
	m_running_status = (enum running_status)( (events[10]&0xe0)>>5 );
	m_free_CA_mode = (enum free_CA_mode)( (events[10]&0x10) );
	uint16_t descriptors_loop_length = ((events[10]&0x0f)<<8) | events[11];
	length += 12;

	printf("Event %u\n", m_event_id);
	// TODO: descriptors (events+12)

	length += descriptors_loop_length;
	return length;
}
