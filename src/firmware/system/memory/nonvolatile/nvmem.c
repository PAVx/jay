/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Niraj Raniga
	April 2017
*/

#include "nvmem.h"

static unsigned int _addr = 0x00000000;

// returns address of block memory written to EEPROM
unsigned int nvmem_write_block(void *data_ptr, uint32_t n_bytes) {
	eeprom_write_block(data_ptr, (void*)_addr, n_bytes);

	_addr += n_bytes;
	return (_addr - n_bytes);
}

void nvmem_read_block(unsigned int addr, void *data_ptr, uint32_t n_bytes) {
	eeprom_read_block (data_ptr, (void*)addr, n_bytes);
}
