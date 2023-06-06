#pragma once

// Handles all tft drawing related code

/*============ READ THIS ============
The tft library only works in a SINGLE cpp file because ????
Therefore, all calls to tft must be done in the source file of this header
============== READ THIS ============*/

/******************************************----- CLASS STRUCTURE ------******************************************
 *
 *   Each drawing class has the following general structure:
 *
 *   The loop gets called in the main program loop and will run one or more update functions every set interval of milliseconds.
 *
 *     - void loopX()
 *
 *
 *   Update functions will retrieve new data (update) and then draw GUI elements. These get called either
 *   in other update functions or in loopX().
 *
 *     - void updateX()
 *
 *
 *   Optionally, classes where multiple windows appear will have sub states to switch between these windows.
 *   In this case the loopX() function will contain a separate state machine to switch between these substates
 *
 *     - enum Substate {state1, state2, state3...};
 *     - void updateState2()
 *     - void updateState3()
 *
 *****************************************************************************************************************/

// RGB565 COLOR PICKER http://www.barth-dev.de/online/rgb565-color-picker/
#include "Adafruit_GFX.h"    // Core graphics library https://learn.adafruit.com/adafruit-gfx-graphics-library/overview
#include "Adafruit_ST7789.h" // Hardware-specific library for ST7789 https://docs.circuitpython.org/projects/st7789/en/latest/index.html
#include <Arduino.h>

#include "gps_storage.h"
#include "map.h"
#include "messages.h"

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
#define GPS_DECIMALS 6 // nr of decimals when GPS gets printed

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

    void setCourse(int course_id); // set course to given id, 0 = no course set
    int returnCourse();
    void updateCompass(float angle);

    float pixels_per_meter{}; // Pixels per meter scale for scaling the map, used for calculating on-screen distances
    float compass_angle{};

  private:
    void drawCourse();
    void drawText();
    void updateRingsRadiusText(int range_close, int range_medium);
    void drawCoordinates();
    int course_id{}; // holds currently set course and gets used by drawCourse when drawing the course line

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

    int returnSelectedItem();

  private:
    const int MENU_SPACING = 35;  // spacing between each item in the menu
    const int MAX_MENU_ITEMS = 5; // 6 items in menu
    const int ITEM_BORDER_SIZE = 5;

    int selected_item{}; // Currently selected menu item in the menu

    int current_time{}; // Used for time-based actions
    int previous_time{};
    const int LOOP_UPDATE_INTERVAL = 2000; // update interval in ms
};

class DrawBookmarks {
  public:
    DrawBookmarks();

    void loopBookmarks(); // runs updateBookmarks() every set UPDATE_INTERVAL millsieconds

    void updateBookmarks();
    void updateWarningPopUp();
    void updateInfoPanel();
    void updateAddNewBookmarkMenu();

    void bookmarkCurrentLocation();

    void upMenu();
    void downMenu();

    int returnSelectedItem();

    enum Substate { list,           // Main bookmark list
                    warning_popup,  // Deletion confirm warning prompt
                    info_popup,     // Info popup showing detailed information
                    add_bookmark }; // Add new bookmark menu -> select description

    Substate current_sub_state{};

  private:
    const int MENU_SPACING = 18;
    const int MAX_MENU_ITEMS = 11; // show x+1 items at once (11 is 12 being shown)
    const int ITEM_BORDER_SIZE = 2;
    const int POPUP_SIZE = 50;
    const int INFO_SIZE = 40;

    int selected_item{};        // Currently selected menu item in the menu
    int selected_description{}; // Currently selected description item in the add new bookmark menu
    int current_page{};         // current page of MAX_MENU_ITEMS bookmarks being displayed on the menu, first page = 0

    // State machine within loopBookmarks() handles subwindows
    int current_time{}; // Used for time-based actions
    int previous_time{};
    const int LOOP_UPDATE_INTERVAL = 2000; // update interval in ms
};
