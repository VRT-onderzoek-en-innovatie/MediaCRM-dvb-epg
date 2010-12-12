#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__

#include <sys/types.h>

class Buffer {
protected:
	unsigned char *w_pointer;
public:
	unsigned char *buf;

	Buffer(size_t size);

	void append(const unsigned char *append, size_t nbytes);
	void flush() { w_pointer = buf; }

	size_t length() {return w_pointer - buf;}
};

#endif // __BUFFER_HPP__
