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
#include "elem/XTogglebtn.h"
#include "elem/XSlider.h"
#include <Arduino_CRC32.h>
#include <EasyButton.h>
Arduino_CRC32 crc32;
#include "seeed_line_chart.h"
TFT_eSPI tft;

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef* m_chamberFanSlider= NULL;
gslc_tsElemRef* m_drvTemp         = NULL;
gslc_tsElemRef* m_drvTempSlider   = NULL;
gslc_tsElemRef* m_fire            = NULL;
gslc_tsElemRef* m_intensity       = NULL;
gslc_tsElemRef* m_intensitySlider = NULL;
gslc_tsElemRef* m_interlock       = NULL;
gslc_tsElemRef* m_ledTemp         = NULL;
gslc_tsElemRef* m_ledTempSlider   = NULL;
gslc_tsElemRef* m_overTemp        = NULL;
gslc_tsElemRef* m_rep             = NULL;
gslc_tsElemRef* m_repEnable       = NULL;
gslc_tsElemRef* m_repSec          = NULL;
gslc_tsElemRef* m_timeHour        = NULL;
gslc_tsElemRef* m_timeMin         = NULL;
gslc_tsElemRef* m_timeSec         = NULL;
gslc_tsElemRef* m_pElemKeyPadNum  = NULL;
//<Save_References !End!>

#define SERIAL_FRAME_RX_BUFFER_SIZE 16
#define SERIAL_FRAME_TX_BUFFER_SIZE 8

union SERIAL_FRAME_RX{
  struct{
    uint16_t status;
    uint16_t led_status;
    float led_temp;
    float drv_temp;
    uint32_t crc;
  }values;
  volatile uint8_t data[SERIAL_FRAME_RX_BUFFER_SIZE];
};

union SERIAL_FRAME_TX{
  struct{
    uint16_t chamber_fan;
    uint16_t target_current;
    uint32_t crc;
  }values;
  volatile uint8_t data[SERIAL_FRAME_TX_BUFFER_SIZE];
};

SERIAL_FRAME_TX tx_serial_frame;
SERIAL_FRAME_RX rx_serial_frame;

EasyButton but_a(WIO_KEY_A), but_b(WIO_KEY_B), but_c(WIO_KEY_C);
EasyButton but_up(WIO_5S_UP), but_down(WIO_5S_DOWN), but_left(WIO_5S_LEFT), but_right(WIO_5S_RIGHT), but_press(WIO_5S_PRESS);

bool fire = false, interlock = false, overTemp = false;
int fire_time_start = 0, fire_time_elapsed = 0, fire_time_target = 0, rep_pause_time_start = 0, rep_pause_time_elapsed = 0, reps = 0;

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
        gslc_ElemXKeyPadInputAsk(&m_gui, m_pElemKeyPadNum, E_POP_KEYPAD_NUM, m_intensity);
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
        gslc_ElemXKeyPadInputGet(pGui, m_intensity, pvData);
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
      nVal = gslc_ElemXSeekbarGetPos(pGui,m_chamberFanSlider);
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

int press_duration = 20, row_selected = 2, col_selected = 2;
bool rep_enable = false, rep_enabled = false;
int drvTemp = 0, ledTemp = 0, intensity = 250, chamber_fan = 0, rep = 3, repSec = 1, minute = 0, second = 0, hour = 0;

void updateMenuSelected(){
  gslc_ElemSetGlow(&m_gui, m_repEnable, false);
  gslc_ElemSetGlow(&m_gui, m_rep, false);
  gslc_ElemSetGlow(&m_gui, m_repSec, false);
  gslc_ElemSetGlow(&m_gui, m_timeHour, false);
  gslc_ElemSetGlow(&m_gui, m_timeMin, false);
  gslc_ElemSetGlow(&m_gui, m_timeSec, false);
  gslc_ElemSetGlow(&m_gui, m_intensitySlider, false);
  gslc_ElemSetGlow(&m_gui, m_chamberFanSlider, false);
  switch(row_selected){
    case 0: 
      if(col_selected==0){
        gslc_ElemSetGlow(&m_gui, m_repEnable, true); 
      }else if(col_selected==1){
        gslc_ElemSetGlow(&m_gui, m_rep, true);
      }else if(col_selected==2){
        gslc_ElemSetGlow(&m_gui, m_repSec, true);
      }
      break;
   case 1: 
      if(col_selected==0){
        gslc_ElemSetGlow(&m_gui, m_timeHour, true); 
      }else if(col_selected==1){
        gslc_ElemSetGlow(&m_gui, m_timeMin, true);
      }else if(col_selected==2){
        gslc_ElemSetGlow(&m_gui, m_timeSec, true);
      }
      break;
    case 2: gslc_ElemSetGlow(&m_gui, m_intensitySlider, true); break;
    case 3: gslc_ElemSetGlow(&m_gui, m_chamberFanSlider, true); break;
  }
  
}

