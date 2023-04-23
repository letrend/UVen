#include <SPI.h>
#include <stdint.h>
#include <Arduino_CRC32.h>
Arduino_CRC32 crc32;
#include "Wire.h"
//Defines so the device can do a self reset
#define SYSRESETREQ    (1<<2)
#define VECTKEY        (0x05fa0000UL)
#define VECTKEY_MASK   (0x0000ffffUL)
#define AIRCR          (*(uint32_t*)0xe000ed0cUL) // fixed arch-defined address
#define REQUEST_EXTERNAL_RESET (AIRCR=(AIRCR&VECTKEY_MASK)|VECTKEY|SYSRESETREQ)


#define BUTTON_0 41
#define BUTTON_1 40
#define BUTTON_2 39

#define LED_0_DISABLE 14
#define LED_1_DISABLE 15

#define TEC_LED0 42
#define TEC_LED1 43
#define TEC_LED0_IN 4
#define TEC_LED0_STATUS 7
#define TEC_LED1_IN 3
#define TEC_LED1_STATUS 6

#define KILL 28
#define RESET_COMS 35

bool led_fire = false;
 
#define pot_address 0x2F // each I2C object has a unique bus address, the MCP4018 is 0x2F or 0101111 in binary

unsigned long t0,t1,t2,fire_start_time,last_pressed,tec_led0_t0,tec_led1_t0;
float poly[3][3] = { {8.57991735e-05, -2.05112717e-01,  1.05402649e+02},
                     {1.29957085e-04, -2.37834294e-01,  1.09390237e+02},
                     {1.30629020e-04, -2.35951069e-01,  1.08283965e+02} };
int analogValue[3] = {0};
int analogPin[3] = {A0,A1,A2};

float calcTemp(int val, float *p){
  return p[0]*val*val+p[1]*val+p[2];
}

bool ran_once = false;
uint32_t time_setpoint = 0;

// SPI 0 interrupt for the SAM3XA chip:
#define SPI0_INTERRUPT_NUMBER (IRQn_Type)24

// Buffer sized as needed
#define BUFFER_SIZE 24

// Default chip select pin, not tested with any other pins
#define CS 10

union SPI_FRAME{
  struct{
    uint8_t control[2];
    uint8_t intensity[2];
    uint32_t time;
    float temperature[3];
    uint32_t crc;
  }values;
  byte data[BUFFER_SIZE];
};

static volatile SPI_FRAME cmd, tmp_cmd, res, tmp_res;

// Initialize the buffer
uint8_t buff [BUFFER_SIZE];

// Needs to be volatile to update properly within the interrupt
volatile uint32_t pos;

// Make sure the handler is properly defined for the compiler to catch it.
void SPI0_Handler( void );

void slaveBegin(uint8_t _pin) {
  // Setup the SPI Interrupt registers.
  NVIC_ClearPendingIRQ(SPI0_INTERRUPT_NUMBER);
  NVIC_EnableIRQ(SPI0_INTERRUPT_NUMBER);

  // Initialize the SPI device with Arduino default values
  SPI.begin(_pin);
  REG_SPI0_CR = SPI_CR_SWRST;     // reset SPI
  
  // Setup interrupt
  REG_SPI0_IDR = SPI_IDR_TDRE | SPI_IDR_MODF | SPI_IDR_OVRES | SPI_IDR_NSSR | SPI_IDR_TXEMPTY | SPI_IDR_UNDES;
  REG_SPI0_IER = SPI_IER_RDRF;
  
  // Setup the SPI registers.
  REG_SPI0_CR = SPI_CR_SPIEN;     // enable SPI
  REG_SPI0_MR = SPI_MR_MODFDIS;     // slave and no modefault
  REG_SPI0_CSR = SPI_MODE0;    // DLYBCT=0, DLYBS=0, SCBR=0, 8 bit transfer
  pos = 0;
  REG_SPI0_TDR = res.data[pos];
  res.values.crc = crc32.calc((uint8_t const *)&res.data[0], 20);
}


void SPI0_Handler( void )
{
    uint32_t d = 0;
    // Receive byte
    d = REG_SPI0_RDR;
    
    // save to buffer
    buff[pos] = d & 0xFF;
    pos++;
    REG_SPI0_TDR = res.data[pos];
}

