// hmc5883l_mag.h

#include <stdint.h>

#define HMC5883L_WRITE 0x3C
#define HMC5883L_READ 0x3D

void HMC5883L_init(void);
void HMC5883L_Update(void);

// Access Functions
int16_t HMC5883L_GetX(void);
int16_t HMC5883L_GetY(void);
int16_t HMC5883L_GetZ(void);
