#include <Seeed_Arduino_FreeRTOS.h>
#include "MCP48FEB28.h"
#include"Free_Fonts.h"
#include "seeed_line_chart.h"
TFT_eSPI tft;

//**************************************************************************
// Type Defines and Constants
//**************************************************************************

#define  ERROR_LED_PIN  13 //Led Pin: Typical Arduino Board
#include "pinConfig.h"

#define ERROR_LED_LIGHTUP_STATE  LOW // the state that makes the led light up on your board, either low or high

//#define SERIAL          SerialUSB
#define SERIAL          Serial

//**************************************************************************
// global variables
//**************************************************************************
TaskHandle_t Handle_currentControlTask;
TaskHandle_t Handle_temperatureTask;
TaskHandle_t Handle_displayTask;
TaskHandle_t Handle_monitorTask;

enum TEMP{
  LED0,
  LED1,
  CHAMBER0,
  CHAMBER1,
  DRIVER_LED,
  DRIVER_TEC_LED,
  DRIVER_TEC_CHAMBER
}temperature;
float temp[7];

float temp_sp[3] = {20,20,20};

bool over_temp[7] = {false,false,false,false,false,false,false};

enum{
  CURRENT_LED0,
  CURRENT_LED1,
  CURRENT_LED_TEC_0,
  CURRENT_LED_TEC_1,
  CURRENT_CHAMBER_TEC_0,
  CURRENT_CHAMBER_TEC_1
}current;
int32_t current_raw[6];
bool emergency_off = false;
int32_t target_current[6] = {0,0,0,0,0,0};
int32_t gate_sp[6] = {0,0,0,0,0,0};

//**************************************************************************
// Can use these function for RTOS delays
// Takes into account procesor speed
//**************************************************************************
void myDelayUs(int us) {
    vTaskDelay(us / portTICK_PERIOD_US);
}

void myDelayMsUntil(TickType_t* previousWakeTime, int ms) {
    vTaskDelayUntil(previousWakeTime, (ms * 1000) / portTICK_PERIOD_US);
}

