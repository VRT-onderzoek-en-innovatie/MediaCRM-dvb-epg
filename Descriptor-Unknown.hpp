#ifndef __DESCRIPTOR_UNKNOWN_HPP__
#define __DESCRIPTOR_UNKNOWN_HPP__

#include "Descriptor.hpp"
#include <stdint.h>

class Descriptor_Unknown : public Descriptor{
public:
	uint8_t m_tag;
	uint8_t m_length;

	Descriptor_Unknown(const unsigned char **descriptor);
};

#endif // __DESCRIPTOR_UNKNOWN_HPP__
