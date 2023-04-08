#include <SPI.h>
#include <Arduino_CRC32.h>
Arduino_CRC32 crc32;
#include "seeed_line_chart.h" //include the library

#include "analogMeter.h"
TFT_eSprite analogMeter_spr = TFT_eSprite(&tft); // Sprite
TFT_eSprite analogMeter_spr2 = TFT_eSprite(&tft);
TFT_eSprite spr0 = TFT_eSprite(&tft); // Sprite
TFT_eSprite spr1 = TFT_eSprite(&tft); // Sprite
TFT_eSprite spr2 = TFT_eSprite(&tft); // Sprite
#include"Free_Fonts.h" //include the header file

#define LOOP_PERIOD 35 // Display updates every 35 ms
#define MAX_SIZE 30 // maximum size of data
doubles temp[3];       // Initilising a doubles type to store data
doubles inten[2];

#define CS 1 
#define RESET_COMS 0

#define BUFFER_SIZE 24

union SPI_FRAME{
  struct{
    uint8_t control[2];
    uint8_t intensity[2];
    uint32_t time;
    float temperature[3];
    uint32_t crc;
  }values;
  uint8_t data[BUFFER_SIZE];
};

SPI_FRAME cmd, res;

// Initialize the buffer
uint8_t buff [BUFFER_SIZE];

unsigned long t0, t1;

float temp_min[3] = {1000,1000,1000};

enum {
  TEMPERATURE,
  SETPOINT_TEMP_0,
  SETPOINT_TEMP_1,
  SETPOINT_TEMP_2,
  SETPOINT_LED_0,
  SETPOINT_LED_1,
  CRC_MISMATCH
};

int8_t menu = TEMPERATURE, menu_prev = TEMPERATURE;

void setup() {
  pinMode(CS, OUTPUT);
  pinMode(RESET_COMS, OUTPUT);
  digitalWrite(CS, HIGH);
  digitalWrite(RESET_COMS, LOW);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV16);

  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  spr0.createSprite(TFT_HEIGHT-5, TFT_WIDTH/3-15);
  spr0.setRotation(3);
  spr1.createSprite(TFT_HEIGHT-5, TFT_WIDTH/3-15);
  spr1.setRotation(3);
  spr2.createSprite(TFT_HEIGHT-5, TFT_WIDTH/3-15);
  spr2.setRotation(3);
  t0 = millis();
  tft.fillScreen(TFT_WHITE); 

  analogMeter_spr.createSprite(TFT_HEIGHT,TFT_WIDTH/3-15);

  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);

  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  cmd.values.control[0] = 0;
  cmd.values.control[1] = 0;
  cmd.values.intensity[0] = 8;
  cmd.values.intensity[1] = 8;
  cmd.values.time = 0;
  cmd.values.temperature[0] = 24;
  cmd.values.temperature[1] = 24;
  cmd.values.temperature[2] = 24;
}

