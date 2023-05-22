#include "Button2.h"
#include "tft_drawing.h"
#include <Arduino.h>
#include <SPI.h>

// Button pins
#define UP_PIN 33
#define DOWN_PIN 25
#define LEFT_PIN 26
#define RIGHT_PIN 13

DrawController screen;
DrawMap gps_map;
DrawMenu menu;
DrawBookmarks bookmarks;
GpsStorage gps_storage; // stores GPS coordinates that get drawn on the screen

Button2 btn_up, btn_down, btn_left, btn_right;
bool btn_up_pressed{}, btn_down_pressed{}, btn_left_pressed{}, btn_right_pressed{}, btn_right_long_pressed{};

enum State { main_menu,
             map_display,
             list_bookmarks,
             check_messages,
             send_message,
             send_emergency };

int current_state = map_display;

void btn_pressed(Button2 &btn) {
    if (btn == btn_up) {
        btn_up_pressed = true;
        Serial.println("up pressed");
    } else if (btn == btn_down) {
        btn_down_pressed = true;
        Serial.println("down pressed");
    } else if (btn == btn_left) {
        btn_left_pressed = true;
        Serial.println("left pressed");
    } else {
        btn_right_pressed = true;
        Serial.println("right pressed");
    }
}

void btn_longclick(Button2 &btn) {
    btn_right_long_pressed = true;
    Serial.println("right longclick");
}

void setup() {
    Serial.begin(9600);

    btn_up.begin(UP_PIN);
    btn_up.setPressedHandler(btn_pressed);
    btn_down.begin(DOWN_PIN);
    btn_down.setPressedHandler(btn_pressed);
    btn_left.begin(LEFT_PIN);
    btn_left.setPressedHandler(btn_pressed);
    btn_right.begin(RIGHT_PIN);
    btn_right.setClickHandler(btn_pressed);
    btn_right.setLongClickHandler(btn_longclick);
    btn_right.setLongClickTime(500); // Longclick is 500 ms

    /////// DEBUG //////////
    // Distance between userlocation and seahorses is 139.85 m
    gps_storage.setUser(12.195722, -69.046859, 10);
    gps_storage.addBookmark(12.194572, -69.047380, 12, "Sea horses", 1);
    gps_storage.addBookmark(12.195009, -69.046143, 31, "Shipwreck", 2);
    gps_storage.addBookmark(12.197472, -69.047321, 0, "Cool boat", 3);
    gps_storage.addBookmark(12.196264, -69.051067, 0, "Beach", 4);
    for (int i = 5; i < 32; i++) {
        gps_storage.addBookmark(10 + i, 10 + i, 0, "Filler", i);
    }
    gps_storage.addBookmark(69, 69, 0, "This was painful", 63);
    /////// DEBUG //////////

    Serial.println("Setup finished");
}

void loop() {
    btn_up.loop();
    btn_down.loop();
    btn_left.loop();
    btn_right.loop();

    switch (current_state) {
    case main_menu:
        menu.loopMenu();

        if (btn_up_pressed) {
            menu.upMenu();
        }

        if (btn_down_pressed) {
            menu.downMenu();
        }

        if (btn_right_pressed) {
            // Convert selected_item to current_state
            current_state = menu.returnSelectedItem() + 1;
        }
        break;
    case map_display:
        gps_map.loopMap();

        if (btn_up_pressed) {
            gps_map.pixels_per_meter *= 1.2;
            gps_map.updateMap(); // Instantly update map
        }

        if (btn_down_pressed) {
            gps_map.pixels_per_meter /= 1.2;
            gps_map.updateMap();
        }

        if (btn_left_pressed) {
            // return to main menu
            current_state = main_menu;
        }

        if (btn_right_pressed) {
            if (gps_map.returnCourse() != 4) {
                gps_map.setCourse(4);
            } else {
                gps_map.setCourse(0);
            }
        }

        break;

    case list_bookmarks:
        // item loop
        bookmarks.loopBookmarks();

        if (btn_up_pressed) {
            bookmarks.upMenu();
        }

        if (btn_down_pressed) {
            bookmarks.downMenu();
        }

        if (btn_left_pressed) {
            // Button press actions per sub state
            if (bookmarks.current_sub_state == 0 /*list state*/) {
                // return to main menu
                current_state = main_menu;
                Serial.println("returned to menu");
            } else if (bookmarks.current_sub_state == 1 /*warning popup state*/) {
                // return to list
                bookmarks.current_sub_state = 0; /*list*/
                bookmarks.updateBookmarks();     // Force update bookmarks to make popup disappear instantly
                Serial.println("returned to list");
            }
        }

        if (btn_right_pressed) {
            // Button press actions per sub state
            if (bookmarks.current_sub_state == 0 /*list state*/) {
                if (bookmarks.returnSelectedItem() == 0) {
                    gps_map.setCourse(0); // Remove current course
                    Serial.println("Deleted course");
                } else {
                    // Go to show info panel state
                }
            }
        }

        if (btn_right_long_pressed) {
            // Button press actions per sub state
            if (bookmarks.current_sub_state == 0 /*list state*/) {

                if (bookmarks.returnSelectedItem() != 0) {
                    // Show warning popup if in the list state
                    bookmarks.current_sub_state = 1; /*warning popup state*/
                }

            } else if (bookmarks.current_sub_state == 1 /*warning popup state*/) {

                // Delete GPS if in the popup state
                gps_storage.deleteBookmark(bookmarks.returnSelectedItem());
                bookmarks.current_sub_state = 0; // Return to list substate
                bookmarks.updateBookmarks();     // Force update bookmarks to make popup disappear instantly
            }
        }

        break;
    case check_messages:
        // code here
        Serial.println("check messages selected");
        current_state = main_menu;
        break;
    case send_message:
        // code here
        Serial.println("send message selected");
        current_state = main_menu;
        break;
    case send_emergency:
        // code here
        Serial.println("emergency message selected");
        current_state = main_menu;
        break;

    default:
        current_state = map_display;
    }

    // Reset button flags to unpressed
    btn_up_pressed = false;
    btn_down_pressed = false;
    btn_left_pressed = false;
    btn_right_pressed = false;
    btn_right_long_pressed = false;
}