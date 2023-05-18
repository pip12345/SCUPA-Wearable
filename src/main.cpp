#include "Button2.h"
#include "tft_drawing.h"
#include <Arduino.h>
#include <SPI.h>

#define UP_PIN 33
#define DOWN_PIN 25
#define LEFT_PIN 26
#define RIGHT_PIN 13

DrawController screen;
DrawMap gps_map;
Button2 btn_up, btn_down, btn_left, btn_right;
bool btn_up_pressed{}, btn_down_pressed{}, btn_left_pressed{}, btn_right_pressed{};

enum State { map_display,
             main_menu,
             messages,
             bookmarks };

State current_state = map_display;

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

void setup() {
    Serial.begin(9600);

    btn_up.begin(UP_PIN);
    btn_up.setPressedHandler(btn_pressed);
    btn_down.begin(DOWN_PIN);
    btn_down.setPressedHandler(btn_pressed);
    btn_left.begin(LEFT_PIN);
    btn_left.setPressedHandler(btn_pressed);
    btn_right.begin(RIGHT_PIN);
    btn_right.setPressedHandler(btn_pressed);

    /////// DEBUG //////////
    // Distance between userlocation and seahorses is 139.85 m
    gps_map.storage.setUser(12.195722, -69.046859, 10);
    gps_map.storage.addBookmark(12.194572, -69.047380, 12, "Sea horses", 1);
    gps_map.storage.addBookmark(12.195009, -69.046143, 31, "Shipwreck", 2);
    gps_map.storage.addBookmark(12.197472, -69.047321, 0, "Cool boat", 3);
    gps_map.storage.addBookmark(12.196264, -69.051067, 0, "Beach", 4);
    /////// DEBUG //////////

    Serial.println("Setup finished");
}

void loop() {
    btn_up.loop();
    btn_down.loop();
    btn_left.loop();
    btn_right.loop();

    switch (current_state) {
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

        break;
    case main_menu:
        // code here n shit
        break;

    case messages:
        // code here n shit
        break;

    case bookmarks:
        // code here n shit
        break;

    default:
        current_state = map_display;
    }

    // Reset button flags to unpressed
    btn_up_pressed = false;
    btn_down_pressed = false;
    btn_left_pressed = false;
    btn_right_pressed = false;
}