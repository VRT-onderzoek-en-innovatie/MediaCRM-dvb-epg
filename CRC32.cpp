#include "CRC32.hpp"
#include <assert.h>

CRC32::CRC32(uint32_t poly, uint32_t init, uint32_t xorout, bool refin, bool refout) :
	m_poly(poly), m_init(init), m_xorout(xorout), m_refin(refin), m_refout(refout) {

	if( m_refin ) { // reflect the poly
		m_poly = reflect(poly);
	}

	// Calculate table
	for(int b=0; b<256; b++) {
		m_table[b] = this->table_entry(b);
	}

	this->init();
}

uint32_t CRC32::reflect(uint32_t in) {
	uint32_t out = 0;
	for(int i=0; i<32; i++) {
		out |= ((in >> i)&0x1)<<(31-i);
	}
	return out;
}

uint32_t CRC32::table_entry(uint8_t control) {
	uint32_t reg = (m_refin ? control : control << 24);
	for(int i = 0; i<8; i++ ) {
		bool pop = reg & (m_refin ? 0x00000001 : 0x80000000);
		if( m_refin ) { reg >>= 1; } else { reg <<= 1; }
		if( pop ) reg ^= this->m_poly;
	}
	return reg;
}

void CRC32::init() {
	m_register = m_init;
}

void CRC32::append(const unsigned char* bytes, size_t nbytes) {
	assert(bytes != NULL);
	if( nbytes == 0 ) return;
	while( nbytes-- ) {
		m_register = (m_refin ? m_register >> 8 : m_register << 8)
			^ m_table[ ((m_register >> (m_refin ? 0 : 24))&0xff) ^ *bytes++ ];
	}
}

uint32_t CRC32::crc() {
	return (m_refin ^ m_refout ? reflect(m_register) : m_register) ^ m_xorout;
}
