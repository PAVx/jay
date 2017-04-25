#include <Wire.h>
#include "GY_85.h"

//Pin config
#define D6T_addr 0x0A
#define D6T_cmd 0x4C
uint8_t led = 0;
GY_85 GY85;     //create the object

#define D6T_addr 0x0A // Address of OMRON D6T is 0x0A in hex
#define D6T_cmd 0x4C // Standard command is 4C in hex

int numbytes = 19; 
int numel = 8;
int rbuf[19]; // Actual raw data is coming in as 35 bytes. Hence the needed for WireExt so that we can handle more than 32 bytes
int tdata[8]; // The data comming from the sensor is in 8 elements
float t_PTAT;


void setup()
{
   pinMode(13, OUTPUT);
 Wire.begin();
  Serial.begin(9600);
  GY85.init();

}
void loop()
{

int i;
      // Step one - send commands to the sensor
      Wire.beginTransmission(D6T_addr);
      Wire.write(D6T_cmd);
      Wire.endTransmission();

      delay(70); // Delay between instruction and data acquisition

      // Read from the sensor
      // Need to send a read command here - but how will this work with 7 bit addressing?
      Wire.requestFrom(D6T_addr,numbytes); // D6T-8 returns 19 bytes 

      // Receive the data

      if (0 <= Wire.available()) { // If there is data still left in buffer
        i = 0;
        for (i=0; i < numbytes; i++) {
          rbuf[i] = Wire.read();
        }
        t_PTAT = (rbuf[0] + (rbuf[1] << 8) ) * 0.1;
        for (i = 0; i < numel; i++) {
          tdata[i] = (rbuf[(i*2+2)] + (rbuf[(i*2+3)] << 8 )) * 0.1;
          Serial.print(tdata[i]);
          Serial.print(",");
        }
        Serial.print("\n");
      }

          delay(100);          



  
    int ax = GY85.accelerometer_x( GY85.readFromAccelerometer() );
    int ay = GY85.accelerometer_y( GY85.readFromAccelerometer() );
    int az = GY85.accelerometer_z( GY85.readFromAccelerometer() );
    
    int cx = GY85.compass_x( GY85.readFromCompass() );
    int cy = GY85.compass_y( GY85.readFromCompass() );
    int cz = GY85.compass_z( GY85.readFromCompass() );

    float gx = GY85.gyro_x( GY85.readGyro() );
    float gy = GY85.gyro_y( GY85.readGyro() );
    float gz = GY85.gyro_z( GY85.readGyro() );
    float gt = GY85.temp  ( GY85.readGyro() );
    
    Serial.print  ( "accelerometer" );
    Serial.print  ( " x:" );
    Serial.print  ( ax );
    Serial.print  ( " y:" );
    Serial.print  ( ay );
    Serial.print  ( " z:" );
    Serial.print  ( az );
    
    Serial.print  ( "  compass" );
    Serial.print  ( " x:" );
    Serial.print  ( cx );
    Serial.print  ( " y:" );
    Serial.print  ( cy );
    Serial.print  (" z:");
    Serial.print  ( cz );
    
    Serial.print  ( "  gyro" );
    Serial.print  ( " x:" );
    Serial.print  ( gx );
    Serial.print  ( " y:" );
    Serial.print  ( gy );
    Serial.print  ( " z:" );
    Serial.print  ( gz );
    Serial.print  ( " gyro temp:" );
    Serial.println( gt );
    
    delay(500);             // only read every 0,5 seconds, 10ms for 100Hz, 20ms for 50Hz

}

