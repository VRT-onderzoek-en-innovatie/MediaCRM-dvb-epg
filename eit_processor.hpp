#ifndef __EIT_PROCESSOR_HPP__
#define __EIT_PROCESSOR_HPP__

#include "section_buffer.hpp"
#include "Event.hpp"
#include <stdint.h>
#include <map>
#include <list>

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

class EIT_processor_channel; // declaration
class EIT_processor_channel_table {
public:
	EIT_processor_channel *m_parent;
	uint8_t m_version;
	uint8_t m_waiting_for_section;
	bool m_waiting_for_section_gap_allowed;
	std::list< Event* > m_events;

	EIT_processor_channel_table(EIT_processor_channel *parent, uint8_t ver = 0);
	~EIT_processor_channel_table();
	void parse_table(const unsigned char *table, size_t ntable);
};

class EIT_processor; // declaration
class EIT_processor_channel {
public:
	EIT_processor *m_parent;
	struct channel_id m_chan;
	uint8_t m_last_table_id;
	std::map< uint8_t, EIT_processor_channel_table* > m_tables;
	std::map< uint8_t, EIT_processor_channel_table* > m_tables_processing;

	EIT_processor_channel(EIT_processor *parent);
	~EIT_processor_channel();
	void parse_table(const unsigned char *table, size_t ntable);
	void table_done(uint8_t table_id);
};

class EIT_processor: public Section_Processor {
public:
	std::map< struct channel_id, EIT_processor_channel > m_channels;

	virtual void process_sections(const unsigned char *sections, size_t nsections);
	void channel_done(struct channel_id chan);
};

#endif // __EIT_PROCESSOR_HPP__
