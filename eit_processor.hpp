#ifndef __EIT_PROCESSOR_HPP__
#define __EIT_PROCESSOR_HPP__

#include "section_buffer.hpp"

class EIT_processor: public Section_Processor {
public:
	virtual void process_section(byte *section, size_t nsection);
};

#endif // __EIT_PROCESSOR_HPP__
