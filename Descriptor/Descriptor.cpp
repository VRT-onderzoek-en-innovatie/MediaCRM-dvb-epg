#include "Descriptor.hpp"
#include "Unknown.hpp"
#include <assert.h>

namespace Descriptor {

Descriptor* descriptor_factory(const unsigned char **descriptor) {
	uint8_t tag = (*descriptor)[0];
	switch( tag ) {
	default:
		return new Unknown(descriptor);
	}
	assert(0); // should never ever get here
}

} // namespace
