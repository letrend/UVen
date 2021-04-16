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
float temp = 0;
float current[5];

// the loop function runs over and over again forever
void loop() {
//  digitalWrite(2,toggle);
//  toggle = !toggle;
        for(int i=4; i<5; i++) {
                if(current_sensor[i].readOut())
                {
                        Serial.print(i);
                        Serial.print(" FAILED");
                        Serial.println(current_sensor[i].getStatus());
                }
                else
                {
                        current[i] = current_sensor[i].getCurrent();
                        Serial.println(current[i]);
                }
        }
//  temp = 0.99*temp+0.01*analogRead(A0);
//float current[4] = current_sensor[i].getCurrent();
//  sevseg.setNumber(int(temp),0);
//    sevseg.refreshDisplay(); // Must run repeatedly
        delay(500);

}
