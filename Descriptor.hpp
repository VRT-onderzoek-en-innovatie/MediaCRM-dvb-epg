#ifndef __DESCRIPTOR_HPP__
#define __DESCRIPTOR_HPP__

#include <stdint.h>

class Descriptor {
public:
	uint8_t m_tag;
	uint8_t m_length;

	Descriptor(const unsigned char **descriptor);
};

#endif // __DESCRIPTOR_HPP__
