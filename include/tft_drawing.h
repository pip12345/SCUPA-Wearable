#pragma once

// Handles all tft drawing related code

/*============ READ THIS ============
The tft library only works in a SINGLE cpp file because ????
Therefore, all calls to tft must be done in the source file of this header
============== READ THIS ============*/

// RGB565 COLOR PICKER http://www.barth-dev.de/online/rgb565-color-picker/

#include "Adafruit_GFX.h"    // Core graphics library http://adafruit.github.io/Adafruit-GFX-Library/html/class_adafruit___g_f_x.html https://learn.adafruit.com/adafruit-gfx-graphics-library/overview
#include "Adafruit_ST7789.h" // Hardware-specific library for ST7789 https://docs.circuitpython.org/projects/st7789/en/latest/index.html
#include <Arduino.h>

#include "gps_storage.h"
#include "map.h"

#define TFT_MOSI 23 // DIN on waveshare
#define TFT_SCK 18  // CLK on waveshare
#define TFT_CS 16
#define TFT_RST 17
#define TFT_DC 32
#define TFT_BL 27 // Backlight, any pin, just needs to be on

// Coordinates go from X: 0 - 239 and Y: 0 - 319
#define TFT_Y 240
#define TFT_X 320
#define TFT_CENTER_Y 119
#define TFT_CENTER_X 159

#define BACKGROUND_COLOR 0x0082
#define LOCATION_DOT_SIZE 2

// tolerance around screen borders before elements aren't drawn anymore (example: allows rendering of text beneath an off-screen dot)
#define BORDER_TOLERANCE 50

static Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

class DrawController {
  public:
    DrawController();
    void clearAll();
    void resetTextToDefault();
};

class DrawMap {
  public:
    DrawMap();
    void loopMap();   // runs updateMap() every set UPDATE_INTERVAL milliseconds
    void updateMap(); // Update and redraw the map with the given storage data

    void updateCourseTo(int storage_id);
    void updateCompass(float angle);

    float pixels_per_meter{}; // Pixels per meter scale for scaling the map, used for calculating on-screen distances
    float compass_angle{};
    int course_id{};

  private:
    void drawText();
    void updateRingsRadiusText(int range_close, int range_medium);
    void drawCoordinates();

    const char RANGE_CIRCLE_RAD_CLOSE = 50; // char if value is less than 127
    const char RANGE_CIRCLE_RAD_MEDIUM = 100;

    int current_time{}; // Used for time-based actions
    int previous_time{};
    const int LOOP_UPDATE_INTERVAL = 2000; // update interval in ms
};

class DrawMenu {
  public:
    void loopMenu(); // runs updateMenu() every set UPDATE_INTERVAL millsieconds
    void updateMenu();
    void upMenu();
    void downMenu();

    int selected_item{};

  private:
    const int MENU_SPACING = 35;  // spacing between each item in the menu
    const int MAX_MENU_ITEMS = 5; // 6 items in menu
    const int ITEM_BORDER_SIZE = 5;

    int current_time{}; // Used for time-based actions
    int previous_time{};
    const int LOOP_UPDATE_INTERVAL = 2000; // update interval in ms
};

class DrawBookmarks {
  public:
    DrawBookmarks();

    void loopBookmarks();
    void updateBookmarks();
    void upMenu();
    void downMenu();

    void displayInfo();
    void warningPopUp();

    int selected_item{};
    int current_page{}; // current page of MAX_MENU_ITEMS bookmarks being displayed on the menu, first page = 0

  private:
    const int MENU_SPACING = 18;
    const int MAX_MENU_ITEMS = 11; // show x+1 items at once (11 is 12 being shown)
    const int ITEM_BORDER_SIZE = 2;

    int current_time{}; // Used for time-based actions
    int previous_time{};
    const int LOOP_UPDATE_INTERVAL = 2000; // update interval in ms
};
