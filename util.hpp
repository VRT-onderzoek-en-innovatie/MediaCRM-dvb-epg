#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <stdint.h>
#include <time.h>

uint8_t bcd_decode(uint8_t in);
time_t mjd_decode(uint16_t mjd);

#endif // __UTIL_HPP__
