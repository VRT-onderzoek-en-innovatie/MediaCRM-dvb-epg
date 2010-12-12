#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include <string.h>
#include <stdint.h>

class Event {
protected:
	uint16_t m_event_id;
	enum running_status {
		undefined=0,
		not_running=1,
		starts_in_a_few_seconds=2,
		pausing=3,
		running=4,
		service_off_air=5,
		reserved_1=6,
		reserved_2=7 } m_running_status;
	enum free_CA_mode {
		free=0,
		scrambled=1 } m_free_CA_mode;

public:
	size_t parse_event(const unsigned char *events);
};

#endif // __EVENT_HPP__
