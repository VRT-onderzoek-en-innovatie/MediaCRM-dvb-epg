#ifndef __SECTION_BUFFER_HPP__
#define __SECTION_BUFFER_HPP__

#include <map>
#include "Buffer.hpp"

#define TS_PACKET_SIZE 188

typedef unsigned char byte;

class Section_Processor {
public:
	virtual void process_section(byte *section, size_t nsection) {}
};

class Section_Buffer {
protected:
	std::multimap<uint16_t, Section_Processor*> pid_map;
	std::map<uint16_t, Buffer*> section_buffer;

public:
	void process_ts_packet(const byte *ts_packet);

	void add_processor(uint16_t pid, Section_Processor *proc);
};

#endif // __SECTION_BUFFER_HPP__
