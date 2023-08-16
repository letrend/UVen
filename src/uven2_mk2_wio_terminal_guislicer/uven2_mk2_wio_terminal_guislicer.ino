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
#include <Arduino_CRC32.h>
#include <EasyButton.h>
Arduino_CRC32 crc32;
#include "seeed_line_chart.h"
TFT_eSPI tft;

#define BUFFER_SIZE 176

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef* m_chamberFan      = NULL;
gslc_tsElemRef* m_drvTemp         = NULL;
gslc_tsElemRef* m_drvTempSlider   = NULL;
gslc_tsElemRef* m_intensitySlider = NULL;
gslc_tsElemRef* m_interlock       = NULL;
gslc_tsElemRef* m_ledTemp         = NULL;
gslc_tsElemRef* m_ledTempSlider   = NULL;
gslc_tsElemRef* m_rep             = NULL;
gslc_tsElemRef* m_repEnable       = NULL;
gslc_tsElemRef* m_repSec          = NULL;
gslc_tsElemRef* m_slider          = NULL;
gslc_tsElemRef* m_timeHour        = NULL;
gslc_tsElemRef* m_timeMin         = NULL;
gslc_tsElemRef* m_timeSec         = NULL;
gslc_tsElemRef* m_pElemKeyPadNum  = NULL;
//<Save_References !End!>

union SERIAL_FRAME{
  struct{
    uint32_t time;
    float temperature[17];
    uint16_t led_fan;
    uint16_t chamber_fan;
    uint16_t target_current[16];
    uint16_t current[16];
    uint16_t gate[16];
    uint32_t crc;
  }values;
  volatile uint8_t data[BUFFER_SIZE];
};

SERIAL_FRAME rx, tx;

EasyButton but_a(WIO_KEY_A), but_b(WIO_KEY_B), but_c(WIO_KEY_C);
EasyButton but_up(WIO_5S_UP), but_down(WIO_5S_DOWN), but_left(WIO_5S_LEFT), but_right(WIO_5S_RIGHT), but_press(WIO_5S_PRESSs);

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>
      case E_ELEM_NUMINPUT1:
        // Clicked on edit field, so show popup box and associate with this text field
        gslc_ElemXKeyPadInputAsk(&m_gui, m_pElemKeyPadNum, E_POP_KEYPAD_NUM, m_drvTemp);
        break;
      case E_ELEM_NUMINPUT2:
        // Clicked on edit field, so show popup box and associate with this text field
        gslc_ElemXKeyPadInputAsk(&m_gui, m_pElemKeyPadNum, E_POP_KEYPAD_NUM, m_ledTemp);
        break;
      case E_ELEM_NUMINPUT3:
        // Clicked on edit field, so show popup box and associate with this text field
        gslc_ElemXKeyPadInputAsk(&m_gui, m_pElemKeyPadNum, E_POP_KEYPAD_NUM, m_slider);
        break;
      case E_ELEM_NUMINPUT4:
        // Clicked on edit field, so show popup box and associate with this text field
        gslc_ElemXKeyPadInputAsk(&m_gui, m_pElemKeyPadNum, E_POP_KEYPAD_NUM, m_timeSec);
        break;
      case E_ELEM_NUMINPUT5:
        // Clicked on edit field, so show popup box and associate with this text field
        gslc_ElemXKeyPadInputAsk(&m_gui, m_pElemKeyPadNum, E_POP_KEYPAD_NUM, m_timeMin);
        break;
      case E_ELEM_NUMINPUT6:
        // Clicked on edit field, so show popup box and associate with this text field
        gslc_ElemXKeyPadInputAsk(&m_gui, m_pElemKeyPadNum, E_POP_KEYPAD_NUM, m_timeHour);
        break;
      case E_ELEM_NUMINPUT7:
        // Clicked on edit field, so show popup box and associate with this text field
        gslc_ElemXKeyPadInputAsk(&m_gui, m_pElemKeyPadNum, E_POP_KEYPAD_NUM, m_repSec);
        break;
      case E_ELEM_TOGGLE1:
        // TODO Add code for Toggle button ON/OFF state
        if (gslc_ElemXTogglebtnGetState(&m_gui, m_repEnable)) {
          ;
        }
        break;
      case E_ELEM_NUMINPUT8:
        // Clicked on edit field, so show popup box and associate with this text field
        gslc_ElemXKeyPadInputAsk(&m_gui, m_pElemKeyPadNum, E_POP_KEYPAD_NUM, m_rep);
        break;

