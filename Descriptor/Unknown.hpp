#ifndef __DESCRIPTOR_UNKNOWN_HPP__
#define __DESCRIPTOR_UNKNOWN_HPP__

#include "Descriptor.hpp"
#include <stdint.h>

namespace Descriptor {

class Unknown : public Descriptor {
public:
	uint8_t m_tag;
	uint8_t m_length;

	Unknown(const unsigned char **descriptor);
};

} // namespace

#endif // __DESCRIPTOR_UNKNOWN_HPP__
