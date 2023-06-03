#include "pinConfig.h"
#include <SPI.h>
#include "seeed_line_chart.h" //include the library
#include "MCP48FEB28.h"

#include "analogMeter.h"
TFT_eSprite analogMeter_spr = TFT_eSprite(&tft); // Sprite
TFT_eSprite analogMeter_spr2 = TFT_eSprite(&tft);
TFT_eSprite spr0 = TFT_eSprite(&tft); // Sprite
TFT_eSprite spr1 = TFT_eSprite(&tft); // Sprite
TFT_eSprite spr2 = TFT_eSprite(&tft); // Sprite
#include"Free_Fonts.h" //include the header file

volatile uint16_t dac_val = 0;

void keyAPressed(){
  static unsigned long t0 = millis();
  unsigned long t1 = millis();
  if((t1-t0)>100){
//    dac_val++;
//    if(dac_val>4095){
//      dac_val = 4095;
//    }
    t0 = t1;
  }
}

void keyCPressed(){
  static unsigned long t0 = millis();
  unsigned long t1 = millis();
  if((t1-t0)>100){
//    if(dac_val>0){
//      dac_val--;
//    }
    t0 = t1;
  }
}

MCP48FEB28 *dac;

void setup() {
  Serial.begin(115200);
      
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);

  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(WIO_KEY_A), keyAPressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(WIO_KEY_C), keyCPressed, FALLING);

  dac = new MCP48FEB28(CS,LATCH);
  dac->init();
}

void loop() {
  Serial.print(dac_val);
  Serial.print("\t\t");
  Serial.println(analogRead(LED_SENS));
  if(!digitalRead(WIO_5S_UP)){
    dac_val++;
    if(dac_val>4095){
      dac_val = 4095;
    }
  }else if(!digitalRead(WIO_5S_DOWN)){
    if(dac_val>0){
      dac_val--;
    }
  }
  dac->write(0,dac_val);
  
  delay(100);
}
