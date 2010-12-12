#include <stdio.h>
#include "../debug.h"
#include "../CRC32.hpp"

#define verify(e)  \
    ((e) ? 0 : (__verify (#e, __FILE__, __LINE__),1))
#define __verify(e, file, line)  \
    fprintf(stderr, "%s:%u: failed verify `%s'\n", file, line, e)

int main() {
	int rv = 0;

	{
	CRC32 c(0x04c11db7, 0xffffffff, 0xffffffff, 1, 1);
	c.append("123456789", 9);
	rv |= verify( c.crc() == 0xCBF43926 );
	}

	{
	CRC32 c(0x04c11db7, 0xffffffff, 0xffffffff, 0, 1);
	c.append("123456789", 9);
	rv |= verify( c.crc() == 0x1898913f );
	}

	{
	CRC32 c(0x04c11db7, 0xffffffff, 0xffffffff, 1, 0);
	c.append("123456789", 9);
	rv |= verify( c.crc() == 0x649c2fd3 );
	}

	{
	CRC32 c(0x04c11db7, 0xffffffff, 0xffffffff, 0, 0);
	c.append("123456789", 9);
	rv |= verify( c.crc() == 0xfc891918 );
	}

	{
	CRC32 c(0x04c12db7, 0xfffff0ff, 0xffffff0f, 0, 0);
	c.append("123456789", 9);
	rv |= verify( c.crc() == 0x8976f2ee );
	}

	return rv;
}
