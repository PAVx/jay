/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>

uint64_t double_to_binary(double val);
double binary_to_double(uint64_t bin);
void double_to_byte_array(double val, uint8_t buff[8]);
double byte_array_to_double(uint8_t *data, uint8_t pos);

#endif