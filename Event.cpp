#include "Event.hpp"
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include "util.hpp"

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

	ret += "<programme";
	{
		ret += " channel=\"";
		ret.append(channel);
		ret += "\"";
	}
	{
		char buffer[6];
		sprintf(buffer, "%d", m_event_id); // 16bit, 65535 max => 6 bytes is enough;
		ret += " id=\"";
		ret.append(buffer);
		ret += "\"";
	}
	{
		struct tm *start_datetime;
		start_datetime = localtime(&m_start);
		char buffer[21];
		strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S %Z", start_datetime);
		ret += " start=\"";
		ret.append(buffer);
		ret += "\"";
	}
	{
		time_t stop = m_start + m_duration;
		struct tm *stop_datetime;
		stop_datetime = localtime(&stop);
		char buffer[21];
		strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S %Z", stop_datetime);
		ret += " stop=\"";
		ret.append(buffer);
		ret += "\"";
	}
	ret += ">";
	
	for( typeof(m_descriptors.begin()) d = m_descriptors.begin(); d != m_descriptors.end(); ++d ) {
		ret += (*d)->XMLTV();
	}

	ret += "</programme>";
	return ret;
}
