#include "sdcard.h"

extern DrawController screen;

void SdCardController::writeGpsArrayToSD(GpsCoordinates *arr) {

    /* 
    This is the nastiest code ever written, a short explanation of this crime:
    
        The TFT library tries to hijack the SPI and run it at a higher frequency, therefore we don't init the SD card until 
         we need it as it will also take control of the SPI hardware.
        After having used the SD card, the SD card is closed and then the TFT init runs again to restart the SPI for the TFT.
        This will turn off the screen for about a second after every read/write.
    */

    if (!SD.begin(SD_CS)) {
        sd_init_failed = true;
    } else {
        sd_init_failed = false;
    }

    /// CODE INBETWEEN HERE

    File file = SD.open("/gps_storage.txt", FILE_WRITE);
    if (!file) {
        sd_init_failed = true;
    }

    if (!sd_init_failed) {
        for (int i = 0; i < GPS_STORAGE_SLOTS - 1; i++) {
            // Format:
            //      LATITUDE|LONGITUDE|DEPTH|DESCRIPTION/
            file.print(arr[i].latitude, NR_FLOAT_DIGITS);
            file.print("|");
            file.print(arr[i].longitude, NR_FLOAT_DIGITS);
            file.print("|");
            file.print(arr[i].depth, NR_FLOAT_DIGITS);
            file.print("|");
            file.print(arr[i].description);
            file.println("/"); // EOL TERMINATOR
        }

        file.close();

        Serial.println("Written GPS stuff to SD card");
    }

    SD.end();
    screen.init();
}

void SdCardController::readGpsArrayFromSD(GpsCoordinates *arr) {

    /* 
    This is the nastiest code ever written, a short explanation of this crime:
        The TFT library tries to hijack the SPI and run it at a higher frequency, therefore we don't init the SD card until 
        we need it as it will also take control of the SPI hardware.
        After done reading/writing, the SD card is closed and then the TFT init runs again to restart the SPI for the TFT
    */

    if (!SD.begin(SD_CS)) {
        sd_init_failed = true;
    } else {
        sd_init_failed = false;
    }

    ///// CODE INBETWEEN HERE /////

    File file = SD.open("/gps_storage.txt", FILE_READ);
    if (!file) {
        sd_init_failed = true;
    }

    if (!sd_init_failed) {
        for (int i = 0; i < GPS_STORAGE_SLOTS - 1; i++) {
            // Format:
            //      LATITUDE|LONGITUDE|DEPTH|DESCRIPTION/
            arr[i].latitude = file.readStringUntil('|').toFloat();
            arr[i].longitude = file.readStringUntil('|').toFloat();
            arr[i].depth = file.readStringUntil('|').toFloat();
            arr[i].description = file.readStringUntil('/');
        }

        file.close();

        Serial.println("Read GPS stuff from SD card");
    }

    ///// END CODE /////

    SD.end();
    screen.init();
}
