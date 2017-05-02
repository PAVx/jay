//Macros
#define MAX_POT_VAL 668.0
#define MAX_THROT_VAL 99
#define MAX_ROT_VAL 360.0 
#define ROT_OFFSET MAX_ROT_VAL/2 //if we want -max_left to +max_right 
#define MAX_DRONES 3

//Global Variables
//button code ------------------------------------
int rightUpButtonPin = 3; // itter
int leftUpButtonPin = 4; // reset
int rightUpButtonState = 0;
int prevRightUpButtonState = 0;
int leftUpButtonState = 0;
int prevLeftUpButtonState = 0;
uint8_t dronePicked = 1;

//pot code ---------------------------------------
uint16_t vertLeftPot = 0;//throttle
uint16_t horiLeftPot = 0;//yaw
uint16_t vertRightPot = 0;//pitch
uint16_t horiRightPot = 0;//roll
int16_t throttleVal = 0;
int16_t yawVal = 0;
int16_t pitchVal = 0;
int16_t rollVal = 0;

//pkt code --------------------------------------
uint8_t cntrlPkt[12] = {0};




void setup() {
  Serial.begin(9600);
  pinMode(rightUpButtonPin, INPUT);
  pinMode(leftUpButtonPin, INPUT);
  
  cntrlPkt[0] = 0x1; //SOH
  cntrlPkt[1] = 0x2; //STX
  cntrlPkt[2] = 0x0; //Manual Control
  cntrlPkt[3] = 0x0; //SRC Add
  cntrlPkt[12] = 0x04;
}

void loop() {
  //pot code ---------------------------------------
  vertLeftPot = analogRead(A0);
  horiLeftPot = analogRead(A1);
  vertRightPot = analogRead(A2);
  horiRightPot = analogRead(A3);
  throttleVal = (vertLeftPot / MAX_POT_VAL) * MAX_THROT_VAL;
  yawVal  = ((horiLeftPot) / MAX_POT_VAL) * MAX_ROT_VAL - ROT_OFFSET;  
  pitchVal = ((vertRightPot) / MAX_POT_VAL) * MAX_ROT_VAL - ROT_OFFSET;   
  rollVal = ((horiRightPot) / MAX_POT_VAL) * MAX_ROT_VAL - ROT_OFFSET;Serial.print("t: ");
  Serial.print(throttleVal);
   
  Serial.print("y: ");
  Serial.print(yawVal);
  Serial.print("  ");
  Serial.print("p: ");
  Serial.print(pitchVal);
  Serial.print("r: ");
  Serial.print(rollVal);
  Serial.print("\n");

//button code ------------------------------------
  rightUpButtonState = digitalRead(rightUpButtonPin);
  if ((rightUpButtonState == 0)&&(prevRightUpButtonState == 1)) {
    if(dronePicked < (MAX_DRONES)){
      dronePicked = dronePicked + 1;
    } else {
      dronePicked = 1;
    }
  }
  
  leftUpButtonState = digitalRead(leftUpButtonPin);
  if ((leftUpButtonState == 0)&&(prevLeftUpButtonState == 1)) {
    dronePicked = 1;
  }
  prevRightUpButtonState = rightUpButtonState;
  prevLeftUpButtonState = leftUpButtonState;

  Serial.print("Drone currently selected: ");
  Serial.print(dronePicked);
  Serial.print("\n"); 


  cntrlPkt[4] = dronePicked; //droneX
  cntrlPkt[5] = (yawVal & 0xff00) >> 8;
  cntrlPkt[6] = (yawVal & 0x00ff);
  cntrlPkt[7] = (pitchVal & 0xff00) >> 8;
  cntrlPkt[8] = (pitchVal & 0x00ff);
  cntrlPkt[9] = (rollVal & 0xff00) >> 8;
  cntrlPkt[10] = (rollVal & 0x00ff);
  cntrlPkt[11] = throttleVal & 0x00ff;
  
  for(int i = 0; i < 13; i++)
  {
    Serial.println(cntrlPkt[i]);
  }
  Serial.print("\n"); 
  
  delay(1000);  
}
