#include "Event.hpp"
#include <math.h>
#include <assert.h>
#include <stdio.h>

inline uint8_t bcd_decode(uint8_t in) {
	assert( (in&0x0f) <= 0x09 );
	assert( (in&0xf0) <= 0x90 );
	return (in>>4)*10+(in&0x0f);
}

inline time_t mjd_decode(uint16_t mjd) {
	struct tm ret;
	
	int ya = ((double)mjd - 15078.2)/365.25;
	int ma = ((double)mjd - 14956.1 - floor(ya * 365.25)) / 30.6001;
	ret.tm_mday = mjd - 14956 - floor(ya*365.25) - floor(ma*30.6001);
	int k = (ma==14 || ma==15 ? 1 : 0);
	ret.tm_year = ya + k;
	ret.tm_mon = ma - 1 - k*12   -1;
	ret.tm_hour = ret.tm_min = ret.tm_sec = 0;

	return mktime(&ret);
}

size_t Event::parse_event(const unsigned char *events) {
	size_t length = 0;

	m_event_id = (events[0]<<8) | events[1];
	m_start = mjd_decode(events[2]<<8 | events[3]) + bcd_decode(events[4])*3600 + bcd_decode(events[5])*60 + events[6];
	m_duration = bcd_decode(events[7])*3600 + bcd_decode(events[8])*60 + bcd_decode(events[9]);
	m_running_status = (enum running_status)( (events[10]&0xe0)>>5 );
	m_free_CA_mode = (enum free_CA_mode)( (events[10]&0x10) );
	uint16_t descriptors_loop_length = ((events[10]&0x0f)<<8) | events[11];
	length += 12;

	printf("Event %u: %u %s", m_event_id, m_duration, ctime(&m_start));
	// TODO: descriptors (events+12)

	length += descriptors_loop_length;
	return length;
}