//*****************************************************************
//Current Control Thread
//Measures All Currents and controls the MOSFET gates 
//*****************************************************************
static void currentControlThread(void* pvParameters) {
    SERIAL.println("CurrentControlThread: Started");
    TickType_t xLastWakeTime;
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    pinMode(LED_ENABLE, INPUT_PULLUP);

    pinMode(LED_LATCH, OUTPUT);
    pinMode(TEC_LED_LATCH, OUTPUT);
    
    digitalWrite(LED_LATCH, true);
    digitalWrite(TEC_LED_LATCH, false);
    
    pinMode(LED_DIAG_ENABLE, OUTPUT);
    pinMode(TEC0_DIAG_ENABLE, OUTPUT);
    pinMode(TEC1_DIAG_ENABLE, OUTPUT);

    digitalWrite(LED_DIAG_ENABLE, false);
    digitalWrite(TEC0_DIAG_ENABLE, false);
    digitalWrite(TEC1_DIAG_ENABLE, false);

    pinMode(SEL0, OUTPUT);
    pinMode(SEL1, OUTPUT);
    digitalWrite(SEL0, false);
    digitalWrite(SEL1, false);

    int32_t target = 0, dac_val = 0;
    MCP48FEB28 *dac;
    dac = new MCP48FEB28(CS,LATCH);
    dac->init();

    int32_t iter = 0;
    
    while(1){
      // read LED currents
      digitalWrite(LED_DIAG_ENABLE,  true);
      digitalWrite(TEC0_DIAG_ENABLE, false);
      digitalWrite(TEC1_DIAG_ENABLE, false);
      digitalWrite(SEL1, false);
      myDelayUs(1000);
      current_raw[CURRENT_LED0] = analogRead(LED_SENS);
      digitalWrite(SEL1, true);
      myDelayUs(1000);
      current_raw[CURRENT_LED1] = analogRead(LED_SENS);
      
      // read LED TEC currents
      digitalWrite(LED_DIAG_ENABLE,  false);
      digitalWrite(TEC0_DIAG_ENABLE, true);
      digitalWrite(TEC1_DIAG_ENABLE, false);
      digitalWrite(SEL1, false);
      myDelayUs(1000);
      current_raw[CURRENT_LED_TEC_0] = analogRead(LED_SENS);
      digitalWrite(SEL1, true);
      myDelayUs(1000);
      current_raw[CURRENT_LED_TEC_1] = analogRead(LED_SENS);
      
      // read Chamber TEC currents
      digitalWrite(LED_DIAG_ENABLE,  false);
      digitalWrite(TEC0_DIAG_ENABLE, false);
      digitalWrite(TEC1_DIAG_ENABLE, true);
      digitalWrite(SEL1, false);
      myDelayUs(1000);
      current_raw[CURRENT_CHAMBER_TEC_0] = analogRead(LED_SENS);
      digitalWrite(SEL1, true);
      myDelayUs(1000);
      current_raw[CURRENT_CHAMBER_TEC_1] = analogRead(LED_SENS);

//      // EMERGENCY OFF 
//      for(int i=0;i<7;i++){
//        if(over_temp[i]){ 
//          emergency_off = true;
//          break;
//        }
//      }

      if(emergency_off){ // turn everything off
        // disable switches
        pinMode(LED_ENABLE, OUTPUT);
        digitalWrite(LED_ENABLE, false);
        // zero mosfet gates
        for(int i=0;i<6;i++){
          dac->write(i,0);
          gate_sp[i] = 0;
        }
      }else{
        if(!digitalRead(WIO_5S_UP)){
          target_current[0]+=5;
          target_current[1]+=5;
          if(target_current[0]>827){
            target_current[0] = 827;
          }
          if(target_current[1]>827){
            target_current[1] = 827;
          }
        }else if(!digitalRead(WIO_5S_DOWN)){
          if(target_current[0]>5){
            target_current[0]-=5;
          }else{
            target_current[0]=0;
          }
          if(target_current[1]>5){
            target_current[1]-=5;
          }else{
            target_current[1]=0;
          }
        }
        if(target_current[0]==0){
          gate_sp[0] = 0;
          digitalWrite(LED_LATCH, false);
        }else{
          digitalWrite(LED_LATCH, true);
        }

        if(target_current[1]==0){
          gate_sp[1] = 0;
        }

        if(target_current[0]>0 && gate_sp[0]==0){
          gate_sp[0] = 2500;
        }
        if(target_current[1]>0 && gate_sp[1]==0){
          gate_sp[1] = 2500;
        }

        if(current_raw[0]<target_current[0]){
          if(gate_sp[0]<4095){
            gate_sp[0]+=1;
          }
        }else{
          if(gate_sp[0]>0){
            gate_sp[0]-=1;  
          }
        }

        if(current_raw[1]<target_current[1]){
          if(gate_sp[1]<4095){
            gate_sp[1]+=1;
          }
        }else{
          if(gate_sp[1]>0){
            gate_sp[1]-=1;  
          }
        }

        for(int i=0;i<2;i++){
          dac->write(i,gate_sp[i]);
        }
      }

      myDelayMsUntil(&xLastWakeTime,10);
    }
}

//*****************************************************************
//Temperature Readout Thread
//Reads all temperatures
//*****************************************************************

float calcTemp(int val, float *p){
  return p[0]*val*val*val+p[1]*val*val+p[2]*val+p[3];
}

