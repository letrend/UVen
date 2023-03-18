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

#define TEC_LED0 8
#define TEC_LED1 9
#define TEC_LED0_IN 4
#define TEC_LED0_STATUS 7
#define TEC_LED1_IN 3
#define TEC_LED1_STATUS 6

#define KILL 28

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
unsigned long t0,t1,t2,fire_start_time,last_pressed;
float poly[3][3] = { {8.57991735e-05, -2.05112717e-01,  1.05402649e+02},
{1.29957085e-04, -2.37834294e-01,  1.09390237e+02},
{1.30629020e-04, -2.35951069e-01,  1.08283965e+02} };
int analogValue[3] = {0};
int analogPin[3] = {A0,A1,A2};
float temp_setpoint[3] = {20,20,20};
unsigned long time_setpoint = 0;
int temp_sp = 0; // temporary setpoint
enum{
  START,
  INTENSITY,
  TEMP0,
  TEMP1,
  TEMP2,
  TIME
}MENU_MODE;
uint8_t menu_mode = START;

float calcTemp(int val, float *p){
  return p[0]*val*val+p[1]*val+p[2];
}

void drawTemperature() {
  tft.setTextSize(6);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  if(temp[0].data > temp_setpoint[0]){
    tft.setTextColor(ST77XX_YELLOW);
  }else if(temp[0].data > 50){
    tft.setTextColor(ST77XX_RED);
  }else{
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.print(temp[0].data);
  tft.setCursor(0, 60);
  if(temp[1].data > temp_setpoint[1]){
    tft.setTextColor(ST77XX_YELLOW);
  }else if(temp[1].data > 50){
    tft.setTextColor(ST77XX_RED);
  }else{
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.print(temp[1].data);
  tft.setCursor(0, 120);
  if(temp[2].data > temp_setpoint[2]){
    tft.setTextColor(ST77XX_YELLOW);
  }else if(temp[2].data > 50){
    tft.setTextColor(ST77XX_RED);
  }else{
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.print(temp[2].data);
}

void drawTime() {
  tft.setTextSize(4);
  tft.setCursor(0, 200);
  tft.setTextColor(ST77XX_WHITE);
  if(led_fire){
    unsigned long ms, seconds, minutes, hours;
    if(time_setpoint>0){
      ms = time_setpoint - (t1-fire_start_time);
    }else{
      ms = t1-fire_start_time;
    }
    seconds = ms/1000;
    minutes = seconds/60;
    hours = minutes/60;
    char str[20];
    sprintf(str,"%.2d:%.2d:%.2d",hours, minutes, seconds%60);
    tft.print(str);
  }else{
    tft.print(time_setpoint);
  }
}

bool menu_changed = false, ran_once = false;

void menu_change(){
  if((t1-last_pressed)>300){
    last_pressed = t1;
    menu_mode++;
    if(menu_mode>=6){
      menu_mode = START;
    }
    menu_changed = true;
  }
}

void setup() {
  pinMode(LED_0_DISABLE,OUTPUT);
  pinMode(LED_1_DISABLE,OUTPUT);
  
  digitalWrite(LED_0_DISABLE,true);
  digitalWrite(LED_1_DISABLE,true);

  Wire.begin();
  Wire1.begin();

  Wire.beginTransmission(pot_address);
  Wire.write(0); // 
  Wire.endTransmission();

  Wire1.beginTransmission(pot_address);
  Wire1.write(0); // 
  Wire1.endTransmission();
  
  tft.init(240, 240, SPI_MODE2);
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(6);
  tft.setCursor(0, 10);
  tft.print("UVen");
  tft.setCursor(0, 70);
  tft.print("pocket");
  pinMode(ENCODER_BUTTON,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BUTTON), menu_change, FALLING);
  
  pinMode(BUTTON_0,INPUT_PULLUP);
  pinMode(BUTTON_1,INPUT_PULLUP);
  pinMode(BUTTON_2,INPUT_PULLUP);
  pinMode(TEC_LED0_STATUS,INPUT_PULLUP);
  pinMode(TEC_LED1_STATUS,INPUT_PULLUP);
  pinMode(TEC_LED0_IN,OUTPUT);
  pinMode(TEC_LED1_IN,OUTPUT);
  pinMode(KILL,INPUT_PULLUP);
  pinMode(TEC_LED0,OUTPUT);
  pinMode(TEC_LED1,OUTPUT);
  for(int i=0;i<3;i++){
    digitalWrite(TEC_LED0,true);
    digitalWrite(TEC_LED1,false);
    delay(250);
    digitalWrite(TEC_LED0,false);
    digitalWrite(TEC_LED1,true);
    delay(250);
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

int encoder_offset = 0;

void loop() {
  t1 = millis();
  encoder_pos = myEnc.read();
  switch(menu_mode){
    case START:{
      if(menu_changed){
        time_setpoint = temp_sp;
        menu_changed = false;
      }
      if(t1-t2>1000){
        t2 = t1;
        drawTemperature();
        drawTime();
      }
      break;
    }
    case INTENSITY:{
      if(menu_changed){
        encoder_offset = encoder_pos;
        menu_changed = false;
      }
      encoder_pos -= encoder_offset;
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
      }
      break;
    }
    case TEMP0:{
      if(menu_changed){
        encoder_offset = encoder_pos;
        menu_changed = false;
      }
      encoder_pos -= encoder_offset;
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(6);
      tft.setCursor(0, 0);
      temp_sp = temp_setpoint[0]+encoder_pos;
      tft.print(temp_sp);
      break;
    }
    case TEMP1:{
      if(menu_changed){
        temp_setpoint[0] = temp_sp;
        encoder_offset = encoder_pos;
        menu_changed = false;
      }
      encoder_pos -= encoder_offset;
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(6);
      tft.setCursor(0, 60);
      temp_sp = temp_setpoint[1]+encoder_pos;
      tft.print(temp_sp);
      break;
    }
    case TEMP2:{
      if(menu_changed){
        temp_setpoint[1] = temp_sp;
        encoder_offset = encoder_pos;
        menu_changed = false;
      }
      encoder_pos -= encoder_offset;
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(6);
      tft.setCursor(0, 120);
      temp_sp = temp_setpoint[2]+encoder_pos;
      tft.print(temp_sp);
      break;
    }
    case TIME:{
      if(menu_changed){
        temp_setpoint[2] = temp_sp;
        encoder_offset = encoder_pos;
        menu_changed = false;
      }
      encoder_pos -= encoder_offset;
      tft.fillScreen(ST77XX_BLACK);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(4);
      tft.setCursor(0, 200);
      temp_sp = time_setpoint+encoder_pos*1000;
      unsigned long seconds = temp_sp/1000;
      unsigned long minutes = seconds/60;
      unsigned long hours = minutes/60;
      char str[20];
      sprintf(str,"%.2d:%.2d:%.2d",hours, minutes, seconds%60);
      tft.print(str);
      break;
    }
  }

  if(t1-t0>100){
    for(int i=0;i<3;i++){
      analogValue[i] = analogRead(analogPin[i]);  
      temp[i].data = calcTemp(analogValue[i],poly[i]);
      temp_pub[i].publish(&temp[i]);
    }
    t0 = t1;
    nh.spinOnce();

    if(temp[0].data>temp_setpoint[0]){
      digitalWrite(TEC_LED0,true);
    }else{
      digitalWrite(TEC_LED0,false);
    }
    if(temp[1].data>temp_setpoint[1]){
      digitalWrite(TEC_LED0,true);
    }else{
      digitalWrite(TEC_LED0,false);
    }
    if(temp[2].data>temp_setpoint[2]){
      digitalWrite(TEC_LED0_IN,true);
      digitalWrite(TEC_LED1_IN,true);
    }else{
      digitalWrite(TEC_LED0_IN,false);
      digitalWrite(TEC_LED1_IN,false);
    }
  }
  if(digitalRead(BUTTON_0)==0){
    if(time_setpoint>0 ){
      if(!ran_once){
        if(!led_fire){
          fire_start_time = millis();
        }
        if((millis()-fire_start_time)<time_setpoint){
          led_fire = true;
        }else{
          led_fire = false;
          ran_once = true;
        }
      }
    }else{
      led_fire = true;  
    }
  }else{
    ran_once = false;
    led_fire = false;
  }
  
  if(led_fire){
    digitalWrite(LED_0_DISABLE,false);
    digitalWrite(LED_1_DISABLE,false);
    sevseg.setChars("fire");
  }else{
    digitalWrite(LED_0_DISABLE,true);
    digitalWrite(LED_1_DISABLE,true);
    sevseg.setNumber(poti_setpoint);  
  }
  sevseg.refreshDisplay(); // Must run repeatedly
}
