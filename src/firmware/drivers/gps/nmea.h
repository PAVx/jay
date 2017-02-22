// gps.h
// nmea.h
#ifndef _NMEA_H_
#define _NMEA_H_
#include <stdbool.h>
#include <time.h>

typedef struct {
    int number;
    struct tm time;
    char status;
    double Lat;
    double Long;
    double speed;
    double altitude;
} GPS_DATA;
typedef enum {
    NMEA_TYPE_GSV,
    NMEA_TYPE_RMC,
    NMEA_TYPE_GSA,
    NMEA_TYPE_GGA,
    NMEA_TYPE_GLL,
    NMEA_TYPE_VTG,
    NMEA_TYPE_ERROR
} NMEA_DataType;

NMEA_DataType NMEA_GetMessage(void);
void NMEA_ParseData(NMEA_DataType type);
void GPS_UpdateData(void);
GPS_DATA GPS_GetData(void);

#endif
