#include <SPI.h>
#include <Arduino_CRC32.h>
Arduino_CRC32 crc32;
#include "seeed_line_chart.h" //include the library

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft); // Sprite

#define MAX_SIZE 30 // maximum size of data
doubles temp[3];       // Initilising a doubles type to store data

const byte CS = 1; // Chip Select pin

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

void setup() {
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV16);

  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  spr.createSprite(TFT_HEIGHT, TFT_WIDTH);
  spr.setRotation(3);
}

void loop() {
  digitalWrite(CS, LOW);
  delay(10);

  cmd.values.control[0] = cmd.values.control[0]+1;
  cmd.values.control[1] = cmd.values.control[1]+1;
  cmd.values.intensity[0] = 8;
  cmd.values.intensity[1] = 8;
  cmd.values.time = 0;
  cmd.values.temperature[0] = 24;
  cmd.values.temperature[1] = 24;
  cmd.values.temperature[2] = 24;
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
    spr.fillSprite(TFT_WHITE);
    if (temp[0].size() > MAX_SIZE)
    {
        temp[0].pop(); 
        temp[1].pop();
        temp[2].pop();
    }
    temp[0].push(res.values.temperature[0]);
    temp[1].push(res.values.temperature[1]);
    temp[2].push(res.values.temperature[2]);
    // Settings for the line graph title
    auto header = text(0, 0)
                      .value("Light Sensor Readings")
                      .align(center)
                      .valign(vcenter)
                      .width(spr.width())
                      .thickness(2);

    header.height(header.font_height(&spr) * 2);
    header.draw(&spr); // Header height is the twice the height of the font

    // Settings for the line graph
    auto content = line_chart(20, header.height()); //(x,y) where the line graph begins
    content
        .height(spr.height() - header.height() * 1.5) // actual height of the line chart
        .width(spr.width() - content.x() * 2)         // actual width of the line chart
        .based_on(0.0)                                // Starting point of y-axis, must be a float
        .show_circle(false)                           // drawing a cirle at each point, default is on.
        .value(temp[0])                                  // passing through the data to line graph
        .max_size(MAX_SIZE)
        .color(TFT_RED)                               // Setting the color for the line
        .backgroud(TFT_WHITE)
        .draw(&spr);

    spr.pushSprite(0, 0);
  }else{
    Serial.println("crc mismatch");
  }

//  delay(100); // Wait
}
