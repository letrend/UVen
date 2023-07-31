#include <Arduino_CRC32.h>
Arduino_CRC32 crc32;
#include "seeed_line_chart.h"
TFT_eSPI tft;

#define BUFFER_SIZE 176

union SERIAL_FRAME{
  struct{
    uint32_t time;
    float temperature[17];
    uint16_t led_fan;
    uint16_t chamber_fan;
    uint16_t target_current[16];
    uint16_t current[16];
    uint16_t gate[16];
    uint32_t crc;
  }values;
  volatile uint8_t data[BUFFER_SIZE];
};

SERIAL_FRAME rx, tx;

void keyAPressed(){
  static unsigned long t0 = millis();
  unsigned long t1 = millis();
  if((t1-t0)>100){
    for(int i=0;i<16;i++){
      if(tx.values.target_current[i]<100){
        tx.values.target_current[i]++;
      }
    }
    t0 = t1;
  }
}

void keyBPressed(){
  for(int i=0;i<16;i++){
    tx.values.target_current[i] = 0;
  }
}

void keyCPressed(){
  static unsigned long t0 = millis();
  unsigned long t1 = millis();
  if((t1-t0)>100){
    for(int i=0;i<16;i++){
      if(tx.values.target_current[i]>0){
        tx.values.target_current[i]--;
      }
    }
    t0 = t1;
  }
}

void setup() {
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);

  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(WIO_KEY_A), keyAPressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(WIO_KEY_B), keyBPressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(WIO_KEY_C), keyCPressed, FALLING);
  
  Serial.begin(4000000);
  tft.begin();
  tft.setRotation(3);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.fillScreen(TFT_WHITE);
  tft.drawString("targ", 10, 5);
  tft.drawString("curr", 80, 5);
  tft.drawString("gate", 150, 5);
  tft.drawString("temp", 220, 5);
}

void drawValues(bool clear=false){
  if(clear){
    tft.setTextColor(TFT_WHITE);
  }else{
    tft.setTextColor(TFT_BLACK);
  }
  char str[20];
  for(int i=0;i<16;i++){
    sprintf(str,"%d",rx.values.target_current[i]);
    tft.drawString(str, 10, 20+i*12);
    sprintf(str,"%d",rx.values.current[i]);
    tft.drawString(str, 80, 20+i*12);
    sprintf(str,"%d",rx.values.gate[i]);
    tft.drawString(str, 150, 20+i*12);
    sprintf(str,"%.1f",rx.values.temperature[i]);
    tft.drawString(str, 220, 20+i*12);
  }
  sprintf(str,"%.1f",rx.values.temperature[16]);
  tft.drawString(str, 220, 20+16*12);
  sprintf(str,"%dms",rx.values.time);
  tft.drawString(str, 10, 20+16*12);
}

void loop() {
  if(Serial.available()){
    drawValues(true);
    Serial.readBytes((char*)&rx.data[0], BUFFER_SIZE);
    uint32_t crc = crc32.calc((uint8_t const *)&rx.data[0], BUFFER_SIZE-4);
    if(crc==rx.values.crc){
      tx.values.crc = crc32.calc((uint8_t const *)&tx.data[0], BUFFER_SIZE-4);
      Serial.write((char*)tx.data,BUFFER_SIZE);
      drawValues();
    }else{
      tft.fillScreen(TFT_RED);
      char str[20];
      sprintf(str,"%x != %x",crc,rx.values.crc);
      tft.drawString(str, 100, 0);
    }
  }
  
}
