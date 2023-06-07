#include "tft_drawing.h"

extern GpsStorage gps_storage;     // Requires gps_storage to be declared in main
extern MessageStorage msg_storage; // Requires msg_storage to be declared in main

// Inits tft in this cpp file on creation of the drawcontroller, use this for all global stuff
// tft commands such as tft.drawPixel ONLY WORK inside this .cpp file
DrawController::DrawController() {
    tft.init(TFT_Y, TFT_X);
    tft.setSPISpeed(40000000);        // DANGER MIGHT BREAK STUFF
    tft.setRotation(1);               // Set screen in landscape mode
    tft.fillScreen(BACKGROUND_COLOR); // Wipe whole screen
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);
    Serial.print("Draw Controller Initialized");
}

// Clear all elements from screen
void DrawController::clearAll() {
    tft.fillScreen(BACKGROUND_COLOR);
}

void DrawController::resetTextToDefault() {
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);
}

// constructor
DrawMap::DrawMap() {
    pixels_per_meter = 0.4;
    compass_angle = 90;
}

// Update and redraw the map with the given storage data
void DrawMap::updateMap() {
    // Set text parameters to defaults
    tft.fillScreen(BACKGROUND_COLOR);
    tft.setTextSize(1);
    tft.setTextWrap(true);

    updateRingsRadiusText(RANGE_CIRCLE_RAD_CLOSE / pixels_per_meter, RANGE_CIRCLE_RAD_MEDIUM / pixels_per_meter);
    drawCoordinates();
    drawText();

    // TO DO: ADD FUNCTION HERE THAT GETS THE LATEST COMPASS ANGLE FROM MAGNETOMETER
    // TO DO: ADD FUNCTION THAT SETS THE COURSE
    drawCourse();                 // update course to the set course_id
    updateCompass(compass_angle); // Update compass
}

void DrawMap::setCourse(int course_storage_id) {
    if (course_storage_id >= 0 && course_storage_id < GPS_STORAGE_SLOTS)
        course_id = course_storage_id;
}

int DrawMap::returnCourse() {
    return course_id;
}

// Update and redraw map after a time of UPDATE_INTERVAL has passed
void DrawMap::loopMap() {
    current_time = millis();
    if (current_time - previous_time >= LOOP_UPDATE_INTERVAL) {
        previous_time = current_time;
        updateMap();
    }
}

// Redraw radius rings and update text of radius rings
void DrawMap::updateRingsRadiusText(int range_close, int range_medium) {
    // Convert range ints to const char arrays
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);

    // Draw circles again
    tft.drawCircle(TFT_CENTER_X, TFT_CENTER_Y, RANGE_CIRCLE_RAD_CLOSE, ST77XX_WHITE);
    tft.drawCircle(TFT_CENTER_X, TFT_CENTER_Y, RANGE_CIRCLE_RAD_MEDIUM, ST77XX_WHITE);

    // Draw meters radius text above range rings
    tft.setCursor(TFT_CENTER_X - 10, TFT_CENTER_Y + RANGE_CIRCLE_RAD_CLOSE + 8);
    tft.print(range_close);
    tft.print(" M");

    tft.setCursor(TFT_CENTER_X - 10, TFT_CENTER_Y + RANGE_CIRCLE_RAD_MEDIUM + 8);
    tft.print(range_medium);
    tft.print(" M");
}

