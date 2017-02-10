// gps.h
// nmea.h
#ifndef _NMEA_H_
#define _NMEA_H_
#include <stdbool.h>
#include <time.h>

typedef struct {
    double time;
    char status;
    double Lat;
    double Long;
    double speed;
    double angle;
    double date;
    double Mag;
} NMEA_TypeRMC;
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
NMEA_TypeRMC NMEA_ParseRMC(void);


#endif
