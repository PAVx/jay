#include "gps.h"
#include "nmea.h"

static GPS_DATA data;

bool GPS_NewDataReady(void) {
    return Get_newDataReady();
}

void GPS_UpdateData(void) {
    data = Get_gpsData();   
}

struct tm GPS_GetTime(void) {
    struct tm fix;    
    fix.tm_sec = (int)fmod(data.time, 100);
    data.time /= 100;
    fix.tm_min = (int)data.time % 100;
    fix.tm_hour = (int)data.time / 100;
    return fix;
}

double GPS_GetLatitude(void) {
    return data.Lat;
}

double GPS_GetLongitude(void) {
    return data.Long;
}

double GPS_GetAltitude(void) {
    return data.altitude;
}

double GPS_GetSpeed(void) {
    return data.speed;
}

char GPS_GetStatus(void) {
    return data.status;
}
