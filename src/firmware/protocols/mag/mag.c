/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

// mag.c
#include "mag.h"
#include "hmc5883l_mag.h"

void InitializeMag(void) {
	HMC5883L_init();
}

void Mag_Update(void) {
	HMC5883L_Update();
}

double Mag_GetX(void) {
	return HMC5883L_GetX();
}

double Mag_GetY(void) {
	return HMC5883L_GetY();
}

double Mag_GetZ(void) {
	return HMC5883L_GetZ();
}