void resetComs(){
//  REQUEST_EXTERNAL_RESET;
  pos = 0;
  REG_SPI0_TDR = res.data[0];
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_0_DISABLE,OUTPUT);
  pinMode(LED_1_DISABLE,OUTPUT);
  
  digitalWrite(LED_0_DISABLE,true);
  digitalWrite(LED_1_DISABLE,true);

  Serial.println("setting up");

  cmd.values.control[0] = 0;
  cmd.values.control[1] = 0;
  cmd.values.time = 0;
  cmd.values.temperature[0] = 24;
  cmd.values.temperature[1] = 24;
  cmd.values.temperature[2] = 24;

  Wire.begin();
  Wire1.begin();

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
  pinMode(CS,INPUT);
  pinMode(RESET_COMS,INPUT);
  attachInterrupt(digitalPinToInterrupt(RESET_COMS), resetComs, RISING);
  // Setup the SPI as Slave
  slaveBegin(CS);
  t1 = millis();
}

void loop() {
  t0 = millis();
  // If transfer is complete send the data.
  if ( pos == BUFFER_SIZE )
  {
    for(int i=0;i<BUFFER_SIZE;i++){
//      Serial.print(buff[i],HEX);
//      Serial.print("\t");
      tmp_cmd.data[i] = buff[i];
    }
//    Serial.println("\n");

    if(crc32.calc((uint8_t const *)&tmp_cmd.data[0], 20)==tmp_cmd.values.crc){
      for(int i=0;i<BUFFER_SIZE;i++){
        cmd.data[i] = tmp_cmd.data[i];
      }
//      Serial.println();
//      Serial.print("control_field: ");
//      Serial.print(cmd.values.control[0]);
//      Serial.print("\t");
//      Serial.println(cmd.values.control[1]);
//      Serial.print("intensity: ");
//      Serial.print(cmd.values.intensity[0]);
//      Serial.print("\t");
//      Serial.println(cmd.values.intensity[1]);
//      Serial.print("temperature: ");
//      Serial.print(cmd.values.temperature[0]);
//      Serial.print("\t");
//      Serial.print(cmd.values.temperature[1]);
//      Serial.print("\t");
//      Serial.println(cmd.values.temperature[2]);
    }else{
      Serial.println("crc mismatch");
    }

    Wire.beginTransmission(pot_address);
    Wire.write(cmd.values.intensity[0]); // 
    Wire.endTransmission();
  
    Wire1.beginTransmission(pot_address);
    Wire1.write(cmd.values.intensity[1]); // 
    Wire1.endTransmission();
    
    tmp_res.values.control[0] = cmd.values.control[0];
    tmp_res.values.control[1] = 
      ran_once<<4|
      (tmp_res.values.temperature[2]>cmd.values.temperature[2])<<3|
      (tmp_res.values.temperature[1]>cmd.values.temperature[1])<<2|
      (tmp_res.values.temperature[0]>cmd.values.temperature[0])<<1|
      led_fire;
    tmp_res.values.time = (led_fire?(millis()-fire_start_time):0);
    tmp_res.values.intensity[0] = cmd.values.intensity[0];
    tmp_res.values.intensity[1] = cmd.values.intensity[1];
    tmp_res.values.crc = crc32.calc((uint8_t const *)&tmp_res.data[0], 20);
    for(int i=0;i<BUFFER_SIZE;i++){
      res.data[i] = tmp_res.data[i];
    }
    
    pos = 0;
    REG_SPI0_TDR = res.data[0];
  }

  if(t0-t1>100){
    t1 = t0;
    for(int i=0;i<3;i++){
      analogValue[i] = analogRead(analogPin[i]);  
      tmp_res.values.temperature[i] = calcTemp(analogValue[i],poly[i]);
    }
  
    if(tmp_res.values.temperature[0]>cmd.values.temperature[0]){
      tec_led0_t0 = millis();
      digitalWrite(TEC_LED0,true);
    }else{
      if((millis()-tec_led0_t0)>5000){
        digitalWrite(TEC_LED0,false);
      }
    }
    if(tmp_res.values.temperature[1]>cmd.values.temperature[1]){
      tec_led1_t0 = millis();
      digitalWrite(TEC_LED1,true);
    }else{
      if((millis()-tec_led1_t0)>5000){
        digitalWrite(TEC_LED1,false);
      }
    }
    if(tmp_res.values.temperature[2]>cmd.values.temperature[2]){
      digitalWrite(TEC_LED0_IN,true);
      digitalWrite(TEC_LED1_IN,true);
    }else{
      digitalWrite(TEC_LED0_IN,false);
      digitalWrite(TEC_LED1_IN,false);
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
    }else{
      digitalWrite(LED_0_DISABLE,true);
      digitalWrite(LED_1_DISABLE,true);
    }
  }
}
