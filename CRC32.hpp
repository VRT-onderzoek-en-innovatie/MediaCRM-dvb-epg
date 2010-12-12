#ifndef __CRC32_HPP__
#define __CRC32_HPP__

#include <stdint.h>
#include <string.h>

class CRC32 {
protected:
	uint32_t m_poly;
	uint32_t m_init;
	uint32_t m_xorout;
	bool m_refin;
	bool m_refout;

	uint32_t m_register;

	uint32_t m_table[256];

	static uint32_t reflect(uint32_t in);
	uint32_t table_entry(uint8_t control);
public:
	CRC32(uint32_t poly=0x04C11DB7,
		uint32_t init=0xffffffff,
		uint32_t xorout=0xffffffff,
		bool refin=1,
		bool refout=1);
	
	void init();
	void append(const char* bytes, size_t nbytes);
	uint32_t crc();
};

#endif // __CRC32_HPP__
