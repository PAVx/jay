#include "nmea.h"
#include "uart.h"
#include "softuart.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define NMEA_MAX_LEN 85
#define GPS_DATA(type, field) ((type << 4) | field)
typedef enum {
    NOT_RECORDING,
    RECORDING,
    CHECKSUM_UPPER,
    CHECKSUM_LOWER 
} SentenceState;

typedef enum {
    NMEA_TYPE_GSV,
    NMEA_TYPE_RMC,
    NMEA_TYPE_GSA,
    NMEA_TYPE_GGA,
    NMEA_TYPE_GLL,
    NMEA_TYPE_VTG,
    UBX_TYPE_00,
    NMEA_TYPE_ERROR
} SentenceType;

static GPS_DATA data;
static char NMEA_DataBuffer[NMEA_MAX_LEN];

static void NMEA_Read(char c); 
static void NMEA_ParseData(SentenceType type);
static void CalculateDegrees(double *coordinates, char dir);  
static uint8_t CharToHex(char c); 
static uint8_t CalculateChecksum(const char* buff);

void NEO6M_GetChar(void) {
    char c;
    c = softuart_getchar(0);
    if (c != -1 ){
        //_uart_driver_SendByte(c);
        NMEA_Read(c); 
    }    
}

static void NMEA_Read(char c) {
    static uint8_t i = 0;
    static SentenceState sentence_state = NOT_RECORDING;
    static uint8_t checksum = 0;
    static double curr_time;
    if (c == '$') {
        i = 0;
        memset(NMEA_DataBuffer, '\0', NMEA_MAX_LEN);
        sentence_state = RECORDING;

    } else if (sentence_state == RECORDING) {
        NMEA_DataBuffer[i++] = c;
        if (i > NMEA_MAX_LEN) {
            // error
            sentence_state = NOT_RECORDING;
        }
 
        if (c == '*') {
            sentence_state = CHECKSUM_UPPER;

        }
    } else if (sentence_state == CHECKSUM_UPPER) {
        uint8_t x = CharToHex(c);
        if (x != 0xFF) {
            checksum = x << 4;
            sentence_state = CHECKSUM_LOWER;
        } else {
            sentence_state = NOT_RECORDING;
        }

    } else if (sentence_state == CHECKSUM_LOWER) {
        uint8_t x = CharToHex(c);
        if (x != 0xFF) {
            checksum |= x;
            if (checksum == CalculateChecksum(NMEA_DataBuffer)) {
                if (!strncmp(NMEA_DataBuffer, "GPRMC", 5)) {
                    data.newDataReady = false;
                    NMEA_ParseData(NMEA_TYPE_RMC);
                    curr_time = data.time;
                } else if (!strncmp(NMEA_DataBuffer, "GPGGA", 5)) {
                    NMEA_ParseData(NMEA_TYPE_GGA);
                    if (curr_time == data.time) {
                        data.newDataReady = true;
                    }
                } 
            }
        } 
        sentence_state = NOT_RECORDING;
    }
}

static void NMEA_ParseData(SentenceType type) {
    char *endptr;
    int i = 0;
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
            case GPS_DATA(UBX_TYPE_00, 2):
                // Get time
                data.time = strtod(&NMEA_DataBuffer[i], &endptr);
                break;
            case GPS_DATA(NMEA_TYPE_RMC, 2):
                // get status
                data.status = NMEA_DataBuffer[i];
                break;
            case GPS_DATA(NMEA_TYPE_RMC, 3):
            case GPS_DATA(NMEA_TYPE_GGA, 2):
            case GPS_DATA(UBX_TYPE_00, 3):
                // Get Lat
                data.Lat = strtod(&NMEA_DataBuffer[i], &endptr);
                while(NMEA_DataBuffer[i++] != ',');
                CalculateDegrees(&(data.Lat), NMEA_DataBuffer[i]);
                break;
            case GPS_DATA(NMEA_TYPE_RMC, 4):
            case GPS_DATA(NMEA_TYPE_GGA, 3):
            case GPS_DATA(UBX_TYPE_00, 4):
                // Get Long
                data.Long = strtod (&NMEA_DataBuffer[i], &endptr);
                while(NMEA_DataBuffer[i++] != ',');
                CalculateDegrees(&(data.Long), NMEA_DataBuffer[i]);
                break;
            case GPS_DATA(NMEA_TYPE_RMC, 5):
            case GPS_DATA(UBX_TYPE_00, 9):
                // Get Speed
                data.speed = strtod (&NMEA_DataBuffer[i], &endptr);
                break;
            case GPS_DATA(NMEA_TYPE_GGA, 7):
            case GPS_DATA(UBX_TYPE_00, 5):
                // Get Altitude
                data.altitude = strtod (&NMEA_DataBuffer[i], &endptr);
                break;
            default:
                break;
        }               
                
    }
}

static uint8_t CharToHex(char c) {
    if(isxdigit(c)) {
        if(isalpha(c)) {
            return (uint8_t) (c - 'A' + 10);
        } else {
            return (uint8_t) (c - '0');
        }

    }
    return (uint8_t) -1;
}

static uint8_t CalculateChecksum(const char* buff) {
    uint8_t checksum = 0;
    int i = 0;
    if(buff[i] == '$') {
        i++; //skip $
    }
    int bytes = strlen(buff);
    for (;i<bytes && buff[i]!='*';i++) {
        checksum ^= buff[i];
    }
    return checksum;
}

static void CalculateDegrees(double *coordinates, char dir) {   
    int deg;
    double min;
    deg = *coordinates / 100;
    min = fmod(*coordinates, 100);
    *coordinates = deg + min/60;

    if (dir == 'S' || dir == 'W') {
        *coordinates *= -1;
    }
}

bool Get_newDataReady(void) {
    return data.newDataReady;    
}

GPS_DATA Get_gpsData(void) {
    data.newDataReady = false;
    return data;
}

