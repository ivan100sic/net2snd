#ifndef MYSOUNDLIB_BYTES
#define MYSOUNDLIB_BYTES

const double PI = 3.14159265358979323;
const double TAU = 2 * PI;

uint32_t i16_to_u32(int16_t x) {
	uint16_t y = x;
	uint32_t z = y;
	return z * uint32_t(65537);
}

int16_t u32_to_i16(uint32_t x) {
	uint16_t y = x & 0xffff;
	return y;
}

#include <cmath>

void tau(double& x) {
	x = fmod(x, TAU);
}

#endif // MYSOUNDLIB_BYTES