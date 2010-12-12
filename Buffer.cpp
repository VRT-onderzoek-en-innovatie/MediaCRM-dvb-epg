#include "Buffer.hpp"
#include <string.h>

Buffer::Buffer(size_t size) {
	buf = new unsigned char[size];
	w_pointer = buf;
}

void Buffer::append(const unsigned char *append, size_t nbytes) {
	memcpy(w_pointer, append, nbytes);
	w_pointer += nbytes;
}
