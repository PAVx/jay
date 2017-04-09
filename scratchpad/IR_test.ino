#include <Wire.h>
//#include <WireExt.h>
//Pin config
#define D6T_addr 0x0A
#define D6T_cmd 0x4C
int rbuf[35];
int tdata[16];
float t_PTAT;
uint8_t led = 0;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
}
uint8_t a[9];
void loop()
{
  led ^= led;
  led &= 0x01;
  digitalWrite(13, led);
  Wire.beginTransmission(D6T_addr);
  Wire.write(0);
  Wire.endTransmission();
  // Request data
  Wire.requestFrom(D6T_addr, 8);
  int i;
  for (i = 0; i < 8; i++){
    a[i] = Wire.read();
    Serial.print(a[i]);
    Serial.print(" ");
  }
  //a[i] = '/0';
  Serial.println("\n.\n");
}