//<Button Enums !End!>
      default:
        break;
    }
  }
  return true;
}
//<Checkbox Callback !Start!>
//<Checkbox Callback !End!>
// KeyPad Input Ready callback
bool CbKeypad(void* pvGui, void *pvElemRef, int16_t nState, void* pvData)
{
  gslc_tsGui*     pGui     = (gslc_tsGui*)pvGui;
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);

  // From the pvData we can get the ID element that is ready.
  int16_t nTargetElemId = gslc_ElemXKeyPadDataTargetIdGet(pGui, pvData);
  if (nState == XKEYPAD_CB_STATE_DONE) {
    // User clicked on Enter to leave popup
    // - If we have a popup active, pass the return value directly to
    //   the corresponding value field
    switch (nTargetElemId) {
//<Keypad Enums !Start!>
      case E_ELEM_NUMINPUT1:
        gslc_ElemXKeyPadInputGet(pGui, m_drvTemp, pvData);
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_NUMINPUT2:
        gslc_ElemXKeyPadInputGet(pGui, m_ledTemp, pvData);
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_NUMINPUT3:
        gslc_ElemXKeyPadInputGet(pGui, m_slider, pvData);
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_NUMINPUT4:
        gslc_ElemXKeyPadInputGet(pGui, m_timeSec, pvData);
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_NUMINPUT5:
        gslc_ElemXKeyPadInputGet(pGui, m_timeMin, pvData);
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_NUMINPUT6:
        gslc_ElemXKeyPadInputGet(pGui, m_timeHour, pvData);
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_NUMINPUT7:
        gslc_ElemXKeyPadInputGet(pGui, m_repSec, pvData);
        gslc_PopupHide(&m_gui);
        break;
      case E_ELEM_NUMINPUT8:
        gslc_ElemXKeyPadInputGet(pGui, m_rep, pvData);
        gslc_PopupHide(&m_gui);
        break;

//<Keypad Enums !End!>
      default:
        break;
    }
  } else if (nState == XKEYPAD_CB_STATE_CANCEL) {
    // User escaped from popup, so don't update values
    gslc_PopupHide(&m_gui);
  }
  return true;
}
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos)
{
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);
  int16_t         nVal;

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
//<Slider Enums !Start!>
    case E_ELEM_SEEKBAR1:
      // Fetch the slider position
      nVal = gslc_ElemXSeekbarGetPos(pGui,m_chamberFan);
      break;
    case E_ELEM_SEEKBAR4:
      // Fetch the slider position
      nVal = gslc_ElemXSeekbarGetPos(pGui,m_intensitySlider);
      break;

//<Slider Enums !End!>
    default:
      break;
  }

  return true;
}
//<Tick Callback !Start!>
//<Tick Callback !End!>

int press_duration = 100;

void aPressed() {
    Serial.println("Button a has been pressed for the given duration!");
}

void bPressed() {
    Serial.println("Button b has been pressed for the given duration!");
}

void cPressed() {
    Serial.println("Button c has been pressed for the given duration!");
}

void upPressed() {
    Serial.println("Button up has been pressed for the given duration!");
}

void downPressed() {
    Serial.println("Button down has been pressed for the given duration!");
}

void leftPressed() {
    Serial.println("Button left has been pressed for the given duration!");
}

void rightPressed() {
    Serial.println("Button right has been pressed for the given duration!");
}

void pressPressed() {
    Serial.println("Button press has been pressed for the given duration!");
}

void setup()
{
  Serial.begin();
  
  but_a.begin(); but_b.begin(); but_c.begin();
  but_up.begin(); but_down.begin(); but_left.begin(); but_right.begin(); but_press.begin();
  but_a.onPressedFor(press_duration, aPressed);
  
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
