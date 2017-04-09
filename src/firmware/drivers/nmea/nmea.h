// gps.h
// nmea.h
#ifndef _NMEA_H_
#define _NMEA_H_
#include <stdbool.h>
#include <time.h>
#include <math.h>
typedef struct {
    double time;
    char status;
    double Lat;
    double Long;
    double speed;
    double altitude;
    bool newDataReady;
} GPS_DATA;

void NEO6M_GetChar(void);
GPS_DATA Get_gpsData(void);
bool Get_newDataReady(void);


#endif
