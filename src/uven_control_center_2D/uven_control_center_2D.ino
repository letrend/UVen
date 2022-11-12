#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // Arduino SPI library
#include <Encoder.h>

#define FORWARDS 8
#define ENCODER_A 9
#define ENCODER_B 10

#define TFT_CS    30  // define chip select pin
#define TFT_DC    17  // define data/command pin
#define TFT_RST   16  // define reset pin, or set to -1 and connect to Arduino RESET pin
 
// Initialize Adafruit ST7789 TFT library
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

int distance;

Encoder myEnc(ENCODER_A, ENCODER_B);

void setup() {
pinMode(18,OUTPUT);
digitalWrite(18,HIGH);
tft.init(240, 240, SPI_MODE2);tft.setRotation(2);
tft.fillScreen(ST77XX_BLUE);
pinMode(FORWARDS,INPUT_PULLUP);

}

void testdrawtext(int text, uint16_t color, uint8_t size,char *text2) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.setTextWrap(true);
  tft.print(text);
  tft.print(text2);
}

void loop() {

  distance = myEnc.read();
  tft.fillScreen(ST77XX_BLUE);
  testdrawtext(distance, ST77XX_WHITE,6,"penis");
}
