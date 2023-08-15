//<App !Start!>
// FILE: [uven2_mk2_wio_terminal_guislicer.ino]
// Created by GUIslice Builder version: [0.17.b24]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<App !End!>

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "uven2_mk2_wio_terminal_guislicer_GSLC.h"


// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef* m_pElemProgress1  = NULL;
//<Save_References !End!>

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
//<Button Callback !Start!>
//<Button Callback !End!>
//<Checkbox Callback !Start!>
//<Checkbox Callback !End!>
//<Keypad Callback !Start!>
//<Keypad Callback !End!>
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>
//<Slider Callback !Start!>
//<Slider Callback !End!>
//<Tick Callback !Start!>
//<Tick Callback !End!>

void setup()
{
  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  Serial.begin(9600);
  // Wait for USB Serial 
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used
  pinMode(WIO_KEY_A,INPUT_PULLUP);
  pinMode(WIO_KEY_B,INPUT_PULLUP);
  gslc_InitDebug(&DebugOut);

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUIslice_gen();

}

// -----------------------------------
// Main event loop
// -----------------------------------
int val = 0, iter = 0;
void loop()
{

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  //TODO - Add update code for any text, gauges, or sliders
  if(!digitalRead(WIO_KEY_A)){
//    gslc_ElemXProgressSetVal(&m_gui,m_pElemProgress1,((val++)%100));
    gslc_SetPageCur(&m_gui,E_PG_MAIN);
  }
  if(!digitalRead(WIO_KEY_B)){
//    gslc_ElemXProgressSetVal(&m_gui,m_pElemProgress1,((val--)%100));
    gslc_SetPageCur(&m_gui,E_PG2);
  }
  iter++;
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
}