// Update the compass direction and draw the compass
void DrawMap::updateCompass(float angle) {
    // TEMP: convert angle from compass angle (0 degrees = north) to unit circle angle (0 degrees = E)
    angle = 360 + (90 - angle);

    int x_offset{};
    int y_offset{}; // The resulting offsets from the center point

    // Normalize the angle to the range 0 to 359
    while (angle < 0)
        angle += 360;

    while (angle > 359)
        angle -= 360;

    x_offset = getCompassSine(angle);
    y_offset = getCompassCosine(angle);

    // Draw compass pointer
    tft.drawLine(TFT_CENTER_X, TFT_CENTER_Y, TFT_CENTER_X + x_offset, TFT_CENTER_Y + y_offset, ST77XX_RED);

    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);

    // Draw N E S W
    tft.setCursor(TFT_CENTER_X - 2, TFT_CENTER_Y - RANGE_CIRCLE_RAD_CLOSE + 10);
    tft.setTextColor(ST77XX_RED);
    tft.print("N");
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(TFT_CENTER_X - 2, TFT_CENTER_Y + RANGE_CIRCLE_RAD_CLOSE - 10);
    tft.print("S");
    tft.setCursor(TFT_CENTER_X + RANGE_CIRCLE_RAD_CLOSE - 10, TFT_CENTER_Y);
    tft.print("E");
    tft.setCursor(TFT_CENTER_X - RANGE_CIRCLE_RAD_CLOSE + 10, TFT_CENTER_Y);
    tft.print("W");

    // Draw center dot
    tft.fillCircle(TFT_CENTER_X, TFT_CENTER_Y, LOCATION_DOT_SIZE, ST77XX_RED);
}

// Updates course pointer to set id of a stored GPS coordinate
void DrawMap::drawCourse() {
    if (course_id > 0 && course_id < GPS_STORAGE_SLOTS) {                                                                      // If course ID is set and not outside bounds
        if (gps_storage.returnBookmark(course_id).longitude != 404 && gps_storage.returnBookmark(course_id).latitude != 404) { // If the coordinate exists
            ScreenCoordinates screen_coords = convertGPSToScreenCoords(gps_storage.returnUser(), gps_storage.returnBookmark(course_id), pixels_per_meter);
            // Draw course line
            tft.drawLine(TFT_CENTER_X, TFT_CENTER_Y, screen_coords.x, screen_coords.y, ST77XX_CYAN);

            // Draw colored dot over the selected one
            tft.fillCircle(screen_coords.x, screen_coords.y, LOCATION_DOT_SIZE + 1, ST77XX_MAGENTA);

            // Print course set to text
            tft.setTextWrap(true);
            tft.setTextSize(1);
            tft.setTextColor(ST77XX_ORANGE);
            tft.setCursor(0, 10);
            tft.print("Course to: ");
            tft.println(course_id);
            tft.print("Distance: ");
            tft.print(distGPStoUser(gps_storage, course_id));
            tft.print(" m");

            // Print depth info underneath selected dot
            tft.setTextColor(ST77XX_BLUE);
            tft.setCursor(screen_coords.x - 15, screen_coords.y + 5);
            tft.println(gps_storage.returnBookmark(course_id).depth);

            // Print distance info underneath selected dot
            tft.setTextColor(ST77XX_ORANGE);
            tft.setCursor(screen_coords.x - 15, screen_coords.y + 15);
            tft.println(distGPStoUser(gps_storage, course_id));
        } else {
            course_id = 0; // Reset course ID so no course will be drawn if it attempts to draw the coordinate to a non initialized GPS point
        }
    }
}

// Draw text in corners of screens
void DrawMap::drawText() {
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);

    // Draw GPS text
    tft.setCursor(0, 0);
    tft.print("GPS: ");
    tft.print(gps_storage.returnUser().latitude, GPS_DECIMALS);
    tft.print(" N");
    tft.print(", ");
    tft.print(gps_storage.returnUser().longitude, GPS_DECIMALS);
    tft.print(" E");

    // Draw current pixel scale
    tft.setCursor(TFT_X - 110, 0);
    tft.print("Scale: ");
    tft.print((float)(1 / pixels_per_meter));
    tft.print(" m/px");

    tft.setCursor(0, TFT_Y - 10);
    tft.print("Depth: ");
    tft.print(gps_storage.returnUser().depth);
    tft.print(" m");
}

