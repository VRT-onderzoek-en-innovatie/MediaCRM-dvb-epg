#ifndef __EIT_PROCESSOR_HPP__
#define __EIT_PROCESSOR_HPP__

#include "section_buffer.hpp"
#include "Event.hpp"
#include <stdint.h>
#include <map>
#include <list>

class EIT_channel_processor {
public:
	uint8_t m_version[16]; // for sub-table 0x50-0x5f or 0x60-0x6f

	uint8_t m_waiting_for_table;
	uint8_t m_waiting_for_section;
	bool m_section_gap_allowed;

	std::list<Event*> m_events;

	EIT_channel_processor();
	~EIT_channel_processor();

	void parse_segment(const unsigned char *section, size_t nsection);
	void full_table_received() const;
};

class EIT_processor: public Section_Processor {
public:
	struct channel_id {
		uint16_t original_network_id;
		uint16_t transport_stream_id;
		uint16_t service_id;
		bool operator< (const struct channel_id &that) const {
			if( this->original_network_id != that.original_network_id )
				return this->original_network_id < that.original_network_id;
			if( this->transport_stream_id != that.transport_stream_id )
				return this->transport_stream_id < that.transport_stream_id;
			return this->service_id < that.service_id;
		}
	};
	std::map< struct channel_id, EIT_channel_processor > m_channels;

	virtual void process_sections(const unsigned char *sections, size_t nsections);
};

#endif // __EIT_PROCESSOR_HPP__