static void temperatureThread(void* pvParameters) {
  SERIAL.println("TemperatureThread: Started");

  TickType_t xLastWakeTime;
  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();

  float temp_poly[4] = {-1.06548079e-06, 1.17278707e-03, -5.32683331e-01, 1.31479023e+02};
  int32_t temp_raw[7];

  pinMode(TEC_CHAMBER_ENABLE, OUTPUT);
  pinMode(TEC_LED0_ENABLE, OUTPUT);
  pinMode(TEC_LED1_ENABLE, OUTPUT);
  digitalWrite(TEC_CHAMBER_ENABLE, false);
  digitalWrite(TEC_LED0_ENABLE, false);
  digitalWrite(TEC_LED1_ENABLE, false);
  
  while(1){
    // read all temperatures
    if(emergency_off){
      SERIAL.println("EMERGENCY OFF");
    }
    temp_raw[0] = analogRead(TEMP_LED0);
    temp_raw[1] = analogRead(TEMP_LED1);
    temp_raw[2] = analogRead(TEMP_CHAMBER0);
    temp_raw[3] = analogRead(TEMP_CHAMBER1);
    temp_raw[4] = analogRead(TEMP_DRIVER_LED);
    temp_raw[5] = analogRead(TEMP_DRIVER_TEC_LED);
    temp_raw[6] = analogRead(TEMP_DRIVER_TEC_CHAMBER);
    for(int j=0;j<7;j++){
      temp[j] = calcTemp(temp_raw[j],temp_poly);
      over_temp[j] = temp[j]>70;
      SERIAL.print(temp[j]);
      SERIAL.print("\t");
    }
    SERIAL.println();
    
    if(temp[0]>temp_sp[0]){
      digitalWrite(TEC_LED0_ENABLE, true);
    }else{
      digitalWrite(TEC_LED0_ENABLE, false);
    }

    if(temp[1]>temp_sp[1]){
      digitalWrite(TEC_LED1_ENABLE, true);
    }else{
      digitalWrite(TEC_LED1_ENABLE, false);
    }

    if(temp[2]>temp_sp[2]){
      digitalWrite(TEC_CHAMBER_ENABLE, true);
    }else{
      digitalWrite(TEC_CHAMBER_ENABLE, false);
    }
    
    myDelayMsUntil(&xLastWakeTime,1000);
  }

}