// Draw all GPS coordinates stored in the GpsStorage object except the user itself
void DrawMap::drawCoordinates() {
    // For every element in the storage array except the first one (which is the user)
    if (gps_storage.returnUser().latitude != 404 && gps_storage.returnUser().longitude != 404) { // If the user exists
        for (int i = 1; i < GPS_STORAGE_SLOTS; i++) {
            // Check for every bookmark if they exist
            if ((gps_storage.returnBookmark(i).latitude != 404) && (gps_storage.returnBookmark(i).longitude != 404)) {
                // If so, then we may calculate and draw the screen coordinates
                ScreenCoordinates screen_coords = convertGPSToScreenCoords(gps_storage.returnUser(), gps_storage.returnBookmark(i), pixels_per_meter);

                // Draw screen coordinates on the screen if they're within the bounds of the screen
                if (screen_coords.x != -1 && screen_coords.y != -1) {
                    if ((screen_coords.x <= TFT_X + BORDER_TOLERANCE) && (screen_coords.x >= 0 - BORDER_TOLERANCE)) {
                        if ((screen_coords.y <= TFT_Y + BORDER_TOLERANCE) && (screen_coords.y >= 0 - BORDER_TOLERANCE)) {
                            // Print location dot
                            tft.fillCircle(screen_coords.x, screen_coords.y, LOCATION_DOT_SIZE, ST77XX_GREEN);

                            // Print id above dot
                            tft.setTextSize(1);
                            tft.setTextColor(ST77XX_GREEN);

                            tft.setCursor(screen_coords.x + 5, screen_coords.y - 10);
                            tft.println(i);

                            // Print depth info underneath selected dot if depth is set
                            if (gps_storage.returnBookmark(i).depth != -1) {
                                tft.setTextColor(ST77XX_BLUE);
                                tft.setCursor(screen_coords.x - 15, screen_coords.y + 5);
                                tft.println(gps_storage.returnBookmark(i).depth);
                            }
                        }
                    }
                } // if the screen coordinates exist
            }
        }
    }
}

// Update and redraw menu after a time of UPDATE_INTERVAL has passed
void DrawMenu::loopMenu() {
    current_time = millis();
    if (current_time - previous_time >= LOOP_UPDATE_INTERVAL) {
        previous_time = current_time;
        updateMenu();
    }
}

// Draw menu elements, needs to be called once before loopMenu()
void DrawMenu::updateMenu() {
    tft.fillScreen(BACKGROUND_COLOR); // Clear screen

    // Draw block for currently selected menu item
    tft.fillRoundRect(5, 18 - ITEM_BORDER_SIZE + (MENU_SPACING * selected_item), 280, 16 + (ITEM_BORDER_SIZE * 2), 5, ST77XX_BLUE);

    // Draw Text
    tft.setTextWrap(true);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(0, 20);
    tft.setTextSize(2); // 12*16
    tft.println(" Map");
    tft.setCursor(0, 20 + MENU_SPACING);
    tft.println(" Bookmarks");
    tft.setCursor(0, 20 + MENU_SPACING * 2);
    tft.println(" Check Messages");
    tft.setCursor(0, 20 + MENU_SPACING * 3);
    tft.println(" Send Message");
    tft.setCursor(0, 20 + MENU_SPACING * 4);
    tft.println(" Send Emergency Message");
    tft.setTextSize(1);
}

// Scroll one item up in the menu
void DrawMenu::upMenu() {
    if (selected_item > 0 && selected_item < MAX_MENU_ITEMS) {
        selected_item -= 1;
        updateMenu();
    }
}

// Scroll one item down in the menu
void DrawMenu::downMenu() {
    if (selected_item >= 0 && selected_item < (MAX_MENU_ITEMS - 1)) {
        selected_item += 1;
        updateMenu();
    }
}

int DrawMenu::returnSelectedItem() {
    return selected_item;
}

DrawBookmarks::DrawBookmarks() {
    selected_item = 1;
    current_sub_state = list; // starting state is list
}

