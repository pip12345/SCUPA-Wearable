#include "tft_drawing.h"

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
    course_id = 0;
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
    updateCourseTo(course_id);    // set course
    updateCompass(compass_angle); // Update compass
}

// Update and redraw map after a time of UPDATE_INTERVAL has passed
void DrawMap::loopMap() {
    current_time = millis();
    if (current_time - previous_time >= UPDATE_INTERVAL) {
        previous_time = current_time;
        updateMap();
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
    tft.print(storage.returnUser().latitude, 6);
    tft.print(", ");
    tft.print(storage.returnUser().longitude, 6);

    // Draw current pixel scale
    tft.setCursor(TFT_X - 110, 0);
    tft.print("Scale: ");
    tft.print((float)(1 / pixels_per_meter));
    tft.print(" m/px");

    tft.setCursor(0, TFT_Y - 10);
    tft.print("Depth: ");
    tft.print(storage.returnUser().depth);
    tft.print(" m");
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

    // tft.fillCircle(TFT_CENTER_X, TFT_CENTER_Y, COMPASS_RADIUS + 1, BACKGROUND_COLOR); // Wipe area

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
// Set to 0 to remove set course
void DrawMap::updateCourseTo(int storage_id) {
    if (storage_id != 0) {
        ScreenCoordinates screen_coords = convertGPSToScreenCoords(storage.returnUser(), storage.returnBookmark(storage_id), pixels_per_meter);
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
        tft.print(distGPStoUser(storage, course_id));

        // Print depth info underneath selected dot
        tft.setTextColor(ST77XX_BLUE);
        tft.setCursor(screen_coords.x - 15, screen_coords.y + 5);

        char depth_text[6] = {};
        tft.println(storage.returnBookmark(storage_id).depth);

        // Print distance info underneath selected dot
        tft.setTextColor(ST77XX_ORANGE);
        tft.setCursor(screen_coords.x - 15, screen_coords.y + 15);
        tft.println(distGPStoUser(storage, storage_id));
    }
}

// Draw all GPS coordinates stored in the GpsStorage object
void DrawMap::drawCoordinates() {
    // For every element in the storage array
    if (storage.returnUser().latitude != 404 && storage.returnUser().longitude != 404) { // If the user exists
        for (int i = 0; i < GPS_STORAGE_SLOTS; i++) {
            // Check for every bookmark if they exist
            if ((storage.returnBookmark(i).latitude != 404) && (storage.returnBookmark(i).longitude != 404)) {
                // If so, then we may calculate and draw the screen coordinates
                ScreenCoordinates screen_coords = convertGPSToScreenCoords(storage.returnUser(), storage.returnBookmark(i), pixels_per_meter);

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
                            if (storage.returnBookmark(i).depth != -1) {
                                tft.setTextColor(ST77XX_BLUE);
                                tft.setCursor(screen_coords.x - 15, screen_coords.y + 5);
                                tft.println(storage.returnBookmark(i).depth);
                            }
                        }
                    }
                } // if the screen coordinates exist
            }
        }
    }
}

DrawMenu::DrawMenu() {
    selected_item = 0;
}

// Draw menu elements, needs to be called once before loopMenu()
void DrawMenu::showMenu() {
    tft.fillScreen(BACKGROUND_COLOR); // Clear screen

    // Draw block for currently selected menu item
    tft.fillRoundRect(5, 18 - 5 + (MENU_SPACING * selected_item), 280, 16 + (5 * 2), 5, ST77XX_BLUE);

    // Draw Text
    tft.setTextWrap(true);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(0, 20);
    tft.setTextSize(2); // 12*16
    tft.println(" Map");
    tft.setCursor(0, 20 + MENU_SPACING);
    tft.println(" List Bookmarks");
    tft.setCursor(0, 20 + MENU_SPACING * 2);
    tft.println(" Save Bookmark");
    tft.setCursor(0, 20 + MENU_SPACING * 3);
    tft.println(" Check Messages");
    tft.setCursor(0, 20 + MENU_SPACING * 4);
    tft.println(" Send Message");
    tft.setCursor(0, 20 + MENU_SPACING * 5);
    tft.println(" Send Emergency Message");
    tft.setTextSize(1);
}

// Scroll one item up in the menu
void DrawMenu::upMenu() {
    if (selected_item > 0 && selected_item < MAX_ITEMS) {
        selected_item -= 1;
        showMenu();
    }
}

// Scroll one item down in the menu
void DrawMenu::downMenu() {
    if (selected_item >= 0 && selected_item < (MAX_ITEMS - 1)) {
        selected_item += 1;
        showMenu();
    }
}
