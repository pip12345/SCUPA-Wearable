#pragma once

#include "tft_drawing.h"
#include <FS.h>
#include <SD.h>
#include <SPI.h>

// SD card controller for tft project scupa

// Always called explicitly to avoid unnecessary or repeat SD writes/reads

#define SD_MISO 19
#define SD_MOSI 23
#define SD_SCK 18
#define SD_CS 22

#define NR_FLOAT_DIGITS 10

class SdCardController {
  public:
    // GPS Storage Arr
    // Write or read contents of the arr
    // Functions that call a writeGpsArrayToSD:
    // gps_storage.AddBookmark()
    // gps_storage.DeleteBookmark()

    void writeGpsArrayToSD(GpsCoordinates *arr);
    void readGpsArrayFromSD(GpsCoordinates *arr);

    void readGpsDescriptionsFromSD(String *descriptions);
    void readMsgDescriptionsFromSD(String *message_descriptions);
    void readMsgEmergencyDescriptionsFromSD(String *emergency_descriptions);

    /* Below is scrapped for now due to the dirty SPI workaround */

    // MessageStorage Arr
    // Write or read contents of the arr
    // Functions that call a writeMsgArrayToSD:
    // msg_storage.addEntryNext()
    // msg_storage.addEmergencyNext()
    // msg_storage.deleteEntry()
    // msg_storage.deleteAll()

    // void writeMsgArrayToSD(MessageEntry *arr);
    // void readMsgArrayFromSD(MessageEntry *arr);

    bool sd_init_failed{false}; // Indicates if SD card failed to get accessed
};