// ADXL345.h
// accel.h
#ifndef _ADXL345_H_
#define _ADXL345_H_

#define ADXL345_ADDR (0x53 << 1)

void InitializeADXL345(void);
void ADXL345_UpdateData(void);

// Returns in unit g
double ADXL345_GetX(void);

double ADXL345_GetY(void);

double ADXL345_GetZ(void);

double Gyro_ADXL345_GetX(void);

double Gyro_ADXL345_GetY(void);

double Gyro_ADXL345_GetZ(void);

double Mag_ADXL345_GetX(void);

double Mag_ADXL345_GetY(void);

double Mag_ADXL345_GetZ(void);

#endif
