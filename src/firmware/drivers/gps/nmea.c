#include "nmea.h"
#include "uart.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define TIMEOUT 20
#define BUFFER_SIZE 90
#define GPS_DATA(type, field) ((type << 4) | field)

GPS_DATA data;
void CalculateDegrees(double *coordinates, char dir);  
bool ValidChecksum(void);
// need to init baudrate 9600 maybe in gps
char NMEA_DataBuffer[BUFFER_SIZE];

void PrintBuffer(char* buffer) {
    int i = 0;
    while (buffer[i] != '\0') {
        _uart_driver_SendByte(buffer[i]);
        i++;
    }
}
NMEA_DataType NMEA_GetMessage(void) {
    memset(NMEA_DataBuffer, '\0', BUFFER_SIZE);
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
            //_uart_driver_SendByte(NMEA_DataBuffer[i]);
            i++;
        }
    } while ((NMEA_DataBuffer[i-1] != '\n') && (i < BUFFER_SIZE));
    // Return data type if valid
    if (ValidChecksum()) {
        if (!strncmp(NMEA_DataBuffer, "GPRMC", 5)) {
            //PrintBuffer(NMEA_DataBuffer);
            return NMEA_TYPE_RMC;    
        } else if (!strncmp(NMEA_DataBuffer, "GPGGA", 5)) {
            //PrintBuffer(NMEA_DataBuffer);
            return NMEA_TYPE_GGA;
        }
    }
    
    return NMEA_TYPE_ERROR;
    
}
void NMEA_ParseData(NMEA_DataType type) {
    char *endptr;
    int i = 0;
    double fix = 0;
    uint8_t data_field = 0;
    while (NMEA_DataBuffer[i] != '*') {
        // Forward to next data field
        while(NMEA_DataBuffer[i++] != ',') {
            if(NMEA_DataBuffer[i] == '*') {break;}
        }
        data_field++;
        // If no data continue
        if(NMEA_DataBuffer[i] == ',') { continue;}

        switch (GPS_DATA(type, data_field)) {
            
            case GPS_DATA(NMEA_TYPE_RMC, 1):
            case GPS_DATA(NMEA_TYPE_GGA, 1):
                // Get time
                fix = strtod(&NMEA_DataBuffer[i], &endptr);
                data.time.tm_sec = (int)fmod(fix, 100);
                fix /= 100;
                data.time.tm_min = (int)fix % 100;
                data.time.tm_hour = (int)fix / 100;
                break;
            case GPS_DATA(NMEA_TYPE_RMC, 2):
                // get status
                data.status = NMEA_DataBuffer[i];
                break;
            case GPS_DATA(NMEA_TYPE_RMC, 3):
            case GPS_DATA(NMEA_TYPE_GGA, 2):
                // Get Lat
                data.Lat = strtod(&NMEA_DataBuffer[i], &endptr);
                while(NMEA_DataBuffer[i++] != ',');
                CalculateDegrees(&(data.Lat), NMEA_DataBuffer[i]);
                break;
            case GPS_DATA(NMEA_TYPE_RMC, 4):
            case GPS_DATA(NMEA_TYPE_GGA, 3):
                // Get Long
                data.Long = strtod (&NMEA_DataBuffer[i], &endptr);
                while(NMEA_DataBuffer[i++] != ',');
                CalculateDegrees(&(data.Long), NMEA_DataBuffer[i]);
                break;
            case GPS_DATA(NMEA_TYPE_RMC, 5):
                // Get Speed
                data.speed = strtod (&NMEA_DataBuffer[i], &endptr);
                break;
            case GPS_DATA(NMEA_TYPE_GGA, 7):
                // Get Altitude
                data.altitude = strtod (&NMEA_DataBuffer[i], &endptr);
                break;
            default:
                break;
        }               
                
    }
}

void GPS_UpdateData(void) {
    uint8_t counter = 0;
    while ((NMEA_GetMessage() != NMEA_TYPE_RMC) && !(counter++ > TIMEOUT));  
    NMEA_ParseData(NMEA_TYPE_RMC);
    counter = 0;
    while ((NMEA_GetMessage() != NMEA_TYPE_GGA) && !(counter++ > TIMEOUT));  
    NMEA_ParseData(NMEA_TYPE_GGA);

}

GPS_DATA GPS_GetData(void) {
    return data;
}
void CalculateDegrees(double *coordinates, char dir) {   
    int deg;
    double min;
    deg = *coordinates / 100;
    min = fmod(*coordinates, 100);
    *coordinates = deg + min/60;

    if (dir == 'S' || dir == 'W') {
        *coordinates *= -1;
    }
}
bool ValidChecksum(void) {
    uint8_t i = 0;
    char check_str[3];
    uint8_t checksum = NMEA_DataBuffer[0];
    memset(check_str, '\0', 3);
    while (NMEA_DataBuffer[++i] != '*') {
        checksum ^= NMEA_DataBuffer[i];
    }
    // check checksum
    sprintf(check_str, "%2X", checksum);

    if (!strncmp(&NMEA_DataBuffer[++i], check_str, 2)) {
        return true;
    } else {
        return false;
    }
    
}

