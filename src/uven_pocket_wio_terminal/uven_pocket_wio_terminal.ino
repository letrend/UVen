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
#define MAX_SIZE 50 // maximum size of data
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
  volatile uint8_t data[BUFFER_SIZE];
};

SPI_FRAME cmd, res;

// Initialize the buffer
uint8_t buff [BUFFER_SIZE];

unsigned long t0, t1;

uint32_t sampling_time_ms = 0;

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
int faulty_frames = 0;

bool sendCommand(){
  

  cmd.values.crc = crc32.calc((uint8_t const *)&cmd.data[0], 20);

  for(int i=0;i<BUFFER_SIZE;i++){
    digitalWrite(CS, LOW);
    delay(5);
    buff[i] = SPI.transfer(cmd.data[i]);
    digitalWrite(CS, HIGH);
    delay(5);
//    Serial.print(cmd.data[i],HEX);Serial.print("\t");
  }
//  Serial.println();

  

  for(int i=0;i<BUFFER_SIZE;i++){
//    Serial.print(res.data[i], HEX);
//    Serial.print("\t");
    res.data[i] = buff[i];
  }

  return crc32.calc((uint8_t const *)&res.data[0], 20)==res.values.crc;
}

void keyAPressed(){
  static unsigned long t0 = millis();
  unsigned long t1 = millis();
  if((t1-t0)>100){
    menu--;
    t0 = t1;
  }
}

void keyCPressed(){
  static unsigned long t0 = millis();
  unsigned long t1 = millis();
  if((t1-t0)>100){
    menu++;
    t0 = t1;
  }
}

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
  cmd.values.intensity[0] = 10;
  cmd.values.intensity[1] = 10;
  cmd.values.time = 0;
  cmd.values.temperature[0] = 24;
  cmd.values.temperature[1] = 24;
  cmd.values.temperature[2] = 24;

  attachInterrupt(digitalPinToInterrupt(WIO_KEY_A), keyAPressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(WIO_KEY_C), keyCPressed, FALLING);
}

void loop() {
  

  if(sendCommand()){
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
//    if(digitalRead(WIO_KEY_A)==0){
//      menu--;
//    }
//    if(digitalRead(WIO_KEY_C)==0){
//      menu++;
//    }

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
        if(digitalRead(WIO_5S_UP)==0){
          char str[10];
          sprintf(str,"sampling time ms: %d",sampling_time_ms);
          tft.setTextColor(TFT_WHITE, TFT_WHITE);
          tft.drawCentreString(str, 160, 140, 2);
          tft.setTextColor(TFT_BLACK, TFT_WHITE);
          sampling_time_ms+=1000;
          sprintf(str,"sampling time ms: %d",sampling_time_ms);
          tft.drawCentreString(str, 140, 100, 2);
        }else if(digitalRead(WIO_5S_DOWN)==0){
          char str[10];
          sprintf(str,"sampling time ms: %d",sampling_time_ms);
          tft.setTextColor(TFT_WHITE, TFT_WHITE);
          tft.drawCentreString(str, 160, 140, 2);
          tft.setTextColor(TFT_BLACK, TFT_WHITE);
          sampling_time_ms-=1000;
          if(sampling_time_ms<0){
            sampling_time_ms = 0;
          }
          sprintf(str,"sampling time ms: %d",sampling_time_ms);
          tft.drawCentreString(str, 140, 100, 2);
        }
        if(t1-t0>sampling_time_ms){
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
          auto header0 = text(60, 0)
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
          auto header1 = text(60, spr0.height()+header0.height())
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
          auto header2 = text(60, spr0.height()*2+header0.height()*2)
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
          auto header0 = text(5,5)
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
            cmd.values.temperature[0]+=1;
          }else if(digitalRead(WIO_5S_DOWN)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.temperature[0]-=1;
          }
      
          if (updateTime <= millis()) {
              updateTime = millis() + LOOP_PERIOD;
              plotNeedle(temp[0].back(), 0);
          }
          break;
      }
     case SETPOINT_TEMP_1:{
      // Settings for the line graph title
          auto header0 = text(5,5)
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
            cmd.values.temperature[1]+=1;
          }else if(digitalRead(WIO_5S_DOWN)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.temperature[1]-=1;
          }
          
          if (updateTime <= millis()) {
              updateTime = millis() + LOOP_PERIOD;
              plotNeedle(temp[1].back(), 0);
          }
          break;
       }
       case SETPOINT_TEMP_2:{
        // Settings for the line graph title
          auto header0 = text(5,5)
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
            cmd.values.temperature[2]+=1;
          }else if(digitalRead(WIO_5S_DOWN)==0){
            tft.setTextColor(TFT_WHITE, TFT_WHITE);
            tft.drawCentreString(str, 160, 140, 2);
            tft.setTextColor(TFT_BLACK, TFT_WHITE);
            cmd.values.temperature[2]-=1;
          }
      
          if (updateTime <= millis()) {
              updateTime = millis() + LOOP_PERIOD;
              plotNeedle(temp[2].back(), 0);
          }
          break;
       }
       case SETPOINT_LED_0:{
        // Settings for the line graph title
          auto header0 = text(5,5)
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
              cmd.values.intensity[0]=0;
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
          auto header0 = text(5,5)
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
              cmd.values.intensity[1]=0;
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
    Serial.println("reconnecting");
    tft.fillScreen(TFT_RED); 
    tft.setTextColor(TFT_RED,TFT_WHITE);
    int j = 0;
    char str[30];
    while(!sendCommand() && j<100){
      tft.fillScreen(TFT_WHITE);
      digitalWrite(RESET_COMS,1);
      delay(200);
      digitalWrite(RESET_COMS,0);
      sprintf(str,"reconnecting %d",j++);
      tft.drawCentreString(str, 160, 140, 2);
      delay(2000);
    }
    tft.fillScreen(TFT_WHITE);
  }

//  delay(100); // Wait
}
