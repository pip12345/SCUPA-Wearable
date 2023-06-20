#pragma once

#include "gps_storage.h"
#include "messages.h"
#include "sensors.h"
#include <Arduino.h>

#define baud 9600

// Handles receiving and sending of serial messages to buoy

// RX default buffer size is 256 bytes!

/****************** Communication frame format: *******************

Notes:
    - All frames are sent in pure ASCII, even numbers
    - MAX SIZE OF ANY FRAME IS 256 BYTES (limited by ESP UART rx buffer size)

-------------------------------------------------------------
    GPS coordinates to/from base station:

    |"GPS"|LATITUDE|LONGITUDE|DEPTH|TEXT DESCRIPTION|
-------------------------------------------------------------
    GPS coordinates sent from buoy to wearable:

    |"GPB"|LATITUDE|LONGITUDE|
-------------------------------------------------------------
    Text messages:

    |"MSG"|MESSAGE CONTENTS|
-------------------------------------------------------------
    Emergency messages:

    |"EMR"|MESSAGE CONTENTS|

*******************************************************************/

class CommHandler {
  public:
    CommHandler();
    bool readReceived(); // Reads and processes uart messages, outputs true if the SD card needs to save the GPS coordinates again

    void sendUserGPS();
    void sendMSG(String msg);
    void sendEMR(String emr_msg);

    String last_received{}; // Last received string

  private:
    // Holds last read values
    String type{};

    String gps_lat{};
    String gps_lon{};
    String depth{};
    String gps_text{};

    String gpb_lat{};
    String gpb_lon{};

    String msg_text{};
    String emr_text{};
};