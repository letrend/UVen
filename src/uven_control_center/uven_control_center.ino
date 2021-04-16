#include <Tli4970.h>
#include "SevSeg.h"
#define USE_USBCON
#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Empty.h>

SevSeg sevseg; 
Tli4970 current_sensor[5] = {Tli4970(),Tli4970(),Tli4970(),Tli4970(),Tli4970()};

#define BUTTON0 40
#define BUTTON1 41
#define BUTTON2 39

#define LED 2

unsigned long t0,t1;

void buttonChange(){
  
}

ros::NodeHandle nh;
std_msgs::Float32 temp[5], current[5];
std_msgs::Int32 over_current, over_temperature;
std_msgs::Empty toggle_msg;
ros::Publisher over_current_pub("over_current", &over_current);
ros::Publisher over_temperature_pub("over_temperature", &over_temperature);
ros::Publisher temp_pub[5]= { ros::Publisher("temp0", &temp[0]),ros::Publisher("temp1", &temp[1]),
                              ros::Publisher("temp2", &temp[2]),ros::Publisher("temp3", &temp[3]),
                              ros::Publisher("temp4", &temp[4])};
ros::Publisher current_pub[5]= { ros::Publisher("current0", &current[0]),ros::Publisher("current1", &current[1]),
                              ros::Publisher("current2", &current[2]),ros::Publisher("current3", &current[3]),
                              ros::Publisher("current4", &current[4])};
//ros::Subscriber<std_msgs::Empty> more_torque_sub("more_torque", &moreTorque );

void setup() {
  pinMode(BUTTON0,INPUT_PULLUP);
  pinMode(BUTTON1,INPUT_PULLUP);
  pinMode(BUTTON2,INPUT_PULLUP);
  
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
  digitalWrite(2,0);
  
  t0 = millis();
  attachInterrupt(digitalPinToInterrupt(BUTTON0), buttonChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON1), buttonChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON2), buttonChange, CHANGE);

  nh.initNode();
  nh.advertise(over_current_pub);
  nh.advertise(over_temperature_pub);
  for(int i=0;i<5;i++){
    nh.advertise(temp_pub[i]);  
    nh.advertise(current_pub[i]);  
  }
//  nh.subscribe(more_torque_sub);
}

float poly[4] = { -9.11401328e-07,   1.12277904e-03,  -5.54068598e-01,   1.35816421e+02};
int analogValue[5] = {0}, analogPin[5] = {A0,A1,A2,A3,A4};

float calcTemp(int val){
  return poly[0]*val*val*val+poly[1]*val*val+poly[2]*val+poly[3];
}

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

    sevseg.setNumber(int(temp[0].data ));

    t0 = t1;
    nh.spinOnce();
  }
  
  sevseg.refreshDisplay(); // Must run repeatedly
}
