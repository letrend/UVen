#include <Tli4970.h>
#include "SevSeg.h"
#define USE_USBCON
#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Empty.h>
#include <std_msgs/Bool.h>
#include <Adafruit_NeoPixel.h>

SevSeg sevseg; 
Tli4970 current_sensor[5] = {Tli4970(),Tli4970(),Tli4970(),Tli4970(),Tli4970()};

#define BUTTON0 40
#define BUTTON1 41
//#define BUTTON2 39
int buttons[2] = {BUTTON0, BUTTON1};

#define LED 2
#define POTI A9
#define NEOPIXEL_PIN    39
Adafruit_NeoPixel strip(1, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ400);

// set endless_loop = true results in
// 1) fire for firetime period
// 2) delay 10 seconds
// 3) repeat 1)
bool endless_loop = true, cool_down = true, cool_down_activated = true; 
unsigned long t0,t1,fire_time=0,t2,elapsed_time,t3,t4;

enum STATES{
  IDLE,
  ARMED,
  FIRE,
  ERROR,
  LID_OPEN
};

int state = IDLE;

void buttonChange(){
  bool b0 = digitalRead(BUTTON0);
  bool b1 = digitalRead(BUTTON1);
  if(b0 && !b1){ //off
    state = IDLE;
  }else if(b0 && b1){ 
    state = ARMED;
  }else if(!b0 && b1){
    state = FIRE;
  }
}

ros::NodeHandle nh;
std_msgs::Float32 temp[5], current[5];
std_msgs::Int32 over_current, over_temperature;
std_msgs::Bool button[3];
std_msgs::Empty toggle_msg;
ros::Publisher over_current_pub("over_current", &over_current);
ros::Publisher over_temperature_pub("over_temperature", &over_temperature);
ros::Publisher temp_pub[5]= { ros::Publisher("temp0", &temp[0]),ros::Publisher("temp1", &temp[1]),
                              ros::Publisher("temp2", &temp[2]),ros::Publisher("temp3", &temp[3]),
                              ros::Publisher("temp4", &temp[4])};
ros::Publisher current_pub[5]= { ros::Publisher("current0", &current[0]),ros::Publisher("current1", &current[1]),
                              ros::Publisher("current2", &current[2]),ros::Publisher("current3", &current[3]),
                              ros::Publisher("current4", &current[4])};
ros::Publisher button_pub[2] = {ros::Publisher("button0", &button[0]), ros::Publisher("button1", &button[1])};                              
//ros::Subscriber<std_msgs::Empty> more_torque_sub("more_torque", &moreTorque );

void setup() {
  pinMode(BUTTON0,INPUT_PULLUP);
  pinMode(BUTTON1,INPUT_PULLUP);
  pinMode(A9,INPUT);
  
  byte numDigits = 4;  
  byte digitPins[] = {50, 48, 52, 53};
  byte segmentPins[] = {42, 51, 44, 46, 45, 47, 49, 43};
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, 0);
  sevseg.setBrightness(100);
  
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
  pinMode(LED,OUTPUT);
  digitalWrite(LED,0);
  
  t0 = millis();
  attachInterrupt(digitalPinToInterrupt(BUTTON0), buttonChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON1), buttonChange, CHANGE);

  nh.initNode();
  nh.advertise(over_current_pub);
  nh.advertise(over_temperature_pub);
  for(int i=0;i<5;i++){
    nh.advertise(temp_pub[i]);  
    nh.advertise(current_pub[i]);  
  }
  for(int i=0;i<2;i++){
    nh.advertise(button_pub[i]);  
  }
  strip.begin();
}

float poly[4] = { -9.11401328e-07,   1.12277904e-03,  -5.54068598e-01,   1.35816421e+02};
int analogValue[5] = {0}, analogPin[5] = {A0,A1,A2,A3,A4};

float calcTemp(int val){
  return poly[0]*val*val*val+poly[1]*val*val+poly[2]*val+poly[3];
}

bool armed_and_ready = false, over_current_flag = false, over_temperature_flag = false;

