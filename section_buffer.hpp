#ifndef __SECTION_BUFFER_HPP__
#define __SECTION_BUFFER_HPP__

#include <map>
#include "Buffer.hpp"

#define TS_PACKET_SIZE 188

class Section_Processor {
public:
	virtual void process_sections(const unsigned char *sections, size_t nsections)=0;
};

class Section_Buffer {
protected:
	std::multimap<uint16_t, Section_Processor*> pid_map;
	std::map<uint16_t, Buffer*> section_buffer;

public:
	void process_ts_packet(const unsigned char *ts_packet);

	void add_processor(uint16_t pid, Section_Processor *proc);
};

#endif // __SECTION_BUFFER_HPP__
