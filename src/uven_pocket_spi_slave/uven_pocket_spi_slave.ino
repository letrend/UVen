// SPI slave for DUE, hack
//  master will lower CS and manage CLK
//
// Connects using the 6-pin SPI header for MISO (pin 1), MOSI (pin 4) and SCK (pin 3), 
//   and Digital Pin 10 for Chip Select
//
// Uses an interrupt to receive data, needed for high-speed SPI data transfers, tested up to
// 5Mhz with no data loss.


#include <SPI.h>
#include <stdint.h>


// SPI 0 interrupt for the SAM3XA chip:
#define SPI0_INTERRUPT_NUMBER (IRQn_Type)24

// Buffer sized as needed
#define BUFFER_SIZE 16

// Default chip select pin, not tested with any other pins
#define CS 10

union SPI_FRAME{
  struct{
    uint8_t control[2];
    uint8_t intensity[2];
    float temperature[3];
  }values;
  byte data[BUFFER_SIZE];
};

static volatile SPI_FRAME cmd, res;

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

void setup() {
  // Setup Serial
  Serial.begin(115200);
  pinMode(CS,INPUT);
  while(digitalRead(CS)==0);
  // Setup the SPI as Slave
  slaveBegin(CS);
  
}

void loop() {
  // If transfer is complete send the data.
  if ( pos == BUFFER_SIZE )
  {
    for(int i=0;i<BUFFER_SIZE;i++){
      Serial.print(buff[i],HEX);
      Serial.print("\t");
      cmd.data[i] = buff[i];
    }
    
    Serial.println();
    Serial.print("control_field: ");
    Serial.print(cmd.values.control[0]);
    Serial.print("\t");
    Serial.println(cmd.values.control[1]);
    Serial.print("intensity: ");
    Serial.print(cmd.values.intensity[0]);
    Serial.print("\t");
    Serial.println(cmd.values.intensity[1]);
    Serial.print("temperature: ");
    Serial.print(cmd.values.temperature[0]);
    Serial.print("\t");
    Serial.print(cmd.values.temperature[1]);
    Serial.print("\t");
    Serial.println(cmd.values.temperature[2]);

    res.values.control[0] = cmd.values.control[0];
    res.values.control[1] = cmd.values.control[1];
    res.values.intensity[0] = cmd.values.intensity[0];
    res.values.intensity[1] = cmd.values.intensity[1];
    res.values.temperature[0] = random(20, 30); 
    res.values.temperature[1] = random(20, 30); 
    res.values.temperature[2] = random(20, 30); 
    
    pos = 0;
    REG_SPI0_TDR = res.data[0];
  }
  
  
}
