#include "nmea.h"
#include "uart.h"
#include "softuart.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <util/delay.h>

#define NMEA_MAX_LEN 120
#define GPS_DATA(type, field) ((type << 4) | field)
#define NOT_RECORDING 0
#define RECORDING 1
#define CHECKSUM_UPPER 2
#define CHECKSUM_LOWER 3
static GPS_DATA data;
void CalculateDegrees(double *coordinates, char dir);  
bool ValidChecksum(void);
void NMEA_ParseData(SentenceType type);
//i need to init baudrate 9600 maybe in gps
char NMEA_DataBuffer[NMEA_MAX_LEN + 1];
uint8_t CharToHex(char c) {
    if(isxdigit(c)) {
        if(isalpha(c)) {
            return (uint8_t) (c - 'A' + 10);
        } else {
            return (uint8_t) (c - '0');
        }

    }
    return (uint8_t) -1;
}
uint8_t CalculateChecksum(const char* buff) {
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

//Input sentence in form of $...*
void Send(char* sentence, char* out) {
    uint8_t checksum = CalculateChecksum(sentence);
    snprintf(out, 50, "$%s*%2X\n", sentence, checksum);

}
void InitializeNEO6M(void) {
    
    softuart_turn_rx_on(0);
}
void NMEA_Read(char c) {
    static uint8_t i = 0;
    static uint8_t sentence_state = NOT_RECORDING;
    static uint8_t checksum = 0;
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
                    //NMEA_ParseData(NMEA_TYPE_RMC);
                    data.new_data |= 0x3;
                } else if (!strncmp(NMEA_DataBuffer, "GPGGA", 5)) {
                    //NMEA_ParseData(NMEA_TYPE_GGA);
                    data.new_data |= 0x2;
                } else if (!strncmp(NMEA_DataBuffer, "PUBX", 4)) {
                    _uart_driver_SendByte('!');
                    NMEA_ParseData(UBX_TYPE_00);
                    data.new_data = 0x3;
                }
 

            }
        } 
        sentence_state = NOT_RECORDING;
       
    }
   
    
}
void NMEA_ParseData(SentenceType type) {
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
            case GPS_DATA(UBX_TYPE_00, 2):
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
void GPS_UpdateData(void) {
    char c;
    //while(!_uart_driver_ReceiveBufferIsEmpty()){
        c = softuart_getchar(0);
        if (c != -1 ){
            softuart_turn_rx_off(0);
            _uart_driver_SendByte(c);
            softuart_turn_rx_on(0);
            //NMEA_Read(c); 
        }    
   //}
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

