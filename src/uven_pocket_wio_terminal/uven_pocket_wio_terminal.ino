#include <SPI.h>

const byte masterSSPin = 1; // Slave Select pin

byte controlField = 0x01;
float tempSetpoint[3] = {25.0, 26.5, 27.0};
float temperature[3];

union {
  float f;
  byte b[4];
} floatToByte;

void setup() {
  pinMode(masterSSPin, OUTPUT);
  digitalWrite(masterSSPin, HIGH);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV4);

  Serial.begin(115200);
}

void loop() {
  digitalWrite(masterSSPin, LOW);
  delay(10);

  uint8_t buf[10];
  for(int i=0;i<10;i++){
    buf[i] = SPI.transfer(i);
    Serial.print(buf[i]);
    Serial.print("\t");
  }
  Serial.println();

  digitalWrite(masterSSPin, HIGH);

  delay(1000); // Wait
}
