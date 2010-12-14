#ifndef __DESCRIPTOR_UNKNOWN_HPP__
#define __DESCRIPTOR_UNKNOWN_HPP__

#include "Descriptor.hpp"
#include <stdint.h>

namespace Descriptor {

class Unknown : public Descriptor {
public:
	Unknown(const unsigned char **descriptor);
};

} // namespace

#endif // __DESCRIPTOR_UNKNOWN_HPP__