// Update and redraw bookmarks after a time of UPDATE_INTERVAL has passed
void DrawBookmarks::loopBookmarks() {
    current_time = millis();
    if (current_time - previous_time >= LOOP_UPDATE_INTERVAL) {
        previous_time = current_time;

        // State machine for handling different subwindows within the bookmarks menu
        switch (current_sub_state) {
        case list:
            updateBookmarks();
            break;
        case warning_popup:
            updateWarningPopUp();
            break;
        case info_popup:
            updateInfoPanel();
            break;
        case add_bookmark:
            updateAddNewBookmarkMenu();
            Serial.print("selected_item: ");
            Serial.println(selected_item);
            Serial.print("selected_description: ");
            Serial.println(selected_description);
        default:
            break;
        }
    }
}

// Draw menu elements, list all bookmarks
void DrawBookmarks::updateBookmarks() {
    tft.fillScreen(BACKGROUND_COLOR); // Clear screen

    // Draw block for currently selected menu item
    // Always resets back to the first location when going to the next page
    tft.fillRoundRect(5, 18 - ITEM_BORDER_SIZE + ((MENU_SPACING * selected_item) - (current_page * MENU_SPACING * MAX_MENU_ITEMS)), 310, 16 + (ITEM_BORDER_SIZE * 2), 5, ST77XX_BLUE);

    // Draw Text
    tft.setTextWrap(false); // Disable text wrap because it may screw with the element positioning in the menu
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2); // 12*16

    for (int i = current_page * MAX_MENU_ITEMS; i <= (current_page * MAX_MENU_ITEMS) + MAX_MENU_ITEMS; i++) {
        if (i < GPS_STORAGE_SLOTS) {
            // Draw ID and description of each entry
            if (i == 0) {
                // Print Wipe currently set course instead of user for the first GPS coordinate
                tft.setCursor(0, 20 + MENU_SPACING * i);
                tft.setTextColor(ST77XX_ORANGE);
                tft.println("=-Remove current course-=");
                tft.setTextColor(ST77XX_WHITE);
            } else {
                // Draw the correct GPS storage spot based on the page
                tft.setCursor(0, 20 + ((MENU_SPACING * i) - (current_page * MENU_SPACING * MAX_MENU_ITEMS)));
                tft.print(" ID: ");
                tft.print(i);
                tft.print(" - ");
                tft.println(gps_storage.returnBookmark(i).description);
            }
        }
    }
    tft.setTextWrap(true);
    tft.setTextSize(1);
}

void DrawBookmarks::updateWarningPopUp() {
    // Don't clear screen on purpose, we want to still see what is in the background

    // Draw textbox in the center of the screen
    // Hardcoded because I'm lazy
    tft.fillRoundRect(POPUP_SIZE, POPUP_SIZE, TFT_X - 2 * POPUP_SIZE, TFT_Y - 2 * POPUP_SIZE, 10, ST77XX_RED);
    tft.setTextWrap(true);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setCursor(POPUP_SIZE + 20, POPUP_SIZE + 20);
    tft.println("Delete bookmark");
    tft.setTextSize(1);
    tft.setCursor(POPUP_SIZE + 20, POPUP_SIZE + 50);
    tft.println("Are you sure you wish to delete");
    tft.setCursor(POPUP_SIZE + 20, POPUP_SIZE + 60);
    tft.println("        this bookmark?");
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(POPUP_SIZE + 20, POPUP_SIZE + 100);
    tft.println("LONG PRESS RIGHT to confirm");
    tft.setCursor(POPUP_SIZE + 20, POPUP_SIZE + 120);
    tft.println("Press LEFT to cancel");
}