//*****************************************************************
// Display Thread
// renders all information on the display
//*****************************************************************
static void displayThread(void* pvParameters) {
    SERIAL.println("Display Thread: Started");

    tft.begin();
    tft.setRotation(3);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.fillScreen(TFT_WHITE);
    tft.setFreeFont(FM9);

    TickType_t xLastWakeTime;
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    char led_current_str0[20];
    char led_current_str1[20];
    char led_temp_str0[20];
    char led_temp_str1[20];
    char led_gate_str0[20];
    char led_gate_str1[20];
    char led_sp_str0[20];
    char led_sp_str1[20];
    char drv_temp_str[20];
    char chamber_temp_str[20];
    char chamber_temp_sp_str[20];

    tft.drawString("led0", 100, 0);
    tft.drawString("led1", 220, 0);
    tft.drawString("[mA]", 0, 30);
    tft.drawString("[C]", 0, 60);
    tft.drawString("[V]", 0, 90);
    tft.drawString("[mA]", 0, 120);
    tft.drawString("drv", 100, 150);
    tft.drawString("cham", 220, 150);
    tft.drawString("[C]", 0, 180);
    tft.drawString("[C]", 0, 210);

    int target_current_prev[2] = {0,0};
    int edit_mode = 0;

    while (1) {
//        if(SERIAL){
//          SERIAL.println("------------------------");
//          SERIAL.println("currents:");
//          SERIAL.print(current_raw[CURRENT_LED0]);
//          SERIAL.print("\t");
//          SERIAL.print(current_raw[CURRENT_LED1]);
//          SERIAL.print("\t");
//          SERIAL.print(current_raw[CURRENT_LED_TEC_0]);
//          SERIAL.print("\t");
//          SERIAL.print(current_raw[CURRENT_LED_TEC_1]);
//          SERIAL.print("\t");
//          SERIAL.print(current_raw[CURRENT_CHAMBER_TEC_0]);
//          SERIAL.print("\t");
//          SERIAL.print(current_raw[CURRENT_CHAMBER_TEC_1]);
//          SERIAL.println();
//          SERIAL.println("gates:");
//          for(int i=0;i<6;i++){
//            SERIAL.print(gate_sp[i]);
//            SERIAL.print("\t");
//          }
//          SERIAL.println();
//          SERIAL.println("target current:");
//          for(int i=0;i<6;i++){
//            SERIAL.print(target_current[i]);
//            SERIAL.print("\t");
//          }
//          SERIAL.println();
//        }

        tft.setTextColor(TFT_WHITE);
        tft.drawString(led_current_str0, 100, 30);
        tft.drawString(led_current_str1, 220, 30);
        tft.drawString(led_temp_str0, 100, 60);
        tft.drawString(led_temp_str1, 220, 60);
        tft.drawString(led_gate_str0, 100, 90);
        tft.drawString(led_gate_str1, 220, 90);
        tft.drawString(drv_temp_str, 100, 180);
        tft.drawString(chamber_temp_str, 220, 180);
        tft.drawString(chamber_temp_sp_str, 220, 210);
        
        tft.setTextColor(TFT_BLACK);
        // current
        sprintf(led_current_str0,"%d", (current_raw[CURRENT_LED0]>10?int(current_raw[CURRENT_LED0]*4.831):0));
        sprintf(led_current_str1,"%d", (current_raw[CURRENT_LED1]>10?int(current_raw[CURRENT_LED1]*4.831):0));
        tft.drawString(led_current_str0, 100, 30);
        tft.drawString(led_current_str1, 220, 30);
        // temp
        sprintf(led_temp_str0,"%.1f", temp[0]);
        sprintf(led_temp_str1,"%.1f", temp[1]);
        if(temp[0]<30){ tft.setTextColor(TFT_BLUE); }else if(temp[0]>30 && temp[0]<50){ tft.setTextColor(TFT_ORANGE); }else{ tft.setTextColor(TFT_RED);}
        tft.drawString(led_temp_str0, 100, 60);
        if(temp[1]<30){ tft.setTextColor(TFT_BLUE); }else if(temp[1]>30 && temp[1]<50){ tft.setTextColor(TFT_ORANGE); }else{ tft.setTextColor(TFT_RED);}
        tft.drawString(led_temp_str1, 220, 60);
        // gate
        tft.setTextColor(TFT_BLACK);
        sprintf(led_gate_str0,"%.1f", gate_sp[0]*3.3/4095);
        sprintf(led_gate_str1,"%.1f", gate_sp[1]*3.3/4095);
        tft.drawString(led_gate_str0, 100, 90);
        tft.drawString(led_gate_str1, 220, 90);
        // led setpoint
        if(target_current[0]!=target_current_prev[0] || target_current[1]!=target_current_prev[1]){
          tft.fillRect(100,120,230,30,TFT_WHITE);
          target_current_prev[0] = target_current[0];
          target_current_prev[1] = target_current[1];
        }
        sprintf(led_sp_str0,"%d", int(target_current[0]*4.838));
        sprintf(led_sp_str1,"%d", int(target_current[1]*4.838));
        tft.drawString(led_sp_str0, 100, 120);
        tft.drawString(led_sp_str1, 220, 120);
        // driver+chamber
        sprintf(drv_temp_str,"%.1f", temp[5]);
        sprintf(chamber_temp_str,"%.1f", temp[2]);
        if(temp[5]<30){ tft.setTextColor(TFT_BLUE); }else if(temp[5]>30 && temp[5]<50){ tft.setTextColor(TFT_ORANGE); }else{ tft.setTextColor(TFT_RED);}
        tft.drawString(drv_temp_str, 100, 180);
        if(temp[2]<30){ tft.setTextColor(TFT_BLUE); }else if(temp[2]>30 && temp[2]<50){ tft.setTextColor(TFT_ORANGE); }else{ tft.setTextColor(TFT_RED);}
        tft.drawString(chamber_temp_str, 220, 180);
        // chamber setpoint
        tft.setTextColor(TFT_BLACK);
        if(!digitalRead(WIO_KEY_C)){
          tft.fillRect(220,210,100,30,TFT_WHITE);
          temp_sp[2]--;
        }else if(!digitalRead(WIO_KEY_A)){
          tft.fillRect(220,210,100,30,TFT_WHITE);
          temp_sp[2]++;
        }
        sprintf(chamber_temp_sp_str,"%.1f", temp_sp[2]);
        tft.drawString(chamber_temp_sp_str, 220, 210);
        
        
        myDelayMsUntil(&xLastWakeTime,200);
    }

}

