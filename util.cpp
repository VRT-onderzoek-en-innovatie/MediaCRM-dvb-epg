#include "util.hpp"
#include <assert.h>
#include <math.h>

uint8_t bcd_decode(uint8_t in) {
	assert( (in&0x0f) <= 0x09 );
	assert( (in&0xf0) <= 0x90 );
	return (in>>4)*10+(in&0x0f);
}

time_t mjd_decode(uint16_t mjd) {
	struct tm ret;
	
	int ya = ((double)mjd - 15078.2)/365.25;
	int ma = ((double)mjd - 14956.1 - floor(ya * 365.25)) / 30.6001;
	ret.tm_mday = mjd - 14956 - floor(ya*365.25) - floor(ma*30.6001);
	int k = (ma==14 || ma==15 ? 1 : 0);
	ret.tm_year = ya + k;
	ret.tm_mon = ma - 1 - k*12   -1;
	ret.tm_hour = ret.tm_min = ret.tm_sec = 0;

	return mktime(&ret);
}
