#include "types.h"

uint64_t double_to_binary(double val) {
	unsigned long long *double_as_bin = (unsigned long long *)&val;

	return ((uint64_t)*double_as_bin);
}

double binary_to_double(uint64_t bin) {
	double *bin_as_double = (double *)&bin;

	return ((double)*bin_as_double);
}

void double_to_byte_array(double val, uint8_t buff[8]) {
	uint64_t bin = double_to_binary(val);
	uint8_t i = 0;

	for (i = 0; i < 8; i++) {
		buff[i] = (bin >> 8 * (i + 1)) & 0x00000000000000FF;
	}

}

double byte_array_to_double(uint8_t *data, uint8_t pos) {
	uint64_t bin = 0x0000000000000000;
	uint8_t parse_count = 0;
	
	for (parse_count = 0; parse_count < 8; parse_count++) {
		bin |= (uint64_t)(data[pos + parse_count]) << (8 * parse_count);	
	}
	return binary_to_double(bin);
}