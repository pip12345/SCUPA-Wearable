#pragma once

#include "tft_drawing.h"
#include <FS.h>
#include <SD.h>
#include <SPI.h>

// SD card controller for tft project scupa

// Always called explicitly to avoid unnecessary or repeat SD writes/reads

/// PROTO
// #define SD_MISO 19
// #define SD_MOSI 23
// #define SD_SCK 18
// #define SD_CS 22

//// REAL
#define SD_MISO 19
#define SD_MOSI 23
#define SD_SCK 18
#define SD_CS 5


#define NR_FLOAT_DIGITS 10

class SdCardController {
  public:

    void writeGpsArrayToSD(GpsCoordinates *arr);
    void readGpsArrayFromSD(GpsCoordinates *arr);

    void readGpsDescriptionsFromSD(String *descriptions);
    void readMsgDescriptionsFromSD(String *message_descriptions);
    void readMsgEmergencyDescriptionsFromSD(String *emergency_descriptions);

    bool sd_init_failed{false}; // Indicates if SD card failed to get accessed
};