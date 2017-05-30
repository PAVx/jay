/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

// mag.c
#include "imu.h"
#include "gy85.h"

void IMU_Init(void) {
	GY85_Init();
}

void IMU_Update(void) {
	GY85_UpdateData();
}

void IMU_Calibrate(void){
	GY85_Calibrate();
}

double Accel_GetX(void) {
	return Accel_GY85_GetX();
}

double Accel_GetY(void) {
	return Accel_GY85_GetY();
}

double Accel_GetZ(void) {
	return Accel_GY85_GetZ();
}

double Gyro_GetX(void) {
	return Gyro_GY85_GetX();
}

double Gyro_GetY(void) {
	return Gyro_GY85_GetY();
}

double Gyro_GetZ(void) {
	return Gyro_GY85_GetZ();
}

double Mag_GetX(void) {
	return Mag_GY85_GetX();
}

double Mag_GetY(void) {
	return Mag_GY85_GetY();
}

double Mag_GetZ(void) {
	return Mag_GY85_GetZ();
}
