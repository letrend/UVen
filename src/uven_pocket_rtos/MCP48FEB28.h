#ifndef MCP48FEB28_LIB_MCP48FEB28_H
#define MCP48FEB28_LIB_MCP48FEB28_H

#include <SPI.h>

class MCP48FEB28{
  public:
  MCP48FEB28(uint8_t cs, uint8_t latch):cs(cs),latch(latch){
     SPI.begin();
     
  }

  ~MCP48FEB28();

  void init(){
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH);
    pinMode(latch, OUTPUT);
    digitalWrite(latch, LOW);
  }

  void write(uint8_t channel, uint16_t val){
    /* begin transaction using maximum clock frequency of 20MHz */
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    digitalWrite(cs, LOW); //select device
    SPI.transfer(channel<<3);
    SPI.transfer16(val);
    digitalWrite(cs, HIGH); //deselect device
    digitalWrite(latch,HIGH);
    digitalWrite(latch,LOW);
  }
  
  private:
    const uint8_t cs, latch;
};

#endif //MCP48FEB28_LIB_MCP48FEB28_H
