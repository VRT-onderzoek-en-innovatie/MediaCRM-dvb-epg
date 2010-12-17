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

Event::Event(const unsigned char **event) {
	m_event_id = ((*event)[0]<<8) | (*event)[1];
	m_start = mjd_decode((*event)[2]<<8 | (*event)[3]) + bcd_decode((*event)[4])*3600 + bcd_decode((*event)[5])*60 + (*event)[6];
	m_duration = bcd_decode((*event)[7])*3600 + bcd_decode((*event)[8])*60 + bcd_decode((*event)[9]);
	m_running_status = (enum running_status)( ((*event)[10]&0xe0)>>5 );
	m_free_CA_mode = (enum free_CA_mode)( ((*event)[10]&0x10) );
	uint16_t descriptors_loop_length = (((*event)[10]&0x0f)<<8) | (*event)[11];
	*event += 12;
	
	const unsigned char *event_end = *event + descriptors_loop_length;
	while( *event < event_end ) {
		Descriptor::Descriptor *d = Descriptor::descriptor_factory(event);
		m_descriptors.push_back(d);
	}
}

Event::~Event() {
	for( typeof(m_descriptors.begin()) it = m_descriptors.begin(); it != m_descriptors.end(); ++it) {
		delete *it;
	}
}

std::string Event::XMLTV(char* channel) const {
	std::string ret;
	ret += "<programme channel=\"";
	ret += channel;
	ret += "\" id=\"\" start=\"\" stop=\"\">";

	ret += "<title></title>";
	ret += "</programme>";
	return ret;
}