void loop() {
  t1 = millis();
  
  if(t1-t0>100){
    for(int i=0;i<5;i++){
      if(!current_sensor[i].readOut()){
        current[i].data = current_sensor[i].getCurrent();
      }else{
        current[i].data = -1000;
      }
      analogValue[i] = analogRead(analogPin[i]);  
      temp[i].data = temp[i].data*0.9+ 0.1*calcTemp(analogValue[i]);
      temp_pub[i].publish(&temp[i]);
      current_pub[i].publish(&current[i]);
    }
    for(int i=0;i<2;i++){
      button[i].data = !digitalRead(buttons[i]);
      button_pub[i].publish(&button[i]);
    }

    t0 = t1;
    nh.spinOnce();
  }

  if(state==IDLE){
    fire_time = int(fire_time*0.9f+0.1f*(float(analogRead(A9))-55.0f)*10);
    sevseg.setNumber(fire_time);
    armed_and_ready = false;
    pinMode(LED,OUTPUT);
    digitalWrite(LED,0); 
    strip.setPixelColor(0, 255, 0, 0, 0);
    strip.show(); 
  }else if(state==ARMED){
    armed_and_ready = true;
    sevseg.setNumber(fire_time);
    pinMode(LED,OUTPUT);
    digitalWrite(LED,0); 
    if(t1-t3>1000){
      t3 = t1;
    }else if(t1-t3<200){
      strip.setPixelColor(0, 0, 255, 0, 0);
      strip.show(); 
    }else{
      strip.setPixelColor(0, 0, 0, 0, 0);
      strip.show(); 
    }
  }else if(state==FIRE){    
    if(armed_and_ready){ // we just fired
      pinMode(LED,INPUT_PULLUP);// fire!
      t2 = millis();
      armed_and_ready = false;
    }
    elapsed_time = millis()-t2;
    // over current and over temperature check
    for(int i=0;i<5;i++){
      if(current[i].data>4.0){
        over_current_flag = true;
        over_current.data = i;
      }
      if(temp[i].data>30){
        over_temperature_flag = true;
        over_temperature.data = i;
      }
    }
    if(!digitalRead(LED)){ // check if lid is open
      state = LID_OPEN;
      pinMode(LED,OUTPUT);
      digitalWrite(LED,0); 
    }
    if(over_current_flag || over_temperature_flag){
      pinMode(LED,OUTPUT);
      digitalWrite(LED,0); 
      state = ERROR;
    }else{
      if(elapsed_time<fire_time){
        strip.setPixelColor(0, 0, 255, 0, 0);
        strip.show(); 
        sevseg.setNumber(int(fire_time-elapsed_time));
      }else{
        pinMode(LED,OUTPUT);
        digitalWrite(LED,0); 
        if(endless_loop){
          if(cool_down){
            if(cool_down_activated){
              t4 = millis();
              cool_down_activated = false;
            }else{
              int elapsed_cool_down_time = millis()-t4;
              if(elapsed_cool_down_time>10000){
                cool_down = false;
              }
              strip.setPixelColor(int(elapsed_cool_down_time/10000.0f*255), 0, 0, 0, 0);
              strip.show();
            }
            sevseg.setChars("COOL");
          }else{
            cool_down = true;
            cool_down_activated = true;
            armed_and_ready = true;
          }
        }else{
          state = ARMED;
        }
      }
      
    }
  }else if(state == ERROR){
      pinMode(LED,OUTPUT);
      digitalWrite(LED,0); 
      
      if(over_current_flag){
        over_current_pub.publish(&over_current);
        sevseg.setChars("curr");
      }
      if(over_temperature_flag){
        over_temperature_pub.publish(&over_temperature);
        sevseg.setChars("temp");
      }
      if(t1-t3>1000){
        t3 = t1;
      }else if(t1-t3>500){
        strip.setPixelColor(0, 0, 255, 0, 0);
        strip.show(); 
      }else{
        strip.setPixelColor(0, 255, 0, 0, 0);
        strip.show(); 
      }
  }else if(state == LID_OPEN){
    pinMode(LED,OUTPUT);
    digitalWrite(LED,0); 
    sevseg.setChars("lid");
    if(t1-t3>1000){
        t3 = t1;
    }else if(t1-t3>500){
      strip.setPixelColor(0, 0, 0, 255, 0);
      strip.show(); 
    }else{
      strip.setPixelColor(0, 0, 0, 0, 255);
      strip.show(); 
    }
  }


  
  sevseg.refreshDisplay(); // Must run repeatedly
}
