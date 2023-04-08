#include "seeed_line_chart.h" //include the library
#include"Free_Fonts.h" //include the header file
TFT_eSPI tft;
#define TFT_GREY 0x5AEB
float ltx = 0;    // Saved x coord of bottom of needle
uint16_t osx = 120, osy = 120; // Saved x & y coords
uint32_t updateTime = 0;       // time for next update
#define meter_pos_y 160
#define meter_pos_x 160
int old_analog =  -999; // Value last displayed

void plotNeedle(int value, byte ms_delay);

void analogMeter() {
    // Meter outline
    tft.setTextColor(TFT_BLACK);  // Text colour
    tft.setFreeFont(FF6); //select Free, Mono, Oblique, 12pt.
    // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
    for (int i = -50; i < 51; i += 5) {
        // Long scale tick length
        int tl = 15;

        // Coodinates of tick to draw
        float sx = cos((i - 90) * 0.0174532925);
        float sy = sin((i - 90) * 0.0174532925);
        uint16_t x0 = sx * (100 + tl) + meter_pos_y;
        uint16_t y0 = sy * (100 + tl) + meter_pos_x;
        uint16_t x1 = sx * 100 + meter_pos_y;
        uint16_t y1 = sy * 100 + meter_pos_x;

        // Coordinates of next tick for zone fill
        float sx2 = cos((i + 5 - 90) * 0.0174532925);
        float sy2 = sin((i + 5 - 90) * 0.0174532925);
        int x2 = sx2 * (100 + tl) + meter_pos_y;
        int y2 = sy2 * (100 + tl) + meter_pos_x;
        int x3 = sx2 * 100 + meter_pos_y;
        int y3 = sy2 * 100 + meter_pos_x;

        // Yellow zone limits
        if (i >= -50 && i < 0) {
          tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_GREEN);
          tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_GREEN);
        }

        // Green zone limits
        if (i >= 0 && i < 25) {
            tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
            tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
        }

        // Orange zone limits
        if (i >= 25 && i < 50) {
            tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_RED);
            tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_RED);
        }

        // Short scale tick length
        if (i % 25 != 0) {
            tl = 8;
        }

        // Recalculate coords incase tick lenght changed
        x0 = sx * (100 + tl) + meter_pos_y;
        y0 = sy * (100 + tl) + meter_pos_x;
        x1 = sx * 100 + meter_pos_y;
        y1 = sy * 100 + meter_pos_x;

        // Draw tick
        tft.drawLine(x0, y0, x1, y1, TFT_BLACK);

        // Check if labels should be drawn, with position tweaks
        if (i % 25 == 0) {
            // Calculate label positions
            x0 = sx * (100 + tl + 20) + meter_pos_y;
            y0 = sy * (100 + tl + 20) + meter_pos_x;
            switch (i / 25) {
                case -2: tft.drawCentreString("0", x0, y0 - 12, 2); break;
                case -1: tft.drawCentreString("25", x0, y0 - 9, 2); break;
                case 0: tft.drawCentreString("50", x0, y0 - 6, 2); break;
                case 1: tft.drawCentreString("75", x0, y0 - 9, 2); break;
                case 2: tft.drawCentreString("100", x0, y0 - 12, 2); break;
            }
        }

        // Now draw the arc of the scale
        sx = cos((i + 5 - 90) * 0.0174532925);
        sy = sin((i + 5 - 90) * 0.0174532925);
        x0 = sx * 100 + meter_pos_y;
        y0 = sy * 100 + meter_pos_x;
        // Draw scale arc, don't draw the last part
        if (i < 50) {
            tft.drawLine(x0, y0, x1, y1, TFT_BLACK);
        }
    }
    
    plotNeedle(0, 0); // Put meter needle at 0
}
  


// #########################################################################
// Update needle position
// This function is blocking while needle moves, time depends on ms_delay
// 10ms minimises needle flicker if text is drawn within needle sweep area
// Smaller values OK if text not in sweep area, zero for instant movement but
// does not look realistic... (note: 100 increments for full scale deflection)
// #########################################################################
void plotNeedle(int value, byte ms_delay) {
    if (value < -10) {
        value = -10;    // Limit value to emulate needle end stops
    }
    if (value > 110) {
        value = 110;
    }

    // Move the needle util new value reached
    while (!(value == old_analog)) {
        if (old_analog < value) {
            old_analog++;
        } else {
            old_analog--;
        }

        if (ms_delay == 0) {
            old_analog = value;    // Update immediately id delay is 0
        }

        float sdeg = map(old_analog, -10, 110, -150, -30); // Map value to angle
        // Calcualte tip of needle coords
        float sx = cos(sdeg * 0.0174532925);
        float sy = sin(sdeg * 0.0174532925);

        // Calculate x delta of needle start (does not start at pivot point)
        float tx = tan((sdeg + 90) * 0.0174532925);

        // Erase old needle image
        tft.drawLine(meter_pos_y + 20 * ltx - 1, meter_pos_x - 20, osx - 1, osy, TFT_WHITE);
        tft.drawLine(meter_pos_y + 20 * ltx, meter_pos_x - 20, osx, osy, TFT_WHITE);
        tft.drawLine(meter_pos_y + 20 * ltx + 1, meter_pos_x - 20, osx + 1, osy, TFT_WHITE);

        // Re-plot text under needle
        tft.setTextColor(TFT_BLACK);

        // Store new needle end coords for next erase
        ltx = tx;
        osx = sx * 98 + meter_pos_y;
        osy = sy * 98 + meter_pos_x;

        // Draw the needle in the new postion, magenta makes needle a bit bolder
        // draws 3 lines to thicken needle
        tft.drawLine(meter_pos_y + 20 * ltx - 1, meter_pos_x - 20, osx - 1, osy, TFT_RED);
        tft.drawLine(meter_pos_y + 20 * ltx, meter_pos_x - 20, osx, osy, TFT_MAGENTA);
        tft.drawLine(meter_pos_y + 20 * ltx + 1, meter_pos_x - 20, osx + 1, osy, TFT_RED);

        // Slow needle down slightly as it approaches new postion
        if (abs(old_analog - value) < 10) {
            ms_delay += ms_delay / 5;
        }
        // Wait before next update
        delay(ms_delay);
    }
}
