#include "Button2.h"
#include "communication.h"
#include "sdcard.h"
#include "sensors.h"
#include "tft_drawing.h"
#include <Arduino.h>
#include <SPI.h>

// #define INCLUDE_OTA // Uncomment to compile with over-the-air uploading
#define DEBUG_MODE // Uncomment for debug mode, disables GPB receive wait and compass init

#ifdef INCLUDE_OTA
#include <AsyncElegantOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#endif

#ifdef INCLUDE_OTA
// Web server for Over The Air uploading, DO NOT TOUCH OR YOU BREAK OTA //
const char *ssid = "SCUPA Wearable";
const char *password = "stokkink";
AsyncWebServer server(80);
#endif

// Button pins
#define UP_PIN 33
#define DOWN_PIN 25
#define LEFT_PIN 26
#define RIGHT_PIN 21

#define SEND_USER_LOCATION_INTERVAL 2000 // Send user location every 30 seconds 30000

DrawController screen;
DrawMap gps_map;
DrawMenu menu;
DrawBookmarks bookmarks;
DrawCheckMessages messages_check;
DrawSendMessage messages_send;
DrawSendEmergency messages_emergency_send;
SdCardController sd_controller;
CommHandler communication;
Sensors sensors;

GpsStorage gps_storage; // stores GPS coordinates that get drawn on the screen
MessageStorage msg_storage;

Button2 btn_up, btn_down, btn_left, btn_right;
bool btn_up_pressed{}, btn_down_pressed{}, btn_left_pressed{}, btn_right_pressed{}, btn_right_long_pressed{};
bool emergency_displayed{}; // Flag used to only display a single emergency message at once

int send_user_loc_current_time{}; // Used for refreshing the loop
int send_user_loc_previous_time{};

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
    } else if (btn == btn_down) {
        btn_down_pressed = true;
    } else if (btn == btn_left) {
        btn_left_pressed = true;
    } else {
        btn_right_pressed = true;
    }
}

void btn_longclick(Button2 &btn) {
    btn_right_long_pressed = true;
}

void setup() {

#ifdef INCLUDE_OTA
    //////////////// OTA Server setup /////////////////////
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    Serial.print("AP started: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Hi! Go to <this_ip>/update to upload files!");
    });

    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    server.begin();
    Serial.println("HTTP server started");
/////////^ DO NOT TOUCH OR YOU BREAK OTA ^///////////////
#endif

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
    // gps_storage.setUser(12.195722, -69.046859, 10);
    // gps_storage.addBookmark(12.194572, -69.047380, 12, "Sea horses", 2);
    // gps_storage.addBookmark(12.195009, -69.046143, 31, "Shipwreck", 3);
    // gps_storage.addBookmark(12.197472, -69.047321, 0, "Cool boat", 4);
    // gps_storage.addBookmark(12.196264, -69.051067, 0, "Beach", 5);
    // for (int i = 5; i < 32; i++) {
    //     gps_storage.addBookmark(10 + i, 10 + i, 0, "Filler", i);
    // }
    // gps_storage.addBookmark(69, 69, 0, "This was painful", 63);
    // /////// DEBUG //////////
    // msg_storage.addEntryNext("Cool message 1");
    // msg_storage.addEntryNext("Cool message 2");
    // msg_storage.addEntryNext("I am in excruciating pain without any indication if it will stop any time soon, please send help");
    // msg_storage.addEntryNext("Cool message 3");
    // msg_storage.addEntryNext("Cool last message");
    /////// DEBUG //////////

    // Read Current data from SD card
    sd_controller.readGpsArrayFromSD(gps_storage.arr);
    sd_controller.readGpsDescriptionsFromSD(gps_storage.descriptions);
    sd_controller.readMsgDescriptionsFromSD(msg_storage.message_descriptions);
    sd_controller.readMsgEmergencyDescriptionsFromSD(msg_storage.emergency_descriptions);

#ifndef DEBUG_MODE
    Serial.println("Initializing compass");
    sensors.initCompass();
    Serial.println("Compass initialized");
    screen.loading_screen();
    // RUN GPS HERE ONCE FOR STARTING LOCATION
    Serial.println("Waiting for GPB receive from buoy");
    while (!communication.GPB_received) {
        // Reading buffers waiting for GPB
        communication.readReceived();
        if (btn_right_pressed) {
            break;
        }
    }
    Serial.println("GPB received.");
#endif

    // Set this location as the starting location and save it
    gps_storage.addBookmark(gps_storage.returnUser().latitude, gps_storage.returnUser().longitude, gps_storage.returnUser().depth, "[Start Location]", 1);
    sd_controller.writeGpsArrayToSD(gps_storage.arr);

    Serial.println("Setup finished");
}

