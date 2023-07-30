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

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(4000000);
  tft.begin();
  tft.setRotation(3);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.fillScreen(TFT_WHITE);
}

// the loop function runs over and over again forever
void loop() {
  if(Serial.available()){
    Serial.readBytes((char*)&rx.data[0], BUFFER_SIZE);
    uint32_t crc = crc32.calc((uint8_t const *)&rx.data[0], BUFFER_SIZE-4);
    if(crc==rx.values.crc){
      tx.values.crc = crc32.calc((uint8_t const *)&tx.data[0], BUFFER_SIZE-4);
      Serial.write((char*)tx.data,BUFFER_SIZE);
      tft.fillScreen(TFT_GREEN);
      tft.drawString("yes", 100, 0);
    }else{
      tft.fillScreen(TFT_RED);
      char str[20];
      sprintf(str,"%x != %x",crc,rx.values.crc);
      tft.drawString(str, 100, 0);
    }
  }
  
}
