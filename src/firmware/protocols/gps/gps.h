// gps.h
// nmea.h
#ifndef _GPS_H_
#define _GPS_H_
#include <stdbool.h>
#include <time.h>

bool GPS_NewDataReady(void); 
void GPS_UpdateData(void); 
struct tm GPS_GetTime(void); 
double GPS_GetLatitude(void); 
double GPS_GetLongitude(void); 
double GPS_GetAltitude(void); 
double GPS_GetSpeed(void); 
char GPS_GetStatus(void); 

#endif
