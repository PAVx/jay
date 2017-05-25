// ADXL345.h
// accel.h
#ifndef _ADXL345_H_
#define _ADXL345_H_

// ACCEL DEFINES
#define ADXL345_ADDR (0x53 << 1)

// GYRO DEFINES
#define GYRO_CALIBRATION_ITERATIONS (1500)
#define GYRO_SENSITIVITY_RANGE (2000) // deg/sec
#define GYRO_SENSITIVITY (14.375)

#define GYRO_ANGLE_DEGREE_CONVERSION (0.0000347826) // (1 / (GYRO_SENSITIVITY_RANGE / GYRO_SENSITIVITY))

#define ITG3200_ADDR (0x68 << 1)

#define G_SMPLRT_DIV 0x15
#define G_DLPF_FS 0x16
#define G_INT_CFG 0x17
#define G_PWR_MGM 0x3E

#define G_TEMP_REG_HI (0x1B)
#define G_TEMP_REG_LO (0x1C)

#define G_X_REG_HI (0x1D)
#define G_X_REG_LO (0x1E)

#define G_Y_REG_HI (0x1F)
#define G_Y_REG_LO (0x20)

#define G_Z_REG_HI (0x21)
#define G_Z_REG_LO (0x22)

// Mag DEFINES
#define HMC5883L_ADDR 0x3C

void GY85_Init(void);
void GY85_UpdateData(void);

double Accel_GY85_GetX(void);

double Accel_GY85_GetY(void);

double Accel_GY85_GetZ(void);

double Gyro_GY85_GetX(void);

double Gyro_GY85_GetY(void);

double Gyro_GY85_GetZ(void);

double Mag_GY85_GetX(void);

double Mag_GY85_GetY(void);

double Mag_GY85_GetZ(void);

#endif