void updateText(){
  char str4[4];
  sprintf(str4,"%d", intensity);
  gslc_ElemSetTxtStr(&m_gui, m_intensity, str4);
  if(!fire){
    char str2[2];
    sprintf(str2,"%d", rep);
    gslc_ElemSetTxtStr(&m_gui, m_rep, str2);
    sprintf(str2,"%d", repSec);
    gslc_ElemSetTxtStr(&m_gui, m_repSec, str2);
    sprintf(str2,"%d", second);
    gslc_ElemSetTxtStr(&m_gui, m_timeSec, str2);
    sprintf(str2,"%d", minute);
    gslc_ElemSetTxtStr(&m_gui, m_timeMin, str2);
    sprintf(str2,"%d", hour);
    gslc_ElemSetTxtStr(&m_gui, m_timeHour, str2);
  }
}

void aPressed() {
    static int t0 = millis();
    int t1 = millis();
    if((t1-t0)>150){
      t0 = t1;
      switch(row_selected){
        case 0: {
          if(col_selected==1){
            rep++;
            if(rep>99){
              rep = 99;
            }
          }else if(col_selected==2){
            repSec++;
            if(repSec>99){
              repSec = 99;
            }
          }
          break;
        }
        case 1: {
          if(!fire){
            if(col_selected==0){
              hour++;
              if(hour>99){
                hour = 99;
              }
            }else if(col_selected==1){
              minute++;
              if(minute>60){
                minute = 0;
                hour++;
              }
            }else if(col_selected==2){
              second++;
              if(second>60){
                second = 0;
                minute++;
                if(minute>60){
                  minute = 0;
                  hour++;
                  if(hour>99){
                    hour = 99;
                  }
                }
              }
            }
            fire_time_target = hour*3600+minute*60+second;
          }
          break;
        }
        case 2:{
          intensity+=250;
          if(intensity>4000){
            intensity = 4000;
          }
          gslc_ElemXSeekbarSetPos(&m_gui, m_intensitySlider, (int16_t)intensity/40);
          break;
        }
        case 3:{
          chamber_fan+=5;
          if(chamber_fan>100){
            chamber_fan = 100;
          }
          gslc_ElemXSeekbarSetPos(&m_gui, m_chamberFanSlider, (int16_t)chamber_fan);
          break;
        }
      }
      updateText();
    }
}

void bPressed() {
  if(!interlock && !overTemp){
    fire = !fire;
    if(fire){
      fire_time_start = millis();
    }
    if(rep_enable){
      reps = rep-1;
    }
  }
}

void cPressed() {
    static int t0 = millis();
    int t1 = millis();
    if((t1-t0)>150){
      t0 = t1;
      switch(row_selected){
        case 0: {
          if(col_selected==1){
            rep--;
            if(rep<1){
              rep = 1;
            }
          }else if(col_selected==2){
            repSec--;
            if(repSec<1){
              repSec = 1;
            }
          }
          break;
        }
        case 1: {
          if(!fire){
            if(col_selected==0){
              hour--;
              if(hour<0){
                hour = 0;
              }
            }else if(col_selected==1){
              minute--;
              if(minute<0){
                minute = 0;
                hour--;
              }
            }else if(col_selected==2){
              second--;
              if(second<0){
                second = 0;
                minute--;
                if(minute<0){
                  minute = 0;
                  hour--;
                  if(hour<0){
                    hour = 0;
                  }
                }
              }
            }
            fire_time_target = hour*3600+minute*60+second;
          }
          break;
        }
        case 2:{
          intensity-=250;
          if(intensity<0){
            intensity = 0;
          }
          gslc_ElemXSeekbarSetPos(&m_gui, m_intensitySlider, (int16_t)intensity/40);
          break;
        }
        case 3:{
          chamber_fan-=5;
          if(chamber_fan<0){
            chamber_fan = 0;
          }
          gslc_ElemXSeekbarSetPos(&m_gui, m_chamberFanSlider, (int16_t)chamber_fan);
          break;
        }
      }
      updateText();
  }
}

void upPressed() {
  row_selected--;
  if(row_selected<0){
    row_selected = 3;
  }
  updateMenuSelected();
}

void downPressed() {
  row_selected++;
  if(row_selected>3){
    row_selected = 0;
  }
  updateMenuSelected();
}

void leftPressed() {
  col_selected--;
  if(col_selected<0){
    col_selected=0;
  }
  updateMenuSelected();
}

void rightPressed() {
  col_selected++;
  if(col_selected>2){
    col_selected=2;
  }
  updateMenuSelected();
}

