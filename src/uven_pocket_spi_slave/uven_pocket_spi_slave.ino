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
#define BUFFER_SIZE 10

// Default chip select pin, not tested with any other pins
#define SS 10

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
    REG_SPI0_TDR = buff[pos];
    pos++;
}

// Debug printing of the register
#define PRREG(x) Serial.print(#x" 0x"); Serial.println(x,HEX)

// Print the main SPI registers.
// NOTE: Not worth trying to print the SPI Interrupt Enable/Disable registers, they are write only.
void prregs() {
  PRREG(REG_SPI0_MR);
  PRREG(REG_SPI0_CSR);
  PRREG(REG_SPI0_SR);
}


void setup() {
  // Setup Serial
  Serial.begin(115200);
  
  prregs();  // debug

  // Setup the SPI as Slave
  slaveBegin(SS);
  
  prregs();  // debug
}

void loop() {
  // If transfer is complete send the data.
  if ( pos == BUFFER_SIZE )
  {
    for(int i=0;i<BUFFER_SIZE;i++){
      Serial.print(buff[i]);
      Serial.print("\t");
    }
    Serial.println();
    
    pos = 0;
  }
  
  
}
