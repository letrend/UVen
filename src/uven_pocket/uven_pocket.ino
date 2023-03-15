#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // Arduino SPI library
#include <Encoder.h>
#include "SevSeg.h"
#include "Wire.h"
#define USE_USBCON
#include <ros.h>
#include <std_msgs/Float32.h>

#define ENCODER_BUTTON 11
#define ENCODER_A 12
#define ENCODER_B 13
#define BUTTON_0 41
#define BUTTON_1 40
#define BUTTON_2 39

 #define TFT_DC    36  // define data/command pin
 #define TFT_RST   35  // define reset pin, or set to -1 and connect to Arduino RESET pin
#define LED_0_DISABLE 14
#define LED_1_DISABLE 15

#define TEC_LED0_IN 4
#define TEC_LED0_STATUS 7
#define TEC_LED1_IN 3
#define TEC_LED1_STATUS 6

#define TEC_LEDS 28

bool led_fire = false;
 
 Adafruit_ST7789 tft = Adafruit_ST7789(-1, TFT_DC, TFT_RST);
SevSeg sevseg; 
int encoder_pos = 0;
int poti_setpoint = -1;
#define pot_address 0x2F // each I2C object has a unique bus address, the MCP4018 is 0x2F or 0101111 in binary

Encoder myEnc(ENCODER_A, ENCODER_B);

ros::NodeHandle nh;
std_msgs::Float32 temp[3];
ros::Publisher temp_pub[3]= { ros::Publisher("temp0", &temp[0]), ros::Publisher("temp1", &temp[1]), ros::Publisher("temp2", &temp[2])};
unsigned long t0,t1,t2;
float poly[3][3] = { {8.57991735e-05, -2.05112717e-01,  1.05402649e+02},
{1.29957085e-04, -2.37834294e-01,  1.09390237e+02},
{1.30629020e-04, -2.35951069e-01,  1.08283965e+02} };
int analogValue[3] = {0};
int analogPin[3] = {A0,A1,A2};
float temp_setpoint = 20, temp_gain = 100;

float calcTemp(int val, float *p){
  return p[0]*val*val+p[1]*val+p[2];
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void setup() {
  // Serial.begin(115200);
  // pinMode(18,OUTPUT);
  // digitalWrite(18,HIGH);
   tft.init(240, 240, SPI_MODE2);
   tft.setRotation(2);
   tft.fillScreen(ST77XX_BLACK);
   tft.setTextColor(ST77XX_WHITE);
   tft.setTextSize(6);
  // pinMode(ENCODER_BUTTON,INPUT_PULLUP);

  Wire.begin();
  Wire1.begin();

  pinMode(LED_0_DISABLE,OUTPUT);
  pinMode(LED_1_DISABLE,OUTPUT);
  
  digitalWrite(LED_0_DISABLE,true);
  digitalWrite(LED_1_DISABLE,true);

  Wire.beginTransmission(pot_address);
  Wire.write(0); // 
  Wire.endTransmission();

  Wire1.beginTransmission(pot_address);
  Wire1.write(0); // 
  Wire1.endTransmission();
  
  pinMode(BUTTON_0,INPUT_PULLUP);
  pinMode(BUTTON_1,INPUT_PULLUP);
  pinMode(BUTTON_2,INPUT_PULLUP);
  pinMode(TEC_LED0_STATUS,INPUT_PULLUP);
  pinMode(TEC_LED1_STATUS,INPUT_PULLUP);
  pinMode(TEC_LED0_IN,OUTPUT);
  pinMode(TEC_LED1_IN,OUTPUT);

  pinMode(TEC_LEDS,OUTPUT);
  for(int i=0;i<3;i++){
    digitalWrite(TEC_LEDS,true);
    delay(100);
    digitalWrite(TEC_LEDS,false);
    delay(100);
  }

  byte numDigits = 4;  
  byte digitPins[] = {50, 48, 52, 53};
  byte segmentPins[] = {42, 51, 44, 46, 45, 47, 49, 43};
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, 0);
  sevseg.setBrightness(100);

  nh.initNode();
  for(int i=0;i<3;i++){
    nh.advertise(temp_pub[i]);  
  }
}