void pressPressed() {
  if(row_selected==0 && col_selected==0){
    if(second>0 || minute>0 || hour>0){
      rep_enable = !rep_enable;
      gslc_ElemXTogglebtnSetState(&m_gui, m_repEnable, rep_enable);
    }
  }  
}

void setup()
{
  Serial.begin(4000000);
  
  but_a.begin(); but_b.begin(); but_c.begin();
  but_up.begin(); but_down.begin(); but_left.begin(); but_right.begin(); but_press.begin();
//  but_a.onPressedFor(press_duration, aPressed); 
  but_b.onPressedFor(500, bPressed);
//  but_c.onPressedFor(press_duration, cPressed);
  but_up.onPressedFor(press_duration, upPressed); 
  but_down.onPressedFor(press_duration, downPressed);
  but_left.onPressedFor(press_duration, leftPressed);
  but_right.onPressedFor(press_duration, rightPressed); 
  but_press.onPressedFor(press_duration, pressPressed);
  
  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUIslice_gen();
  gslc_ElemSetGlowCol(&m_gui, m_chamberFanSlider, GSLC_COL_GREEN_LT4, GSLC_COL_GREEN_LT4, GSLC_COL_GREEN_LT4);
  gslc_ElemSetGlowCol(&m_gui, m_rep, GSLC_COL_GREEN_LT4, GSLC_COL_GREEN_LT4, GSLC_COL_BLACK);
  gslc_ElemSetGlowEn(&m_gui, m_rep, true);
  gslc_ElemSetGlowCol(&m_gui, m_repEnable, GSLC_COL_GREEN_LT4, GSLC_COL_GREEN_LT4, GSLC_COL_GREEN_LT4);
  gslc_ElemSetGlowEn(&m_gui, m_repEnable, true);
  gslc_ElemSetGlowCol(&m_gui, m_repSec, GSLC_COL_GREEN_LT4, GSLC_COL_GREEN_LT4, GSLC_COL_BLACK);
  gslc_ElemSetGlowEn(&m_gui, m_repSec, true);
  gslc_ElemSetGlowCol(&m_gui, m_intensitySlider, GSLC_COL_GREEN_LT4, GSLC_COL_GREEN_LT4, GSLC_COL_GREEN_LT4);
  gslc_ElemSetGlowCol(&m_gui, m_timeHour, GSLC_COL_GREEN_LT4, GSLC_COL_GREEN_LT4, GSLC_COL_BLACK);
  gslc_ElemSetGlowEn(&m_gui, m_timeHour, true);
  gslc_ElemSetGlowCol(&m_gui, m_timeMin, GSLC_COL_GREEN_LT4, GSLC_COL_GREEN_LT4, GSLC_COL_BLACK);
  gslc_ElemSetGlowEn(&m_gui, m_timeMin, true);
  gslc_ElemSetGlowCol(&m_gui, m_timeSec, GSLC_COL_GREEN_LT4, GSLC_COL_GREEN_LT4, GSLC_COL_BLACK);
  gslc_ElemSetGlowEn(&m_gui, m_timeSec, true);

  updateMenuSelected();
  updateText();
}

// -----------------------------------
// Main event loop
// -----------------------------------

