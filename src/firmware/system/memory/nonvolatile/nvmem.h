// eeprom.h
// nvram.h by Sargis Yonan
// April 2017

#include <avr/io.h>
#include <avr/eeprom.h>

// returns address of block memory written to EEPROM
unsigned int nvmem_write_block(void *data_ptr, uint32_t n_bytes);

void nvmem_read_block(unsigned int addr, void *data_ptr, uint32_t n_bytes);