
#ifndef GY_85_h
#define GY_85_h

//----------addresses----------//

#define ADXL345 (0x53)         // Device address as specified in data sheet //ADXL345 accelerometer
#define DATAX0  (0x32)         //X-Axis Data 0
//#define DATAX1 0x33          //X-Axis Data 1
//#define DATAY0 0x34          //Y-Axis Data 0
//#define DATAY1 0x35          //Y-Axis Data 1
//#define DATAZ0 0x36          //Z-Axis Data 0
//#define DATAZ1 0x37          //Z-Axis Data 1
#define HMC5883 (0x1E)         //gyro
#define ITG3200 (0x68)         //compass

#define GYRO_CALIBRATION_ITERATIONS (1500)
#define GYRO_SENSITIVITY_RANGE (2000) // deg/sec
#define GYRO_SENSITIVITY (14.375)
#define GYRO_ANGLE_DEGREE_CONVERSION (0.0000347826) // (1 / (GYRO_SENSITIVITY_RANGE / GYRO_SENSITIVITY))


//----------------------------------------

void ADXL345_Init();
void ADXL345_Read();

double ADXL345_GetX(void);
double ADXL345_GetY(void);
double ADXL345_GetZ(void);

//----------------------------------------

void HMC5883L_Init();
void HMC5883L_Read();

double HMC5883L_GetX(void);
double HMC5883L_GetY(void);
double HMC5883L_GetZ(void);

//----------------------------------------

void ITG3200_Init();
void ITG3200_Calibrate();
void ITG3200_Read();

double ITG3200_GetX(void);
double ITG3200_GetY(void);
double ITG3200_GetZ(void);
double ITG3200_GetTemp(void);

#endif