void loop() {
    // UPDATE BUTTONS
    btn_up.loop();
    btn_down.loop();
    btn_left.loop();
    btn_right.loop();

    // UPDATE SENSORS
    sensors.loopCompass();
    gps_map.compass_angle = sensors.compass_azimuth; // Update map compass angle with the last retrieved compass angle
    sensors.loopDepth();

    // WINDOW STATE MACHINE
    switch (current_state) {
    case main_menu:

        menu.loopMenu();

        if (btn_up_pressed) {
            menu.upMenu();
        }

        if (btn_down_pressed) {
            menu.downMenu();
        }

        if (btn_left_pressed) {
            // quick return to map
            current_state = map_display;
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
            // msg_storage.addEmergencyNext("EMERGENCY - VERY IMPORTANT EMERGENCY MESSAGE OH MY GOD"); // debug
        }

        break;

    case list_bookmarks:
        // item loop
        bookmarks.loopBookmarks();

        if (btn_up_pressed) {
            if (bookmarks.current_sub_state != bookmarks.Substate::warning_popup) // Don't allow scrolling up or down while in the deletion prompt
                bookmarks.upMenu();
        }

        if (btn_down_pressed) {
            if (bookmarks.current_sub_state != bookmarks.Substate::warning_popup) // Don't allow scrolling up or down while in the deletion prompt
                bookmarks.downMenu();
        }

        if (btn_left_pressed) {
            // Button press actions per sub state
            if (bookmarks.current_sub_state == bookmarks.Substate::list) {
                // return to main menu
                current_state = main_menu;
            } else if (bookmarks.current_sub_state == bookmarks.Substate::warning_popup) {
                // return to list
                bookmarks.current_sub_state = bookmarks.Substate::list;
                bookmarks.updateBookmarks(); // Force update bookmarks to make popup disappear instantly
            } else if (bookmarks.current_sub_state == bookmarks.Substate::info_popup) {
                // return to list
                bookmarks.current_sub_state = bookmarks.Substate::list;
                bookmarks.updateBookmarks(); // Force update bookmarks to make popup disappear instantly
            } else if (bookmarks.current_sub_state == bookmarks.Substate::add_bookmark) {
                // return to list
                bookmarks.current_sub_state = bookmarks.Substate::list;
                bookmarks.updateBookmarks(); // Force update bookmarks to make popup disappear instantly
            }
        }

        if (btn_right_pressed) {
            // Button press actions per sub state
            if (bookmarks.current_sub_state == bookmarks.Substate::list) {
                if (bookmarks.returnSelectedItem() == 0) {
                    gps_map.setCourse(0); // Remove current course
                } else {
                    if ((gps_storage.returnBookmark(bookmarks.returnSelectedItem()).latitude != 404) && (gps_storage.returnBookmark(bookmarks.returnSelectedItem()).longitude != 404)) {
                        // If right pressed on an existing entry, open info popup
                        bookmarks.current_sub_state = bookmarks.Substate::info_popup;
                        bookmarks.updateInfoPanel(); /// Force update to show instantly
                    } else {
                        // If right pressed on a non-existing entry, open add bookmark menu
                        bookmarks.current_sub_state = bookmarks.Substate::add_bookmark;
                        bookmarks.updateAddNewBookmarkMenu(); // Force update to show instantly
                    }
                }
            } else if (bookmarks.current_sub_state == bookmarks.Substate::info_popup) {
                // Set course to the selected item if confirmed in the info panel
                gps_map.setCourse(bookmarks.returnSelectedItem());

                // return to list
                bookmarks.current_sub_state = bookmarks.Substate::list;
                bookmarks.updateBookmarks(); // Force update bookmarks to make popup disappear instantly
            } else if (bookmarks.current_sub_state == bookmarks.Substate::add_bookmark) {
                bookmarks.bookmarkCurrentLocation();
                sd_controller.writeGpsArrayToSD(gps_storage.arr); // Save new location to SD card

                // return to list
                bookmarks.current_sub_state = bookmarks.Substate::list;
                bookmarks.updateBookmarks(); // Force update bookmarks to make popup disappear instantly
            }
        }

        if (btn_right_long_pressed) {
            // Button press actions per sub state
            if (bookmarks.current_sub_state == bookmarks.Substate::list) {
                // Show warning popup if in the list state
                if (bookmarks.returnSelectedItem() != 0) {
                    bookmarks.current_sub_state = bookmarks.Substate::warning_popup;
                    bookmarks.updateWarningPopUp(); // Force update to show instantly
                }
                // If long pressed again while in the warning popup state
            } else if (bookmarks.current_sub_state == bookmarks.Substate::warning_popup) {
                gps_storage.deleteBookmark(bookmarks.returnSelectedItem()); // Delete GPS if in the popup state
                sd_controller.writeGpsArrayToSD(gps_storage.arr);           // Save change to SD card

                bookmarks.current_sub_state = bookmarks.Substate::list;
                bookmarks.updateBookmarks(); // Force update bookmarks to make popup disappear instantly
            }
        }

        break;
    case check_messages:
        messages_check.loopCheckMessages();

        if (btn_up_pressed) {
            if (messages_check.current_sub_state != messages_check.Substate::warning_popup) // Don't allow scrolling up or down while in the deletion prompt
                messages_check.upMenu();
        }

        if (btn_down_pressed) {
            if (messages_check.current_sub_state != messages_check.Substate::warning_popup) // Don't allow scrolling up or down while in the deletion prompt
                messages_check.downMenu();
        }

        if (btn_left_pressed) {
            // Button press actions per sub state
            if (messages_check.current_sub_state == messages_check.Substate::list) {
                // return to main menu
                current_state = main_menu;
            } else if (messages_check.current_sub_state == messages_check.Substate::warning_popup) {
                // return to list
                messages_check.current_sub_state = messages_check.Substate::list;
                messages_check.updateCheckMessages(); // Force update to make popup disappear instantly
            } else if (messages_check.current_sub_state == messages_check.Substate::info_popup) {
                // return to list
                emergency_displayed = false; // set emergency displayed flag to false, in case the user was put into this state due to an emergency message arriving

                messages_check.current_sub_state = messages_check.Substate::list;
                messages_check.updateCheckMessages(); // Force update to make popup disappear instantly
            }
        }

        if (btn_right_pressed) {
            if (messages_check.current_sub_state != messages_check.Substate::warning_popup) {
                messages_check.current_sub_state = messages_check.Substate::info_popup;
                messages_check.updateInfoPanel(); /// Force update to show instantly
            }
        }

        if (btn_right_long_pressed) {
            // Button press actions per sub state
            if (messages_check.current_sub_state == messages_check.Substate::list) {
                // Show warning popup if in the list state
                messages_check.current_sub_state = messages_check.Substate::warning_popup;
                messages_check.updateWarningPopUp(); // Force update to show instantly

                // If long pressed again while in the warning popup state
            } else if (messages_check.current_sub_state == messages_check.Substate::warning_popup) {
                msg_storage.deleteEntry(messages_check.returnSelectedItem());     // Delete message if in the popup state
                msg_storage.reorganize();                                         // Reorganize list to avoid empty spot in the middle
                messages_check.current_sub_state = messages_check.Substate::list; // Return to list state
                messages_check.updateCheckMessages();                             // Force update to make popup disappear instantly
            }
        }

        break;
    case send_message:
        messages_send.loopSendMessage();

        if (btn_up_pressed) {
            messages_send.upMenu();
        }

        if (btn_down_pressed) {
            messages_send.downMenu();
        }

        if (btn_left_pressed) {
            // return to main menu
            current_state = main_menu;
        }

        if (btn_right_pressed) {
            // Send currently selected item
            communication.sendMSG(msg_storage.message_descriptions[messages_send.returnSelectedItem()]);
            // Return to main menu to indicate it has been sent
            current_state = main_menu;
        }
        break;
    case send_emergency:
        messages_emergency_send.loopSendEmergency();

        if (btn_up_pressed) {
            messages_emergency_send.upMenu();
        }

        if (btn_down_pressed) {
            messages_emergency_send.downMenu();
        }

        if (btn_left_pressed) {
            // return to main menu
            current_state = main_menu;
        }

        if (btn_right_pressed) {
            // Send currently selected item
            communication.sendEMR(msg_storage.emergency_descriptions[messages_emergency_send.returnSelectedItem()]);
            // Return to main menu to indicate it has been sent
            current_state = main_menu;
        }
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

    // Check for emergency messages and set to pop up if there is one
    if (!emergency_displayed) {
        if (msg_storage.returnEmergencySlot() != -1) {
            emergency_displayed = true;

            // Go to pop up state and pop up the message
            current_state = check_messages;
            messages_check.current_sub_state = messages_check.Substate::info_popup;
            messages_check.setSelectedItem(msg_storage.returnEmergencySlot());
            messages_check.updateInfoPanel(); /// Force update to show instantly
        }
    }

    // Send user location every SEND_USER_LOCATION_INTERVAL milliseconds
    send_user_loc_current_time = millis();
    if (send_user_loc_current_time - send_user_loc_previous_time >= SEND_USER_LOCATION_INTERVAL) {
        send_user_loc_previous_time = send_user_loc_current_time;

        communication.sendUserGPS();
    }

    // Check if anything has been received on the UART RX line
    if (communication.readReceived()) {
        sd_controller.writeGpsArrayToSD(gps_storage.arr);
    }
}