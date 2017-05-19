// d6t8l.h

#ifndef _D6T8L_H_
#define _D6T8L_H_

#define D6T8L_ADDR (0x0A)
#define D6T8L_CMD (0x4C)
#define D6T8L_LEN (8)

void InitializeD6T8L(void);

void D6T8L_UpdateData(void);

uint8_t *D6T8L_GetData(void);
uint8_t D6T8L_GetAvgData(void);

#endif
