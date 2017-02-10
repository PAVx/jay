#include "nmea.h"
#include "uart.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
// need to init baudrate 9600 maybe in gps
char NMEA_DataBuffer[82];
char test[10] = "123456789\n";
NMEA_DataType NMEA_GetMessage(void) {
    memset(NMEA_DataBuffer, '\0', 82);
    uint8_t i = 0;
    char start_char = '\0';
    // Find start of message
    do {
        start_char = _uart_driver_GetByte();
    } while (start_char != '$');
    // Get message
    do {
        NMEA_DataBuffer[i] = _uart_driver_GetByte();
        if (NMEA_DataBuffer[i] != -1) {

            i++;
        }
    } while ((NMEA_DataBuffer[i-1] != '\n') && (i < 81));
    
    //Debug:_uart_driver_SendBuffer(NMEA_DataBuffer, strlen(NMEA_DataBuffer));
    // Return data type
    if (!strncmp(NMEA_DataBuffer, "GPRMC", 5)) {
        
        return NMEA_TYPE_RMC;    
    }
    else {
        return NMEA_TYPE_ERROR;
    }
}
NMEA_TypeRMC NMEA_ParseRMC(void) {
    char *endptr;
    NMEA_TypeRMC rmc;
    int i = 6;
    rmc.time = strtod (&NMEA_DataBuffer[i], &endptr);

    while(NMEA_DataBuffer[i++] != ',');
    rmc.status = NMEA_DataBuffer[i];
    
    // Get Lat
    while(NMEA_DataBuffer[i++] != ',');
    rmc.Lat = strtod (&NMEA_DataBuffer[i], &endptr);

    while(NMEA_DataBuffer[i++] != ',');
    if (NMEA_DataBuffer[i] == 'S' || NMEA_DataBuffer[i] == 'W') {
        rmc.Lat *= -1;
    }
    
    // Get Long
    while(NMEA_DataBuffer[i++] != ',');
    rmc.Long = strtod (&NMEA_DataBuffer[i], &endptr);

    while(NMEA_DataBuffer[i++] != ',');
    if (NMEA_DataBuffer[i] == 'S' || NMEA_DataBuffer[i] == 'W') {
        rmc.Long *= -1;
    }
    
    // Get Speed
    while(NMEA_DataBuffer[i++] != ',');
    rmc.speed = strtod (&NMEA_DataBuffer[i], &endptr);

    // Get Track Angle
    while(NMEA_DataBuffer[i++] != ',');
    rmc.angle = strtod (&NMEA_DataBuffer[i], &endptr);

    // Get Date
    while(NMEA_DataBuffer[i++] != ',');
    rmc.date = strtod (&NMEA_DataBuffer[i], &endptr);

    // Get Mag
    while(NMEA_DataBuffer[i++] != ',');
    rmc.Mag = strtod (&NMEA_DataBuffer[i], &endptr);

    while(NMEA_DataBuffer[i++] != ',');
    if (NMEA_DataBuffer[i] == 'S' || NMEA_DataBuffer[i] == 'W') {
        rmc.Mag *= -1;
    }

    while(NMEA_DataBuffer[i++] != '*');

    return rmc;

}

