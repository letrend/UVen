// Example 20.1

int dt = 2000; // used for delay duration
byte rval = 0x00; // used for value sent to potentiometer
#include "Wire.h"
#define pot_address 0x2F // each I2C object has a unique bus address, the MCP4018 is 0x2F or 0101111 in binary

#define LED_0_ENABLE 14
#define LED_1_ENABLE 15

void setup()
{
  Wire.begin();
  Wire1.begin();
  Serial.begin(9600); 
  pinMode(LED_0_ENABLE,OUTPUT);
  pinMode(LED_1_ENABLE,OUTPUT);
}

bool led_0_fire = false;
bool led_1_fire = false;

void loop()
{
  Wire.beginTransmission(pot_address);
  Wire.write(rval); // 
  Wire.endTransmission();

  Wire1.beginTransmission(pot_address);
  Wire1.write(rval); // 
  Wire1.endTransmission();
  
  delay(1000);
  digitalWrite(LED_0_ENABLE,!led_0_fire);
  digitalWrite(LED_1_ENABLE,!led_1_fire);
  led_0_fire = !led_0_fire;
  led_1_fire = !led_1_fire;
}