void loop() {
  encoder_pos = myEnc.read();
  // Serial.println(distance);
  if(encoder_pos>=0 && encoder_pos<100){
    if(poti_setpoint!=encoder_pos && !led_fire){
      Wire.beginTransmission(pot_address);
      Wire.write(poti_setpoint); // 
      Wire.endTransmission();

      Wire1.beginTransmission(pot_address);
      Wire1.write(poti_setpoint); // 
      Wire1.endTransmission();     
      poti_setpoint = encoder_pos;
    }
    if(digitalRead(BUTTON_0)==0){
      led_fire = true;
      digitalWrite(LED_0_DISABLE,false);
      digitalWrite(LED_1_DISABLE,false);
      sevseg.setChars("fire");
    }else{
      led_fire = false;
      digitalWrite(LED_0_DISABLE,true);
      digitalWrite(LED_1_DISABLE,true);
      sevseg.setNumber(poti_setpoint);  
    }
  }
  sevseg.refreshDisplay(); // Must run repeatedly
  
  t1 = millis();

  if(t1-t0>100){
    for(int i=0;i<3;i++){
      analogValue[i] = analogRead(analogPin[i]);  
      temp[i].data = calcTemp(analogValue[i],poly[i]);
      temp_pub[i].publish(&temp[i]);
    }
    t0 = t1;
    nh.spinOnce();

    if(temp[0].data>temp_setpoint || temp[1].data>temp_setpoint){
      digitalWrite(TEC_LEDS,true);
    }else{
      digitalWrite(TEC_LEDS,false);
    }
    if(temp[2].data>temp_setpoint){
      digitalWrite(TEC_LED0_IN,true);
      digitalWrite(TEC_LED1_IN,true);
    }else{
      digitalWrite(TEC_LED0_IN,false);
      digitalWrite(TEC_LED1_IN,false);
    }
//    if(temp[0].data>temp_setpoint){
//      float error = temp[0].data-temp_setpoint;
//      if(error*temp_gain<1023){
//        analogWrite(TEC_LED0_IN,int(error*temp_gain));
//      }else{
//        analogWrite(TEC_LED0_IN,1023);
//      }
//    }else{
//      analogWrite(TEC_LED0_IN,0);
//    }
//    if(temp[1].data>temp_setpoint){
//      float error = temp[1].data-temp_setpoint;
//      if(error*temp_gain<1023){
//        analogWrite(TEC_LED1_IN,int(error*temp_gain));
//      }else{
//        analogWrite(TEC_LED1_IN,1023);
//      }
//    }else{
//      analogWrite(TEC_LED1_IN,0);
//    }
  }
  if(t1-t2>1000){
    t2 = t1;
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    if(temp[0].data > temp_setpoint){
      tft.setTextColor(ST77XX_YELLOW);
    }else if(temp[0].data > 60){
      tft.setTextColor(ST77XX_RED);
    }else{
      tft.setTextColor(ST77XX_WHITE);
    }
    tft.print(temp[0].data);
    tft.setCursor(0, 60);
    if(temp[1].data > temp_setpoint){
      tft.setTextColor(ST77XX_YELLOW);
    }else if(temp[1].data > 60){
      tft.setTextColor(ST77XX_RED);
    }else{
      tft.setTextColor(ST77XX_WHITE);
    }
    tft.print(temp[1].data);
    tft.setCursor(0, 120);
    if(temp[2].data > temp_setpoint){
      tft.setTextColor(ST77XX_YELLOW);
    }else if(temp[2].data > 60){
      tft.setTextColor(ST77XX_RED);
    }else{
      tft.setTextColor(ST77XX_WHITE);
    }
    tft.print(temp[2].data);
  }
}
