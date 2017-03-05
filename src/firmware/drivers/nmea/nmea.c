#include "nmea.h"
#include "uart.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define NMEA_MAX_LEN 84
#define GPS_DATA(type, field) ((type << 4) | field)

static GPS_DATA data;
void CalculateDegrees(double *coordinates, char dir);  
bool ValidChecksum(void);
void NMEA_ParseData(NMEA_DataType type);
//i need to init baudrate 9600 maybe in gps
char NMEA_DataBuffer[NMEA_MAX_LEN + 1];

void PrintBuffer(char* buffer) {
    int i = 0;
    while (buffer[i] != '\0') {
        _uart_driver_SendByte(buffer[i]);
        i++;
    }
}
void NMEA_Read(char c) {
    static uint8_t i = 0;
    static bool recording = 0;
    if (c == '$') {
        i = 0;
        memset(NMEA_DataBuffer, '\0', NMEA_MAX_LEN + 1);
        recording = true;

    } else if (recording) {
        NMEA_DataBuffer[i++] = c;
        if (c == '\n') {
            // Return data type if valid
            if (ValidChecksum()) {
                if (!strncmp(NMEA_DataBuffer, "GPRMC", 5)) {
                    NMEA_ParseData(NMEA_TYPE_RMC);
                    data.new_data |= 0x1;
                } else if (!strncmp(NMEA_DataBuffer, "GPGGA", 5)) {
                    NMEA_ParseData(NMEA_TYPE_GGA);
                    data.new_data |= 0x2;
                }
            
            }
        }
        if (i > NMEA_MAX_LEN) {
            // error
            recording = false;
        }
    }
    
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
    char c;
    c = _uart_driver_GetByte();
    if (c != -1 ){
       NMEA_Read(c); 
    }    
}

bool GPS_IsDataReady(void) {
    if(data.new_data == 0x3) {
        return true;
    }else {
        return false;
    }
}

GPS_DATA GPS_GetData(void) {
    data.new_data = 0;
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
        if (i > NMEA_MAX_LEN) {
            return false;
        }
    }
    // check checksum
    sprintf(check_str, "%2X", checksum);

    if (!strncmp(&NMEA_DataBuffer[++i], check_str, 2)) {
        return true;
    } else {
        return false;
    }
    
}

