#include "util.hpp"
#include <assert.h>
#include <math.h>

uint8_t bcd_decode(uint8_t in) {
	assert( (in&0x0f) <= 0x09 );
	assert( (in&0xf0) <= 0x90 );
	return (in>>4)*10+(in&0x0f);
}

#define UNIX_EPOCH_IN_MJD 40587
time_t mjd_decode(uint16_t mjd) {
	assert( mjd >= UNIX_EPOCH_IN_MJD );
	mjd -= UNIX_EPOCH_IN_MJD;
	return mjd * 86400;
}
