#ifndef __DESCRIPTOR_HPP__
#define __DESCRIPTOR_HPP__

#include <stdint.h>
#include <string>

namespace Descriptor {

class Descriptor {
public:
	uint8_t m_tag;
	uint8_t m_length;

	virtual std::string XMLTV() const { return ""; }
};

Descriptor* descriptor_factory(const unsigned char **descriptor);

} // namespace

#endif // __DESCRIPTOR_HPP__