//*****************************************************************
// Task will periodicallt print out useful information about the tasks running
// Is a useful tool to help figure out stack sizes being used
//*****************************************************************
void taskMonitor(void* pvParameters) {
    int x;
    int measurement;

    SERIAL.println("Task Monitor: Started");

    while (1) {

        SERIAL.println("");
        SERIAL.println("******************************");
        SERIAL.println("[Stacks Free Bytes Remaining] ");

        measurement = uxTaskGetStackHighWaterMark(Handle_currentControlTask);
        SERIAL.print("CurrentControl: ");
        SERIAL.println(measurement);

        measurement = uxTaskGetStackHighWaterMark(Handle_temperatureTask);
        SERIAL.print("Temperature: ");
        SERIAL.println(measurement);

        measurement = uxTaskGetStackHighWaterMark(Handle_displayTask);
        SERIAL.print("Display: ");
        SERIAL.println(measurement);

        measurement = uxTaskGetStackHighWaterMark(Handle_monitorTask);
        SERIAL.print("Monitor Stack: ");
        SERIAL.println(measurement);

        SERIAL.println("******************************");

        delay(10000); // print every 10 seconds
    }
}


//*****************************************************************

void setup() {

    SERIAL.begin(2000000);

    vNopDelayMS(1000); // prevents usb driver crash on startup, do not omit this
//    while (!SERIAL) ;  // Wait for serial terminal to open port before starting program

    SERIAL.println("");
    SERIAL.println("******************************");
    SERIAL.println("        Program start         ");
    SERIAL.println("******************************");

    // Set the led the rtos will blink when we have a fatal rtos error
    // RTOS also Needs to know if high/low is the state that turns on the led.
    // Error Blink Codes:
    //    3 blinks - Fatal Rtos Error, something bad happened. Think really hard about what you just changed.
    //    2 blinks - Malloc Failed, Happens when you couldn't create a rtos object.
    //               Probably ran out of heap.
    //    1 blink  - Stack overflow, Task needs more bytes defined for its stack!
    //               Use the taskMonitor thread to help gauge how much more you need
    vSetErrorLed(ERROR_LED_PIN, ERROR_LED_LIGHTUP_STATE);

    // Create the threads that will be managed by the rtos
    // Sets the stack size and priority of each task
    // Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
    xTaskCreate(currentControlThread,     "Task Current Control",       512, NULL, tskIDLE_PRIORITY + 4, &Handle_currentControlTask);
    xTaskCreate(temperatureThread,     "Task Temperature",              512, NULL, tskIDLE_PRIORITY + 3, &Handle_temperatureTask);
    xTaskCreate(displayThread,     "Task Display",                      10000, NULL, tskIDLE_PRIORITY + 2, &Handle_displayTask);
    xTaskCreate(taskMonitor, "Task Monitor",                            256, NULL, tskIDLE_PRIORITY + 1, &Handle_monitorTask);

    pinMode(WIO_5S_UP, INPUT_PULLUP);
    pinMode(WIO_5S_DOWN, INPUT_PULLUP);
    pinMode(WIO_5S_PRESS, INPUT_PULLUP);
    pinMode(WIO_KEY_A, INPUT_PULLUP);
    pinMode(WIO_KEY_B, INPUT_PULLUP);
    pinMode(WIO_KEY_C, INPUT_PULLUP);

    // Start the RTOS, this function will never return and will schedule the tasks.
    vTaskStartScheduler();

}

//*****************************************************************
// loop never run
//*****************************************************************
void loop() {

}


//*****************************************************************
