#include "gps_driver.h"
#include "nmea.h"
#include <stdbool.h>
#include <string.h>

NMEA_TypeRMC rmc_data;

void gpsInitialize(void) {
    // set baudrate to 9600
    // initialize uart
}
bool UpdateGPS(void) {
    NMEA_DataType data_type;
    data_type = NMEA_GetMessage();
    if (data_type == NMEA_TYPE_RMC) {
        NMEA_ParsRMC().time;
    }

}
