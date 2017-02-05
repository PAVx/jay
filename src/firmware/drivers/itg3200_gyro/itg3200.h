// ITG3200.h
// gyro.h
#ifndef _ITG3200_H_
#define _ITG3200_H_

#define ITG3200_ADDR (0xD0)

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

#define ITG3200_REG_READ_TIMEOUT (50) //ms


void InitializeITG3200(void);

void ITG3200_UpdateData(void);

float ITG3200_GetX(void);

float ITG3200_GetY(void);

float ITG3200_GetZ(void);

float ITG3200_GetTemp(void);

#endif