void DrawBookmarks::updateInfoPanel() {
    // Don't clear screen on purpose, we want to still see what is in the background

    // Draw textbox in the center of the screen
    // Hardcoded because I'm lazy
    tft.fillRoundRect(INFO_SIZE, INFO_SIZE, TFT_X - 2 * INFO_SIZE, TFT_Y - 2 * INFO_SIZE, 10, ST77XX_ORANGE);
    tft.setTextWrap(true);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE, ST77XX_ORANGE);
    tft.setCursor(INFO_SIZE + 20, INFO_SIZE + 10);
    tft.print("ID: ");
    tft.print(selected_item);

    tft.setCursor(INFO_SIZE + 20, INFO_SIZE + 30);
    tft.print("Latitude: ");
    tft.print(gps_storage.returnBookmark(selected_item).latitude, GPS_DECIMALS);
    tft.print(" N");

    tft.setCursor(INFO_SIZE + 20, INFO_SIZE + 40);
    tft.print("Longitude: ");
    tft.print(gps_storage.returnBookmark(selected_item).longitude, GPS_DECIMALS);
    tft.print(" E");

    tft.setCursor(INFO_SIZE + 20, INFO_SIZE + 50);
    tft.print("Depth: ");
    tft.print(gps_storage.returnBookmark(selected_item).depth);
    tft.print(" m");

    tft.setCursor(INFO_SIZE + 20, INFO_SIZE + 60);
    tft.print("Distance: ");
    tft.print(distGPStoUser(gps_storage, selected_item));
    tft.print(" m");

    tft.setCursor(INFO_SIZE + 20, INFO_SIZE + 80);
    tft.print("Description: ");
    tft.setCursor(INFO_SIZE + 20, INFO_SIZE + 90);
    tft.print(gps_storage.returnBookmark(selected_item).description);
    tft.setTextColor(ST77XX_WHITE);

    tft.setCursor(INFO_SIZE + 20, INFO_SIZE + 130);
    tft.print("Press RIGHT to set course ");
    tft.setCursor(INFO_SIZE + 20, INFO_SIZE + 140);
    tft.print("Press LEFT to close");

    tft.setTextColor(ST77XX_WHITE);
    // tft.print("Description: ");
    // tft.print(gps_storage.returnBookmark(selected_item).description);
}

// Draw menu to add bookmark to currently selected_item
void DrawBookmarks::updateAddNewBookmarkMenu() {
    // Show menu with preprogrammed strings
    tft.fillScreen(BACKGROUND_COLOR); // Clear screen

    // Draw block for currently selected menu item
    // Always resets back to the first location when going to the next page
    tft.fillRoundRect(5, 18 - ITEM_BORDER_SIZE + (MENU_SPACING * selected_description), 310, 16 + (ITEM_BORDER_SIZE * 2), 5, ST77XX_BLUE);

    // Draw Text
    tft.setTextWrap(false); // Disable text wrap because it may screw with the element positioning in the menu
    tft.setTextColor(ST77XX_ORANGE);
    tft.setTextSize(1);
    tft.setCursor(12, 0);
    tft.print("-=-=- New Bookmark: Select a description -=-=-");
    tft.setTextColor(ST77XX_WHITE);

    tft.setTextSize(2); // 12*16

    for (int i = 0; i <= MAX_MENU_ITEMS; i++) {
        // Draw the preprogrammed strings
        tft.setCursor(12, 20 + (MENU_SPACING * i));
        tft.print(gps_storage.descriptions[i]);
        // tft.print(" - ");
        // tft.println(gps_storage.returnBookmark(i).description);
    }

    tft.setTextSize(1); // 12*16
}

// Bookmark current position to the selected_item chosen in addNewBookmarkMenu() using selected_description
void DrawBookmarks::bookmarkCurrentLocation() {
    gps_storage.addBookmark(gps_storage.returnUser().latitude, gps_storage.returnUser().longitude, gps_storage.returnUser().depth, gps_storage.descriptions[selected_description], selected_item);
}

// Scroll one item up in the bookmark menu
void DrawBookmarks::upMenu() {
    if (current_sub_state == add_bookmark) {
        // In this case we're working with selected_description instead of selected_item
        // since we don't want to change the selected item in the main list while scrolling descriptions
        if (selected_description > 0 && selected_description < MAX_MENU_ITEMS) {
            selected_description -= 1;
            Serial.print("selected_description: ");
            Serial.println(selected_description);

            updateAddNewBookmarkMenu();
        }
    } else {
        if (selected_item > 0 && selected_item < GPS_STORAGE_SLOTS) {
            selected_item -= 1;

            /* We can calculate the current page by rounding down
            selected_item, then dividing it by 11.
            We do that because there are 11 unique elements per page since the last
            element always gets redrawn at the top of the next page. */

            current_page = (float)(int)(selected_item + 0.5) / 11; // use datatype rounding down trick
            Serial.print("selected_item: ");
            Serial.println(selected_item);
            Serial.print("current_page: ");
            Serial.println(current_page);

            updateBookmarks();
        }
    }
}

