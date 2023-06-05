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

  pinMode(LED_ENABLE, OUTPUT);
  pinMode(LED_DIAG_ENABLE, OUTPUT);
  pinMode(TEC0_DIAG_ENABLE, OUTPUT);
  pinMode(TEC1_DIAG_ENABLE, OUTPUT);
  digitalWrite(LED_ENABLE, true);
  digitalWrite(LED_DIAG_ENABLE, true);
  digitalWrite(TEC0_DIAG_ENABLE, false);
  digitalWrite(TEC1_DIAG_ENABLE, false);

  attachInterrupt(digitalPinToInterrupt(WIO_KEY_A), keyAPressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(WIO_KEY_C), keyCPressed, FALLING);

  dac = new MCP48FEB28(CS,LATCH);
  dac->init();
}

volatile uint16_t dac_val = 0;
int32_t target=0;

void loop() {
  Serial.print(target);
  Serial.print("\t\t");
  Serial.print(dac_val);
  Serial.print("\t\t");
  int32_t val = analogRead(LED_SENS);
  Serial.println(val);
  if(!digitalRead(WIO_5S_UP)){
    target+=1;
    if(target>4000){
      target = 4000;
    }
  }else if(!digitalRead(WIO_5S_DOWN)){
    if(target>0){
      target-=1;
    }
  }
  if(dac_val>=0 && dac_val<2500){
    if(val<target){
      dac_val++;
    }else{
      if(dac_val>0){
        dac_val--;  
      }
    }
  }
  dac->write(0,dac_val);
  
  delay(100);
}
