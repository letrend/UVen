#include <Tli4970.h>
#include "SevSeg.h"
SevSeg sevseg; //Initiate a seven segment controller object

// Tli4970 Object
Tli4970 current_sensor[5] = {Tli4970(),Tli4970(),Tli4970(),Tli4970(),Tli4970()};

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  // Use custom SPI
  current_sensor[0].begin(SPI, (uint8_t)3u, (uint8_t)8u);
  current_sensor[1].begin(SPI, (uint8_t)4u, (uint8_t)9u);
  current_sensor[2].begin(SPI, (uint8_t)5u, (uint8_t)10u);
  current_sensor[3].begin(SPI, (uint8_t)6u, (uint8_t)11u);
   current_sensor[4].begin(SPI, (uint8_t)7u, (uint8_t)12u);
  // set OCD pin to enable it
   current_sensor[0].setPinOCD((uint8_t)8);
   current_sensor[1].setPinOCD((uint8_t)9);
   current_sensor[2].setPinOCD((uint8_t)10);
   current_sensor[3].setPinOCD((uint8_t)11);
   current_sensor[4].setPinOCD((uint8_t)12);
   pinMode(2,OUTPUT);
   digitalWrite(2,0);

   byte numDigits = 4;  
    byte digitPins[] = {50, 48, 52, 53};
    byte segmentPins[] = {42, 51, 44, 46, 45, 47, 49, 43};
    sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, 0);
    sevseg.setBrightness(150);
}

bool toggle = false;
float poly[4] = { -9.11401328e-07,   1.12277904e-03,  -5.54068598e-01,   1.35816421e+02};
int analogValue[5] = {0}, analogPin[5] = {A0,A1,A2,A3,A4};
float temperature[5] = {0}, current[5] = {0};

float calcTemp(int val){
  float temp = poly[0]*val*val*val+poly[1]*val*val+poly[2]*val+poly[3];
  return temp;
}

// the loop function runs over and over again forever
void loop() {
//  digitalWrite(2,toggle);
//  toggle = !toggle;
  for(int i=0;i<5;i++){
    current[i] = current_sensor[i].getCurrent();
    analogValue[i] = analogRead(analogPin[i]);  
    temperature[i] = calcTemp(analogValue[i]);
  }
  sevseg.setNumber(int(temperature[0]),0);
  sevseg.refreshDisplay(); // Must run repeatedly

}
