// hmc5883l_mag.h

#define HMC5883L_WRITE 0x3C
#define HMC5883L_READ 0x3D 

void HMC5883L_init(void);
float HMC5883L_get(void);