// Scroll one item down in the bookmark menu
void DrawBookmarks::downMenu() {
    if (current_sub_state == add_bookmark) {
        // In this case we're working with selected_description instead of selected_item
        // since we don't want to change the selected item in the main list while scrolling descriptions
        if (selected_description >= 0 && selected_description < (MAX_MENU_ITEMS - 1)) {
            selected_description += 1;
            Serial.print("selected_description: ");
            Serial.println(selected_description);

            updateAddNewBookmarkMenu();
        }
    } else {
        if (selected_item >= 0 && selected_item < (GPS_STORAGE_SLOTS - 1)) {
            selected_item += 1;

            /* We can calculate the current page by rounding down
            selected_item, then dividing it by 11.
            We do that because there are 11 unique elements per page since the last
            element always gets redrawn at the top of the next page. */

            current_page = (int)(selected_item + 0.5) / 11; // use datatype rounding down trick
            Serial.print("selected_item: ");
            Serial.println(selected_item);
            Serial.print("current_page: ");
            Serial.println(current_page);

            updateBookmarks();
        }
    }
}

int DrawBookmarks::returnSelectedItem() {
    return selected_item;
}

void DrawCheckMessages::loopCheckMessages() {
    current_time = millis();
    if (current_time - previous_time >= LOOP_UPDATE_INTERVAL) {
        previous_time = current_time;

        // State machine for handling different subwindows within the bookmarks menu
        switch (current_sub_state) {
        case list:
            updateCheckMessages();
            break;
        case warning_popup:
            // updateWarningPopUp();
            break;
        case info_popup:
            // updateInfoPanel();
            break;
        default:
            break;
        }
    }
}

void DrawCheckMessages::updateCheckMessages() {
    tft.fillScreen(BACKGROUND_COLOR); // Clear screen

    // Draw block for currently selected menu item
    // Always resets back to the first location when going to the next page
    tft.fillRoundRect(0, 18 - ITEM_BORDER_SIZE + ((MENU_SPACING * selected_item) - (current_page * MENU_SPACING * MAX_MENU_ITEMS)), TFT_X, 16 + (ITEM_BORDER_SIZE * 2), 5, ST77XX_BLUE);

    // Draw Text
    tft.setTextWrap(false); // Disable text wrap because it may screw with the element positioning in the menu
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2); // 12*16

    for (int i = current_page * MAX_MENU_ITEMS; i <= (current_page * MAX_MENU_ITEMS) + MAX_MENU_ITEMS; i++) {
        if (i < GPS_STORAGE_SLOTS) {
            // Draw the message for each slot
            tft.setCursor(0, 20 + ((MENU_SPACING * i) - (current_page * MENU_SPACING * MAX_MENU_ITEMS)));
            tft.print(i + 1);
            tft.print(" - ");
            tft.println(msg_storage.returnEntry(i).text);
        }
    }
    tft.setTextWrap(true);
    tft.setTextSize(1);
}

