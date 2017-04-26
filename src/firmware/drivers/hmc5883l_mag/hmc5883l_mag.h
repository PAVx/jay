// hmc5883l_mag.h

#define HMC5883L_WRITE 0x3C
#define HMC5883L_READ 0x3D

void HMC5883L_init(void);
void HMC5883L_Update(void);

// Access Functions
float HMC5883L_GetX(void);
float HMC5883L_GetY(void);
float HMC5883L_GetZ(void);
