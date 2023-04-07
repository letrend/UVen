#include <SPI.h>

const byte CS = 1; // Chip Select pin

#define BUFFER_SIZE 16

union SPI_FRAME{
  struct{
    uint8_t control[2];
    uint8_t intensity[2];
    float temperature[3];
  }values;
  uint8_t data[BUFFER_SIZE];
};

SPI_FRAME cmd, res;

// Initialize the buffer
uint8_t buff [BUFFER_SIZE];

void setup() {
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV4);

  Serial.begin(115200);
}

void loop() {
  digitalWrite(CS, LOW);

  cmd.values.control[0] = cmd.values.control[0]+1;
  cmd.values.control[1] = cmd.values.control[1]+1;
  cmd.values.intensity[0] = random(0, 100);
  cmd.values.intensity[1] = random(0, 100);
  cmd.values.temperature[0] = random(20, 30); 
  cmd.values.temperature[1] = random(20, 30); 
  cmd.values.temperature[2] = random(20, 30); 

  for(int i=0;i<BUFFER_SIZE;i++){
    buff[i] = SPI.transfer(cmd.data[i]);
    delay(1);
//    Serial.print(cmd.data[i],HEX);Serial.print("\t");
  }
  Serial.println();

  digitalWrite(CS, HIGH);

  for(int i=0;i<BUFFER_SIZE;i++){
    Serial.print(res.data[i], HEX);
    Serial.print("\t");
    res.data[i] = buff[i];
  }
  Serial.println();
  Serial.print("control_field: ");
  Serial.print(res.values.control[0]);
  Serial.print("\t");
  Serial.println(res.values.control[1]);
  Serial.print("intensity: ");
  Serial.print(res.values.intensity[0]);
  Serial.print("\t");
  Serial.println(res.values.intensity[1]);
  Serial.print("temperature: ");
  Serial.print(res.values.temperature[0]);
  Serial.print("\t");
  Serial.print(res.values.temperature[1]);
  Serial.print("\t");
  Serial.println(res.values.temperature[2]);

  delay(1000); // Wait
}
