//<File !Start!>
// FILE: [uven2_mk2_wio_terminal_guislicer_GSLC.h]
// Created by GUIslice Builder version: [0.17.b24]
//
// GUIslice Builder Generated GUI Framework File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>

#ifndef _GUISLICE_GEN_H
#define _GUISLICE_GEN_H

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
//<Includes !Start!>
// Include extended elements
#include "elem/XKeyPad_Num.h"
#include "elem/XProgress.h"
#include "elem/XSeekbar.h"
#include "elem/XTogglebtn.h"

// Ensure optional features are enabled in the configuration
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#if !defined(DRV_DISP_TFT_ESPI)
  #error E_PROJECT_OPTIONS tab->Graphics Library should be Adafruit_GFX
#endif
#include <TFT_eSPI.h>
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PG_MAIN,E_PG2,E_POP_KEYPAD_NUM};
enum {E_ELEM_BOX1,E_ELEM_BOX2,E_ELEM_BOX3,E_ELEM_BOX5,E_ELEM_BOX6
      ,E_ELEM_NUMINPUT1,E_ELEM_NUMINPUT2,E_ELEM_NUMINPUT3
      ,E_ELEM_NUMINPUT4,E_ELEM_NUMINPUT5,E_ELEM_NUMINPUT6
      ,E_ELEM_NUMINPUT7,E_ELEM_NUMINPUT8,E_ELEM_PROGRESS1
      ,E_ELEM_PROGRESS3,E_ELEM_PROGRESS4,E_ELEM_PROGRESS5
      ,E_ELEM_PROGRESS6,E_ELEM_SEEKBAR1,E_ELEM_SEEKBAR4,E_ELEM_TEXT1
      ,E_ELEM_TEXT10,E_ELEM_TEXT11,E_ELEM_TEXT2,E_ELEM_TEXT3
      ,E_ELEM_TEXT4,E_ELEM_TEXT5,E_ELEM_TEXT6,E_ELEM_TEXT7,E_ELEM_TEXT8
      ,E_ELEM_TEXT9,E_ELEM_TOGGLE1,E_ELEM_KEYPAD_NUM};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_BUILTIN5X8,E_FREEMONO12,E_FREEMONO9,E_FREEMONOBOLD12,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                3

#define MAX_ELEM_PG_MAIN 32 // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

#define MAX_ELEM_PG2 0 // # Elems total on page
#define MAX_ELEM_PG2_RAM MAX_ELEM_PG2 // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                     m_asPage2Elem[MAX_ELEM_PG2_RAM];
gslc_tsElemRef                  m_asPage2ElemRef[MAX_ELEM_PG2];
gslc_tsElem                     m_asKeypadNumElem[1];
gslc_tsElemRef                  m_asKeypadNumElemRef[1];
gslc_tsXKeyPad                  m_sKeyPadNum;
gslc_tsXProgress                m_sXBarGauge5;
gslc_tsXProgress                m_sXBarGauge6;
gslc_tsXProgress                m_sXBarGauge4;
gslc_tsXProgress                m_sXBarGauge1;
gslc_tsXSeekbar                 m_sXSeekbar1;
gslc_tsXProgress                m_sXBarGauge3;
gslc_tsXSeekbar                 m_sXSeekbar4;
gslc_tsXTogglebtn               m_asXToggle1;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Element References for direct access
//<Extern_References !Start!>
extern gslc_tsElemRef* m_chamberFanSlider;
extern gslc_tsElemRef* m_drvTemp;
extern gslc_tsElemRef* m_drvTempSlider;
extern gslc_tsElemRef* m_fire;
extern gslc_tsElemRef* m_intensity;
extern gslc_tsElemRef* m_intensitySlider;
extern gslc_tsElemRef* m_interlock;
extern gslc_tsElemRef* m_ledTemp;
extern gslc_tsElemRef* m_ledTempSlider;
extern gslc_tsElemRef* m_overTemp;
extern gslc_tsElemRef* m_rep;
extern gslc_tsElemRef* m_repEnable;
extern gslc_tsElemRef* m_repSec;
extern gslc_tsElemRef* m_timeHour;
extern gslc_tsElemRef* m_timeMin;
extern gslc_tsElemRef* m_timeSec;
extern gslc_tsElemRef* m_pElemKeyPadNum;
//<Extern_References !End!>

