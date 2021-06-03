#include <ros.h>
#include <std_msgs/UInt8.h>

#include "Wire.h"
#define pot_address 0x2F // each I2C object has a unique bus address, the MCP4018 is 0x2F or 0101111 in binary

ros::NodeHandle nh;

void poti_cb(const std_msgs::UInt8& msg){
    Wire.beginTransmission(pot_address);
    Wire.write(msg.data); // 
    Wire.endTransmission();
}

ros::Subscriber<std_msgs::UInt8> poti_sub("poti", &poti_cb );

void setup()
{
  Wire.begin();
  nh.initNode();
  nh.subscribe(poti_sub);
}

void loop()
{
  nh.spinOnce();
}