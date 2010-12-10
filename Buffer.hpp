#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__

#include <sys/types.h>

class Buffer {
protected:
	char *w_pointer;
public:
	char *buf;

	Buffer(size_t size);

	void append(const char *append, size_t nbytes);
	size_t length() {return w_pointer - buf;}
};

#endif // __BUFFER_HPP__
