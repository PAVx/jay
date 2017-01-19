// mag.c
#include "mag.h"
#include "hmc5883l_mag.h"

void InitializeMag(void) {
	HMC5883L_init();
}

float Mag_Get(void) {
	return HMC5883L_get();
}