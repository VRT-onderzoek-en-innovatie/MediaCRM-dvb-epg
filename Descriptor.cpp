#include "Descriptor.hpp"
#include "Descriptor-Unknown.hpp"
#include <assert.h>

Descriptor* Descriptor::descriptor_factory(const unsigned char **descriptor) {
	uint8_t tag = (*descriptor)[0];
	switch( tag ) {
	default:
		return new Descriptor_Unknown(descriptor);
	}
	assert(0); // should never ever get here
}