void loop() {
  digitalWrite(CS, LOW);
  delay(10);

  cmd.values.crc = crc32.calc((uint8_t const *)&cmd.data[0], 20);

  for(int i=0;i<BUFFER_SIZE;i++){
    buff[i] = SPI.transfer(cmd.data[i]);
    delay(10);
//    Serial.print(cmd.data[i],HEX);Serial.print("\t");
  }
//  Serial.println();

  digitalWrite(CS, HIGH);

  for(int i=0;i<BUFFER_SIZE;i++){
//    Serial.print(res.data[i], HEX);
//    Serial.print("\t");
    res.data[i] = buff[i];
  }

  if(crc32.calc((uint8_t const *)&res.data[0], 20)==res.values.crc){
//    Serial.println();
//    Serial.print("control_field: ");
//    Serial.print(res.values.control[0]);
//    Serial.print("\t");
//    Serial.println(res.values.control[1]);
//    Serial.print("intensity: ");
//    Serial.print(res.values.intensity[0]);
//    Serial.print("\t");
//    Serial.println(res.values.intensity[1]);
//    Serial.print("temperature: ");
//    Serial.print(res.values.temperature[0]);
//    Serial.print("\t");
//    Serial.print(res.values.temperature[1]);
//    Serial.print("\t");
//    Serial.println(res.values.temperature[2]);
    t1 = millis();
    if(digitalRead(WIO_KEY_A)==0){
      menu--;
    }
    if(digitalRead(WIO_KEY_C)==0){
      menu++;
    }

    if(menu>5){
      menu = TEMPERATURE;
    }
    if(menu<0){
      menu = SETPOINT_LED_1;
    }

    if(menu!=menu_prev){
      tft.fillScreen(TFT_WHITE); 
      if(menu>TEMPERATURE){
        analogMeter(); // Draw analogue meter
      }
      menu_prev = menu;
    }

    if (temp[0].size() > MAX_SIZE)
    {
        temp[0].pop(); 
        temp[1].pop();
        temp[2].pop();
        inten[0].pop();
        inten[1].pop();
    }
    temp[0].push(res.values.temperature[0]);
    temp[1].push(res.values.temperature[1]);
    temp[2].push(res.values.temperature[2]);
    inten[0].push(cmd.values.intensity[0]);
    inten[1].push(cmd.values.intensity[1]);
    
    switch(menu){
      case TEMPERATURE:
        if(t1-t0>1000){
          t0 = t1;
          spr0.fillSprite(TFT_WHITE);
          spr1.fillSprite(TFT_WHITE);
          spr2.fillSprite(TFT_WHITE);
    
          if(res.values.temperature[0]<temp_min[0]){
            temp_min[0] = res.values.temperature[0];
          }
          if(res.values.temperature[1]<temp_min[1]){
            temp_min[1] = res.values.temperature[1];
          }
          if(res.values.temperature[2]<temp_min[2]){
            temp_min[2] = res.values.temperature[2];
          }
    
          // Settings for the line graph title
          auto header0 = text(0, 0)
                            .value("LED0")
                            .align(left)
                            .valign(vcenter)
                            .width(tft.width())
                            .thickness(2);
      
          header0.height(header0.font_height(&tft));
          header0.draw(&tft); // Header height is the twice the height of the font
          
          // Settings for the line graph temp0
          auto content0 = line_chart(0, 0); //(x,y) where the line graph begins
          content0
              .height(spr0.height()) // actual height of the line chart
              .width(spr0.width())         // actual width of the line chart
              .based_on(temp_min[0])                                // Starting point of y-axis, must be a float
              .show_circle(true)                           // drawing a cirle at each point, default is on.
              .value(temp[0])                                  // passing through the data to line graph
              .max_size(MAX_SIZE)
              .color(TFT_RED)                               // Setting the color for the line
              .backgroud(TFT_WHITE)
              .draw(&spr0);
    
          // Settings for the line graph title
          auto header1 = text(0, spr0.height()+header0.height())
                            .value("LED1")
                            .align(left)
                            .valign(vcenter)
                            .width(tft.width())
                            .thickness(2);
      
          header1.height(header1.font_height(&tft));
          header1.draw(&tft); // Header height is the twice the height of the font
      
          // Settings for the line graph temp0
          auto content1 = line_chart(0, 0); //(x,y) where the line graph begins
          content1
              .height(spr1.height()) // actual height of the line chart
              .width(spr1.width())         // actual width of the line chart
              .based_on(temp_min[1])                                // Starting point of y-axis, must be a float
              .show_circle(true)                           // drawing a cirle at each point, default is on.
              .value(temp[1])                                  // passing through the data to line graph
              .max_size(MAX_SIZE)
              .color(TFT_RED)                               // Setting the color for the line
              .backgroud(TFT_WHITE)
              .draw(&spr1);
    
          // Settings for the line graph title
          auto header2 = text(0, spr0.height()*2+header0.height()*2)
                            .value("CHAMBER")
                            .align(left)
                            .valign(vcenter)
                            .width(tft.width())
                            .thickness(2);
      
          header2.height(header2.font_height(&tft));
          header2.draw(&tft); // Header height is the twice the height of the font
              
          // Settings for the line graph temp0
          auto content2 = line_chart(0, 0); //(x,y) where the line graph begins
          content2
              .height(spr2.height()) // actual height of the line chart
              .width(spr2.width())         // actual width of the line chart
              .based_on(temp_min[2])                                // Starting point of y-axis, must be a float
              .show_circle(true)                           // drawing a cirle at each point, default is on.
              .value(temp[2])                                  // passing through the data to line graph
              .max_size(MAX_SIZE)
              .color(TFT_RED)                               // Setting the color for the line
              .backgroud(TFT_WHITE)
              .draw(&spr2);
      
          spr0.pushSprite(5, header0.height());
          spr1.pushSprite(5, spr0.height()+header0.height()*2);
          spr2.pushSprite(5, spr0.height()*2+header0.height()*3);
        }
        break;
      case SETPOINT_TEMP_0:{
        // Settings for the line graph title
          auto header0 = text(0, 0)
                            .value("LED0 TEMPERATURE")
                            .align(left)
                            .valign(vcenter)
                            .width(tft.width())
                            .thickness(2);
      
          header0.height(header0.font_height(&tft));
          header0.draw(&tft); // Header height is the twice the height of the font
          
         analogMeter_spr.fillSprite(TFT_WHITE);
         auto content = line_chart(8, 0); //(x,y) where the line graph begins
               content
                      .height(tft.height() ) //actual height of the line chart
                      .width(tft.width()) //actual width of the line chart
                      .based_on(0.0) //Starting point of y-axis, must be a float
                      .show_circle(true) //drawing a cirle at each point, default is on.
                      .value(temp[0]) //passing through the data to line graph
                      .color(TFT_PURPLE) //Setting the color for the line
                      .draw(&analogMeter_spr2);
                       
          analogMeter_spr.pushSprite(0, 190);

          char str[10];
          sprintf(str,"setpoint: %.1f",cmd.values.temperature[0]);
          tft.drawCentreString(str, 160, 140, 2);
          if(digitalRead(WIO_5S_UP)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.temperature[0]+=0.5;
          }else if(digitalRead(WIO_5S_DOWN)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.temperature[0]-=0.5;
          }
      
          if (updateTime <= millis()) {
              updateTime = millis() + LOOP_PERIOD;
              plotNeedle(temp[0].back(), 0);
          }
          break;
      }
     case SETPOINT_TEMP_1:{
      // Settings for the line graph title
          auto header0 = text(0, 0)
                            .value("LED1 TEMPERATURE")
                            .align(left)
                            .valign(vcenter)
                            .width(tft.width())
                            .thickness(2);
      
          header0.height(header0.font_height(&tft));
          header0.draw(&tft); // Header height is the twice the height of the font
          analogMeter_spr.fillSprite(TFT_WHITE);
          auto content = line_chart(8, 0); //(x,y) where the line graph begins
               content
                      .height(tft.height() ) //actual height of the line chart
                      .width(tft.width()) //actual width of the line chart
                      .based_on(0.0) //Starting point of y-axis, must be a float
                      .show_circle(true) //drawing a cirle at each point, default is on.
                      .value(temp[1]) //passing through the data to line graph
                      .color(TFT_PURPLE) //Setting the color for the line
                      .draw(&analogMeter_spr2);
                       
          analogMeter_spr.pushSprite(0, 190);

          char str[10];
          sprintf(str,"setpoint: %.1f",cmd.values.temperature[1]);
          tft.drawCentreString(str, 160, 140, 2);
          if(digitalRead(WIO_5S_UP)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.temperature[1]+=0.5;
          }else if(digitalRead(WIO_5S_DOWN)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.temperature[1]-=0.5;
          }
          
          if (updateTime <= millis()) {
              updateTime = millis() + LOOP_PERIOD;
              plotNeedle(temp[1].back(), 0);
          }
          break;
       }
       case SETPOINT_TEMP_2:{
        // Settings for the line graph title
          auto header0 = text(0, 0)
                            .value("CHAMBER TEMPERATURE")
                            .align(left)
                            .valign(vcenter)
                            .width(tft.width())
                            .thickness(2);
      
          header0.height(header0.font_height(&tft));
          header0.draw(&tft); // Header height is the twice the height of the font
        
         analogMeter_spr.fillSprite(TFT_WHITE);
         auto content = line_chart(8, 0); //(x,y) where the line graph begins
               content
                      .height(tft.height() ) //actual height of the line chart
                      .width(tft.width()) //actual width of the line chart
                      .based_on(0.0) //Starting point of y-axis, must be a float
                      .show_circle(true) //drawing a cirle at each point, default is on.
                      .value(temp[2]) //passing through the data to line graph
                      .color(TFT_PURPLE) //Setting the color for the line
                      .draw(&analogMeter_spr2);
                       
          analogMeter_spr.pushSprite(0, 190);

          char str[10];
          sprintf(str,"setpoint: %.1f",cmd.values.temperature[2]);
          tft.drawCentreString(str, 160, 140, 2);
          if(digitalRead(WIO_5S_UP)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.temperature[2]+=0.5;
          }else if(digitalRead(WIO_5S_DOWN)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.temperature[2]-=0.5;
          }
      
          if (updateTime <= millis()) {
              updateTime = millis() + LOOP_PERIOD;
              plotNeedle(temp[2].back(), 0);
          }
          break;
       }
       case SETPOINT_LED_0:{
        // Settings for the line graph title
          auto header0 = text(0, 0)
                            .value("LED0 INTENSITY")
                            .align(left)
                            .valign(vcenter)
                            .width(tft.width())
                            .thickness(2);
      
          header0.height(header0.font_height(&tft));
          header0.draw(&tft); // Header height is the twice the height of the font
          
         analogMeter_spr.fillSprite(TFT_WHITE);
         auto content = line_chart(8, 0); //(x,y) where the line graph begins
               content
                      .height(tft.height() ) //actual height of the line chart
                      .width(tft.width()) //actual width of the line chart
                      .based_on(0.0) //Starting point of y-axis, must be a float
                      .show_circle(true) //drawing a cirle at each point, default is on.
                      .value(inten[0]) //passing through the data to line graph
                      .color(TFT_PURPLE) //Setting the color for the line
                      .draw(&analogMeter_spr2);
                       
          analogMeter_spr.pushSprite(0, 190);

          char str[10];
          sprintf(str,"setpoint: %d",cmd.values.intensity[0]);
          tft.drawCentreString(str, 160, 140, 2);
          if(digitalRead(WIO_5S_UP)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.intensity[0]+=5;
            if(cmd.values.intensity[0]>=100){
              cmd.values.intensity[0]=100;
            }
          }else if(digitalRead(WIO_5S_DOWN)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.intensity[0]-=5;
            if(cmd.values.intensity[0]<5){
              cmd.values.intensity[0]=5;
            }
          }

          if (updateTime <= millis()) {
              updateTime = millis() + LOOP_PERIOD;
              plotNeedle(inten[0].back(), 0);
          }
     
          break;
      }
     case SETPOINT_LED_1:{
        // Settings for the line graph title
          auto header0 = text(0, 0)
                            .value("LED1 INTENSITY")
                            .align(left)
                            .valign(vcenter)
                            .width(tft.width())
                            .thickness(2);
      
          header0.height(header0.font_height(&tft));
          header0.draw(&tft); // Header height is the twice the height of the font
          
         analogMeter_spr.fillSprite(TFT_WHITE);
         auto content = line_chart(8, 0); //(x,y) where the line graph begins
               content
                      .height(tft.height() ) //actual height of the line chart
                      .width(tft.width()) //actual width of the line chart
                      .based_on(0.0) //Starting point of y-axis, must be a float
                      .show_circle(true) //drawing a cirle at each point, default is on.
                      .value(inten[1]) //passing through the data to line graph
                      .color(TFT_PURPLE) //Setting the color for the line
                      .draw(&analogMeter_spr2);
                       
          analogMeter_spr.pushSprite(0, 190);

          char str[10];
          sprintf(str,"setpoint: %d",cmd.values.intensity[1]);
          tft.drawCentreString(str, 160, 140, 2);
          if(digitalRead(WIO_5S_UP)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.intensity[1]+=5;
            if(cmd.values.intensity[1]>=100){
              cmd.values.intensity[1]=100;
            }
          }else if(digitalRead(WIO_5S_DOWN)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.intensity[1]-=5;
            if(cmd.values.intensity[1]<5){
              cmd.values.intensity[1]=5;
            }
          }
          if (updateTime <= millis()) {
              updateTime = millis() + LOOP_PERIOD;
              plotNeedle(inten[1].back(), 0);
          }
     
          break;
      }
    }
    
    
  }else{
    Serial.println("crc mismatch");
    tft.fillScreen(TFT_WHITE); 
    tft.setTextColor(TFT_RED,TFT_WHITE);
    tft.setFreeFont(FF20); //select Free, Mono, Oblique, 12pt.
    tft.drawString("crc mismatch",20,100);//prints string at (70,80)
    digitalWrite(RESET_COMS,1);
    delay(100);
    digitalWrite(RESET_COMS,0);
  }

//  delay(100); // Wait
}
