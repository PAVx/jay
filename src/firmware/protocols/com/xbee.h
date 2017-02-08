//Justin Lee: Feb 2nd, 2017

#include <stdio.h>
#include <stdlib.h>


// xbee AT commands

int AT_Init(void);

int getModuleName(void);

int setModuleName(void);

int discoverMode(void);

int getSignalStrength(void);

int sendPacket(char *packet);

// HELPER FUNCTIONS
int charLength(char *input);

int charReturn(void);

int xbeefprintf(char *xbeePrint);

int hard_to_soft(void);

char * hard_to_memory(void);
