// ADXL345.h
// accel.h
#ifndef _ADXL345_H_
#define _ADXL345_H_

#define ADXL345_ADDR (0xA6)

void InitializeADXL345(void);
void ADXL345_UpdateData(void);
float ADXL345_GetX(void);

float ADXL345_GetY(void);

float ADXL345_GetZ(void);

#endif
