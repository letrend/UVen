#include "pinConfig.h"
#include "MCP48FEB28.h"
MCP48FEB28 *dac_0;
MCP48FEB28 *dac_1;
void setup() {
  dac_0 = new MCP48FEB28(CS0,LATCH0);
  dac_1 = new MCP48FEB28(CS1,LATCH1);
  dac_0->init();
  dac_1->init();
  pinMode(LED_FAN,OUTPUT);
  pinMode(CHAMBER_FAN,OUTPUT);
  analogWrite(LED_FAN,100);
  analogWrite(CHAMBER_FAN,200);

  pinMode(LED_SEL0,OUTPUT);
  pinMode(LED_SEL1,OUTPUT);

  pinMode(LED_ENABLE, INPUT_PULLUP);

  pinMode(LED_LATCH, OUTPUT);  
  digitalWrite(LED_LATCH, true);
  
  pinMode(LED_DIAG_ENABLE_A, OUTPUT);
  pinMode(LED_DIAG_ENABLE_B, OUTPUT);
  pinMode(LED_DIAG_ENABLE_C, OUTPUT);
  pinMode(LED_DIAG_ENABLE_D, OUTPUT);
  pinMode(LED_DIAG_ENABLE_E, OUTPUT);
  pinMode(LED_DIAG_ENABLE_F, OUTPUT);

  digitalWrite(LED_DIAG_ENABLE_A, false);
  digitalWrite(LED_DIAG_ENABLE_B, false);
  digitalWrite(LED_DIAG_ENABLE_C, false);
  digitalWrite(LED_DIAG_ENABLE_D, false);
  digitalWrite(LED_DIAG_ENABLE_E, false);
  digitalWrite(LED_DIAG_ENABLE_F, false);

  pinMode(LED_SEL0, OUTPUT);
  pinMode(LED_SEL1, OUTPUT);
  digitalWrite(LED_SEL0, false);
  digitalWrite(LED_SEL1, false);

  Serial.begin(115200);
}

bool emergency_off = false;
bool over_temp[7] = {false,false,false,false,false,false,false};
int32_t current_raw[16];
int32_t target_current[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int32_t gate_sp[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t temp_pins[17] = { TEMP_LED0,TEMP_LED1,TEMP_LED2,TEMP_LED3,
                          TEMP_LED4,TEMP_LED5,TEMP_LED6,TEMP_LED7,
                          TEMP_LED8,TEMP_LED9,TEMP_LED10,TEMP_LED11,
                          TEMP_LED12,TEMP_LED13,TEMP_LED14,TEMP_LED15,
                          TEMP_DRIVER };
const float temp_poly[4] = {-1.06548079e-06, 1.17278707e-03, -5.32683331e-01, 1.31479023e+02};
int32_t temp_raw[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float temp[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t iteration = 0;

float calcTemp(int val, const float *p){
  return p[0]*val*val*val+p[1]*val*val+p[2]*val+p[3];
}

void loop() {
  // t+=0.01;
  // int val = (sin(t)+1)/2.0*3800;
  // for(int i=0;i<8;i++){
  //   mcp48_0->write(i, val);
  //   mcp48_1->write(i, val);
  // }

  // read LED currents
  digitalWrite(LED_DIAG_ENABLE_A,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[0] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[1] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_A,  false);

  digitalWrite(LED_DIAG_ENABLE_B,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[2] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[3] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_B,  false);

  digitalWrite(LED_DIAG_ENABLE_C,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[4] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[5] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_C,  false);

  digitalWrite(LED_DIAG_ENABLE_D,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[6] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[7] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_D,  false);

  digitalWrite(LED_DIAG_ENABLE_E,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[8] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[9] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_E,  false);

  digitalWrite(LED_DIAG_ENABLE_F,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[10] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[11] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_F,  false);

  digitalWrite(LED_DIAG_ENABLE_G,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[12] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[13] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_G,  false);

  digitalWrite(LED_DIAG_ENABLE_H,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[14] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[15] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_H,  false);

  for(int i=0;i<17;i++){
    temp_raw[i] = analogRead(temp_pins[i]);
    temp[i] = calcTemp(temp_raw[i],temp_poly);
  }

  if(target_current[0]==0){
    gate_sp[0] = 0;
    digitalWrite(LED_LATCH, false);
  }else{
    digitalWrite(LED_LATCH, true);
  }

  if(target_current[0]>0 && gate_sp[0]==0){
    gate_sp[0] = 2500;
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

  for(int i=0;i<8;i++){
    dac_0->write(i,gate_sp[i]);
    dac_1->write(i,gate_sp[i+8]);
  }

  if(Serial.available()){
    Serial.println("new setpoint");
    target_current[0] = Serial.parseInt();
  }

  if((iteration++%50)==0){
    Serial.println("LED currents:");
    for(int i=0;i<16;i++){
      Serial.printf("%d\t",current_raw[i]);
    }
    Serial.println();
    Serial.println("gate setpoints:");
    for(int i=0;i<16;i++){
      Serial.printf("%d\t",gate_sp[i]);
    }
    Serial.println();
    Serial.println("target currents:");
    for(int i=0;i<16;i++){
      Serial.printf("%d\t",target_current[i]);
    }
    Serial.println();
    // Serial.println("LED temps:");
    // for(int i=0;i<16;i++){
    //   Serial.printf("%.1f\t",temp[i]);
    // }
    // Serial.println();
    // Serial.printf("DRIVER temp: %.1f",temp[16]);
    // Serial.println();
  }
  
}