void loop()
{
  if(Serial.available()){
    Serial.readBytes((char*)&rx_serial_frame.data[0], SERIAL_FRAME_RX_BUFFER_SIZE);
    uint32_t crc = crc32.calc((uint8_t const *)&rx_serial_frame.data[0], SERIAL_FRAME_RX_BUFFER_SIZE-4);
    if(crc==rx_serial_frame.values.crc){
      if(fire){
        tx_serial_frame.values.target_current = gslc_ElemXSeekbarGetPos(&m_gui, m_intensitySlider)*40;
      }else{
        tx_serial_frame.values.target_current = 0;
      }
      tx_serial_frame.values.chamber_fan = gslc_ElemXSeekbarGetPos(&m_gui, m_chamberFanSlider);
      
      tx_serial_frame.values.crc = crc32.calc((uint8_t const *)&tx_serial_frame.data[0], SERIAL_FRAME_TX_BUFFER_SIZE-4);
      Serial.write((char*)tx_serial_frame.data,SERIAL_FRAME_TX_BUFFER_SIZE);

      interlock = (rx_serial_frame.values.status&0x1);
      overTemp = ((rx_serial_frame.values.status>>1)&0x1);
      gslc_ElemXProgressSetVal(&m_gui,m_interlock,(interlock?100:0));
      gslc_ElemXProgressSetVal(&m_gui,m_overTemp,(overTemp?100:0));
      gslc_ElemXProgressSetVal(&m_gui,m_fire,(fire?100:0));
      if(overTemp || interlock){
        fire = false;
      }
      if(rx_serial_frame.values.drv_temp<=40){
        gslc_ElemXProgressSetGaugeCol(&m_gui,m_drvTempSlider,GSLC_COL_BLUE_LT1);
      }else if(rx_serial_frame.values.drv_temp>40 && rx_serial_frame.values.drv_temp<70){
        gslc_ElemXProgressSetGaugeCol(&m_gui,m_drvTempSlider,GSLC_COL_YELLOW);
      }else{
        gslc_ElemXProgressSetGaugeCol(&m_gui,m_drvTempSlider,GSLC_COL_RED);
      }

      if(rx_serial_frame.values.led_temp<=40){
        gslc_ElemXProgressSetGaugeCol(&m_gui,m_ledTempSlider,GSLC_COL_BLUE_LT1);
      }else if(rx_serial_frame.values.led_temp>40 && rx_serial_frame.values.led_temp<70){
        gslc_ElemXProgressSetGaugeCol(&m_gui,m_ledTempSlider,GSLC_COL_YELLOW);
      }else{
        gslc_ElemXProgressSetGaugeCol(&m_gui,m_ledTempSlider,GSLC_COL_RED);
      }
      
      gslc_ElemXProgressSetVal(&m_gui,m_drvTempSlider,(int)(rx_serial_frame.values.drv_temp>=20?rx_serial_frame.values.drv_temp:20));
      gslc_ElemXProgressSetVal(&m_gui,m_ledTempSlider,(int)(rx_serial_frame.values.led_temp>=20?rx_serial_frame.values.led_temp:20));
      char str3[3];
      sprintf(str3,"%d", (int)(rx_serial_frame.values.drv_temp>=20?rx_serial_frame.values.drv_temp:20));
      gslc_ElemSetTxtStr(&m_gui, m_drvTemp, str3);
      sprintf(str3,"%d", (int)(rx_serial_frame.values.led_temp>=20?rx_serial_frame.values.led_temp:20));
      gslc_ElemSetTxtStr(&m_gui, m_ledTemp, str3);
    }else{
      tft.fillScreen(TFT_RED);
      char str[20];
      sprintf(str,"%x != %x",crc,rx_serial_frame.values.crc);
      tft.drawString(str, 100, 0);
    }
  }
  
  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  but_a.read(); but_b.read(); but_c.read();
  but_up.read(); but_down.read(); but_left.read(); but_right.read(); but_press.read();
  if(!digitalRead(WIO_KEY_A)){
    aPressed();
  }else if(!digitalRead(WIO_KEY_C)){
    cPressed();
  }

  if(fire){
    fire_time_elapsed = (millis()-fire_time_start)/1000;
    char str2[2];
    
    if(fire_time_target!=0){
      if(fire_time_elapsed>=fire_time_target){
        fire = false;
      }
      int fire_time_left = fire_time_target-fire_time_elapsed;
      int minutes_left = fire_time_left/60;
      int hours_left = fire_time_left/60;
      sprintf(str2,"%d", fire_time_left%60);
      gslc_ElemSetTxtStr(&m_gui, m_timeSec, str2);
      sprintf(str2,"%d", minutes_left%60);
      gslc_ElemSetTxtStr(&m_gui, m_timeMin, str2);
      sprintf(str2,"%d", hours_left%60);
      gslc_ElemSetTxtStr(&m_gui, m_timeHour, str2);
    }else{
      int minutes_elapsed = fire_time_elapsed/60;
      int hours_elapsed = minutes_elapsed/60;
      sprintf(str2,"%d", fire_time_elapsed%60);
      gslc_ElemSetTxtStr(&m_gui, m_timeSec, str2);
      sprintf(str2,"%d", minutes_elapsed%60);
      gslc_ElemSetTxtStr(&m_gui, m_timeMin, str2);
      sprintf(str2,"%d", hours_elapsed%60);
      gslc_ElemSetTxtStr(&m_gui, m_timeHour, str2);
    }
    
    if(rep_enable && !rep_enabled){
      rep_enabled = true;
    }
  }else{
    if(rep_enable){
      if((reps>0)){
        if(!interlock && !overTemp){
          if(rep_enabled){
            rep_enabled = false;
            rep_pause_time_start = millis();
          }else{
            rep_pause_time_elapsed = (millis()-rep_pause_time_start)/1000;
            char str2[2];
            if(rep_pause_time_elapsed>(repSec-1)){
              reps--;
              sprintf(str2,"%d", reps);
              gslc_ElemSetTxtStr(&m_gui, m_rep, str2);
              fire = true;
              fire_time_start = millis();
            }else{
              sprintf(str2,"%d", rep_pause_time_elapsed);
              gslc_ElemSetTxtStr(&m_gui, m_repSec, str2);
            }
          }
        }
      }
    }
    updateText();
  }
  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
}