void DrawCheckMessages::updateWarningPopUp() {
    // Don't clear screen on purpose, we want to still see what is in the background

    // Draw textbox in the center of the screen
    // Hardcoded because I'm lazy
    tft.fillRoundRect(POPUP_SIZE, POPUP_SIZE, TFT_X - 2 * POPUP_SIZE, TFT_Y - 2 * POPUP_SIZE, 10, ST77XX_RED);
    tft.setTextWrap(true);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setCursor(POPUP_SIZE + 20, POPUP_SIZE + 20);
    tft.println("Delete Message");
    tft.setTextSize(1);
    tft.setCursor(POPUP_SIZE + 20, POPUP_SIZE + 50);
    tft.println("Are you sure you wish to delete");
    tft.setCursor(POPUP_SIZE + 20, POPUP_SIZE + 60);
    tft.println("        this message?");
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(POPUP_SIZE + 20, POPUP_SIZE + 100);
    tft.println("LONG PRESS RIGHT to confirm");
    tft.setCursor(POPUP_SIZE + 20, POPUP_SIZE + 120);
    tft.println("Press LEFT to cancel");
}

void DrawCheckMessages::updateInfoPanel() {
    tft.fillScreen(BACKGROUND_COLOR); // Clear screen
    tft.setTextWrap(true);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);

    tft.setCursor(0,50);
    tft.println(msg_storage.returnEntry(selected_item).text);
    tft.setTextSize(1);

    // Print info
    tft.setCursor(0,0);
    tft.setTextColor(ST77XX_ORANGE);
    tft.println("SHOWING MESSAGE INFO");
    tft.setTextColor(ST77XX_WHITE);
}

void DrawCheckMessages::upMenu() {
    if (selected_item > 0 && selected_item < GPS_STORAGE_SLOTS) {
        selected_item -= 1;

        /* We can calculate the current page by rounding down
        selected_item, then dividing it by 11.
        We do that because there are 11 unique elements per page since the last
        element always gets redrawn at the top of the next page. */

        current_page = (float)(int)(selected_item + 0.5) / 11; // use datatype rounding down trick
        Serial.print("selected_item: ");
        Serial.println(selected_item);
        Serial.print("current_page: ");
        Serial.println(current_page);

        updateCheckMessages();
    }
}

void DrawCheckMessages::downMenu() {
    if (selected_item >= 0 && selected_item < (GPS_STORAGE_SLOTS - 1)) {
        selected_item += 1;

        /* We can calculate the current page by rounding down
        selected_item, then dividing it by 11.
        We do that because there are 11 unique elements per page since the last
        element always gets redrawn at the top of the next page. */

        current_page = (int)(selected_item + 0.5) / 11; // use datatype rounding down trick
        Serial.print("selected_item: ");
        Serial.println(selected_item);
        Serial.print("current_page: ");
        Serial.println(current_page);

        updateCheckMessages();
    }
}

int DrawCheckMessages::returnSelectedItem()
{
    return selected_item;
}

/////////////////////////////////////////////// TO DO /////////////////////////////////////////////////////////
void DrawSendMessage::loopSendMessage() {
    current_time = millis();
    if (current_time - previous_time >= LOOP_UPDATE_INTERVAL) {
        previous_time = current_time;
        updateSendMessage();
    }
}

void DrawSendMessage::updateSendMessage() {
    tft.fillScreen(BACKGROUND_COLOR); // Clear screen

    // Draw block for currently selected menu item
    // Always resets back to the first location when going to the next page
    tft.fillRoundRect(5, 18 - ITEM_BORDER_SIZE + (MENU_SPACING * selected_item), 310, 16 + (ITEM_BORDER_SIZE * 2), 5, ST77XX_BLUE);

    // Draw Text
    tft.setTextWrap(false); // Disable text wrap because it may screw with the element positioning in the menu
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2); // 12*16

    for (int i = 0; i <= MAX_MENU_ITEMS; i++) {
        if (i < GPS_STORAGE_SLOTS) {
            tft.setCursor(0, 20 + (MENU_SPACING * i));
            tft.print("SEND ");
            tft.print(" - ");
            tft.println(msg_storage.message_descriptions[i]);
        }
    }
    tft.setTextWrap(true);
    tft.setTextSize(1);
}

void DrawSendEmergency::loopSendEmergency() {
    current_time = millis();
    if (current_time - previous_time >= LOOP_UPDATE_INTERVAL) {
        previous_time = current_time;
        updateSendEmergency();
    }
}

void DrawSendEmergency::updateSendEmergency() {
}