// Define debug message function
static int16_t DebugOut(char ch);

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY);
bool CbCheckbox(void* pvGui, void* pvElemRef, int16_t nSelId, bool bState);
bool CbDrawScanner(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);
bool CbKeypad(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbListbox(void* pvGui, void* pvElemRef, int16_t nSelId);
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos);
bool CbSpinner(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbTickScanner(void* pvGui,void* pvScope);

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
void InitGUIslice_gen()
{
  gslc_tsElemRef* pElemRef = NULL;

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
    if (!gslc_FontSet(&m_gui,E_BUILTIN5X8,GSLC_FONTREF_PTR,NULL,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FREEMONO12,GSLC_FONTREF_PTR,&FreeMono12pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FREEMONO9,GSLC_FONTREF_PTR,&FreeMono9pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FREEMONOBOLD12,GSLC_FONTREF_PTR,&FreeMonoBold12pt7b,1)) { return; }
//<Load_Fonts !End!>

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG2,m_asPage2Elem,MAX_ELEM_PG2_RAM,m_asPage2ElemRef,MAX_ELEM_PG2);
  gslc_PageAdd(&m_gui,E_POP_KEYPAD_NUM,m_asKeypadNumElem,1,m_asKeypadNumElemRef,1);  // KeyPad

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  

  // Create progress bar E_ELEM_PROGRESS5 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS5,E_PG_MAIN,&m_sXBarGauge5,
    (gslc_tsRect){2,24,93,22},0,100,0,GSLC_COL_BLUE_DK1,false);
  m_fire = pElemRef;

  // Create progress bar E_ELEM_PROGRESS6 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS6,E_PG_MAIN,&m_sXBarGauge6,
    (gslc_tsRect){95,24,93,22},0,100,0,GSLC_COL_RED,false);
  m_overTemp = pElemRef;

  // Create progress bar E_ELEM_PROGRESS4 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS4,E_PG_MAIN,&m_sXBarGauge4,
    (gslc_tsRect){2,2,186,22},0,100,0,GSLC_COL_RED,false);
  m_interlock = pElemRef;
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_MAIN,(gslc_tsRect){191,2,125,235});
   
  // Create E_ELEM_BOX2 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX2,E_PG_MAIN,(gslc_tsRect){2,175,186,62});
   
  // Create E_ELEM_BOX3 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX3,E_PG_MAIN,(gslc_tsRect){2,110,186,62});

  // Create progress bar E_ELEM_PROGRESS1 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS1,E_PG_MAIN,&m_sXBarGauge1,
    (gslc_tsRect){259,20,47,165},0,100,0,GSLC_COL_GREEN,true);
  m_drvTempSlider = pElemRef;

  // Create seekbar E_ELEM_SEEKBAR1 
  pElemRef = gslc_ElemXSeekbarCreate(&m_gui,E_ELEM_SEEKBAR1,E_PG_MAIN,&m_sXSeekbar1,
    (gslc_tsRect){10,200,175,30},0,106,0,
    4,2,8,GSLC_COL_BLUE,GSLC_COL_GRAY,GSLC_COL_BLUE_DK2,false);
  gslc_ElemXSeekbarSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  gslc_ElemXSeekbarSetStyle(&m_gui,pElemRef,true,GSLC_COL_BLUE_LT4,true,GSLC_COL_GRAY,
    0,10,GSLC_COL_GRAY);
  m_chamberFanSlider = pElemRef;
  
  // Create E_ELEM_TEXT1 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT1,E_PG_MAIN,(gslc_tsRect){268,220,32,10},
    (char*)"DRV",0,E_FREEMONO9);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT3);
  
  // Create E_ELEM_TEXT2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT2,E_PG_MAIN,(gslc_tsRect){10,184,119,11},
    (char*)"Chamber Fan",0,E_FREEMONO9);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT3);
  
  // Create E_ELEM_NUMINPUT1 numeric input field
  static char m_sInputNumber1[4] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_NUMINPUT1,E_PG_MAIN,(gslc_tsRect){261,195,47,17},
    (char*)m_sInputNumber1,4,E_FREEMONO12);
  gslc_ElemSetTxtMargin(&m_gui,pElemRef,5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_drvTemp = pElemRef;

  // Create progress bar E_ELEM_PROGRESS3 
  pElemRef = gslc_ElemXProgressCreate(&m_gui,E_ELEM_PROGRESS3,E_PG_MAIN,&m_sXBarGauge3,
    (gslc_tsRect){201,20,47,165},0,100,0,GSLC_COL_GREEN,true);
  m_ledTempSlider = pElemRef;
  
  // Create E_ELEM_TEXT3 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT3,E_PG_MAIN,(gslc_tsRect){208,220,30,10},
    (char*)"LED",0,E_FREEMONO9);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT3);
  
  // Create E_ELEM_NUMINPUT2 numeric input field
  static char m_sInputNumber2[4] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_NUMINPUT2,E_PG_MAIN,(gslc_tsRect){201,195,47,17},
    (char*)m_sInputNumber2,4,E_FREEMONO12);
  gslc_ElemSetTxtMargin(&m_gui,pElemRef,5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_ledTemp = pElemRef;
  
  // Create E_ELEM_TEXT4 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT4,E_PG_MAIN,(gslc_tsRect){193,5,90,10},
    (char*)"TEMPERATURE",0,E_FREEMONO9);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT3);
  
  // Create E_ELEM_NUMINPUT3 numeric input field
  static char m_sInputNumber3[5] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_NUMINPUT3,E_PG_MAIN,(gslc_tsRect){120,120,61,17},
    (char*)m_sInputNumber3,5,E_FREEMONO12);
  gslc_ElemSetTxtMargin(&m_gui,pElemRef,5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_intensity = pElemRef;

  // Create seekbar E_ELEM_SEEKBAR4 
  pElemRef = gslc_ElemXSeekbarCreate(&m_gui,E_ELEM_SEEKBAR4,E_PG_MAIN,&m_sXSeekbar4,
    (gslc_tsRect){10,140,175,30},0,108,20,
    10,2,12,((gslc_tsColor){0,255,238}),GSLC_COL_GRAY,GSLC_COL_BLUE_DK2,false);
  gslc_ElemXSeekbarSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  gslc_ElemXSeekbarSetStyle(&m_gui,pElemRef,true,GSLC_COL_BLUE_LT4,true,GSLC_COL_GRAY,
    0,10,GSLC_COL_GRAY);
  m_intensitySlider = pElemRef;
  
  // Create E_ELEM_TEXT6 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT6,E_PG_MAIN,(gslc_tsRect){10,122,95,13},
    (char*)"Intensity",0,E_FREEMONO9);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT3);
   
  // Create E_ELEM_BOX5 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX5,E_PG_MAIN,(gslc_tsRect){2,80,186,27});
  
  // Create E_ELEM_TEXT7 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT7,E_PG_MAIN,(gslc_tsRect){10,87,41,10},
    (char*)"Time",0,E_FREEMONO9);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT3);
  
  // Create E_ELEM_NUMINPUT4 numeric input field
  static char m_sInputNumber4[3] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_NUMINPUT4,E_PG_MAIN,(gslc_tsRect){150,85,33,17},
    (char*)m_sInputNumber4,3,E_FREEMONO12);
  gslc_ElemSetTxtMargin(&m_gui,pElemRef,5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_timeSec = pElemRef;
  
  // Create E_ELEM_NUMINPUT5 numeric input field
  static char m_sInputNumber5[3] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_NUMINPUT5,E_PG_MAIN,(gslc_tsRect){105,85,33,17},
    (char*)m_sInputNumber5,3,E_FREEMONO12);
  gslc_ElemSetTxtMargin(&m_gui,pElemRef,5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_timeMin = pElemRef;
  
  // Create E_ELEM_NUMINPUT6 numeric input field
  static char m_sInputNumber6[3] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_NUMINPUT6,E_PG_MAIN,(gslc_tsRect){60,85,33,17},
    (char*)m_sInputNumber6,3,E_FREEMONO12);
  gslc_ElemSetTxtMargin(&m_gui,pElemRef,5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_timeHour = pElemRef;
   
  // Create E_ELEM_BOX6 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX6,E_PG_MAIN,(gslc_tsRect){2,49,186,27});
  
  // Create E_ELEM_NUMINPUT7 numeric input field
  static char m_sInputNumber7[3] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_NUMINPUT7,E_PG_MAIN,(gslc_tsRect){150,54,33,17},
    (char*)m_sInputNumber7,3,E_FREEMONO12);
  gslc_ElemSetTxtMargin(&m_gui,pElemRef,5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_repSec = pElemRef;
  
  // Create toggle button E_ELEM_TOGGLE1
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_ELEM_TOGGLE1,E_PG_MAIN,&m_asXToggle1,
    (gslc_tsRect){5,53,35,20},GSLC_COL_GRAY,GSLC_COL_BLUE_DK2,GSLC_COL_GRAY_LT3,
    true,false,&CbBtnCommon);
  m_repEnable = pElemRef;
  
  // Create E_ELEM_NUMINPUT8 numeric input field
  static char m_sInputNumber8[3] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_NUMINPUT8,E_PG_MAIN,(gslc_tsRect){80,54,33,17},
    (char*)m_sInputNumber8,3,E_FREEMONO12);
  gslc_ElemSetTxtMargin(&m_gui,pElemRef,5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  gslc_ElemSetClickEn(&m_gui, pElemRef, true);
  gslc_ElemSetTouchFunc(&m_gui, pElemRef, &CbBtnCommon);
  m_rep = pElemRef;
  
  // Create E_ELEM_TEXT8 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT8,E_PG_MAIN,(gslc_tsRect){44,59,29,11},
    (char*)"rep",0,E_FREEMONO9);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT3);
  
  // Create E_ELEM_TEXT9 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT9,E_PG_MAIN,(gslc_tsRect){116,59,29,8},
    (char*)"sec",0,E_FREEMONO9);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT3);
  
  // Create E_ELEM_TEXT10 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT10,E_PG_MAIN,(gslc_tsRect){18,29,55,14},
    (char*)"FIRE",0,E_FREEMONOBOLD12);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  
  // Create E_ELEM_TEXT11 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT11,E_PG_MAIN,(gslc_tsRect){113,29,53,14},
    (char*)"TEMP",0,E_FREEMONOBOLD12);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  
  // Create E_ELEM_TEXT5 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT5,E_PG_MAIN,(gslc_tsRect){30,5,124,14},
    (char*)"INTERLOCK",0,E_FREEMONOBOLD12);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG2
  

  // -----------------------------------
  // PAGE: E_POP_KEYPAD_NUM
  
  static gslc_tsXKeyPadCfg_Num sCfg;
  sCfg = gslc_ElemXKeyPadCfgInit_Num();
  gslc_ElemXKeyPadCfgSetFloatEn_Num(&sCfg, true);
  gslc_ElemXKeyPadCfgSetSignEn_Num(&sCfg, true);
  m_pElemKeyPadNum = gslc_ElemXKeyPadCreate_Num(&m_gui, E_ELEM_KEYPAD_NUM, E_POP_KEYPAD_NUM,
    &m_sKeyPadNum, 65, 80, E_BUILTIN5X8, &sCfg);
  gslc_ElemXKeyPadValSetCb(&m_gui, m_pElemKeyPadNum, &CbKeypad);
//<InitGUI !End!>

//<Startup !Start!>
  gslc_GuiRotate(&m_gui, 3);
//<Startup !End!>

}

#endif // end _GUISLICE_GEN_H
