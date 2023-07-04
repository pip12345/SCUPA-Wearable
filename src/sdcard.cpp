#include "sdcard.h"

extern DrawController screen;

void SdCardController::writeGpsArrayToSD(GpsCoordinates *arr) {

    /*
    This is nasty code, a short explanation of this crime:

        The TFT library tries to hijack the SPI and run it at a higher frequency, therefore we don't init the SD card until
         we need to use it, as it will also take control of the SPI hardware.
        After having used the SD card, the SD card is closed and then the TFT init runs again to restart the SPI for the TFT.
        This will unfortunately turn off the screen for about a second after every read/write.
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
        for (int i = 0; i < GPS_STORAGE_SLOTS; i++) {
            // Format:
            //      LATITUDE|LONGITUDE|DEPTH|DESCRIPTION/
            file.print(arr[i].latitude, NR_FLOAT_DIGITS);
            file.print("|");
            file.print(arr[i].longitude, NR_FLOAT_DIGITS);
            file.print("|");
            file.print(arr[i].depth, NR_FLOAT_DIGITS);
            file.print("|");
            file.print(arr[i].description);
            file.println("|");
        }

        file.close();
    }

    SD.end();
    screen.init(); // i hate this
}

void SdCardController::readGpsArrayFromSD(GpsCoordinates *arr) {

    /*
    This is nasty code, a short explanation of this crime:

        The TFT library tries to hijack the SPI and run it at a higher frequency, therefore we don't init the SD card until
         we need to use it, as it will also take control of the SPI hardware.
        After having used the SD card, the SD card is closed and then the TFT init runs again to restart the SPI for the TFT.
        This will unfortunately turn off the screen for about a second after every read/write.
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
        for (int i = 0; i < GPS_STORAGE_SLOTS; i++) {
            // Format:
            //      LATITUDE|LONGITUDE|DEPTH|DESCRIPTION/
            arr[i].latitude = file.readStringUntil('|').toFloat();
            arr[i].longitude = file.readStringUntil('|').toFloat();
            arr[i].depth = file.readStringUntil('|').toFloat();
            arr[i].description = file.readStringUntil('|');
            arr[i].description.trim(); // Trim newline from the string
        }

        file.close();
    }

    ///// END CODE /////

    SD.end();
    screen.init(); // i hate this
}

void SdCardController::readGpsDescriptionsFromSD(String *descriptions) {

    /*
    This is nasty code, a short explanation of this crime:

        The TFT library tries to hijack the SPI and run it at a higher frequency, therefore we don't init the SD card until
         we need to use it, as it will also take control of the SPI hardware.
        After having used the SD card, the SD card is closed and then the TFT init runs again to restart the SPI for the TFT.
        This will unfortunately turn off the screen for about a second after every read/write.
    */

    if (!SD.begin(SD_CS)) {
        sd_init_failed = true;
    } else {
        sd_init_failed = false;
    }

    ///// CODE INBETWEEN HERE /////

    File file = SD.open("/bookmark_descriptions.txt", FILE_READ);
    if (!file) {
        sd_init_failed = true;
    }

    if (!sd_init_failed) {
        for (int i = 0; i < GPS_DESCRIPTION_STRINGS; i++) {
            // Format:
            //      DESCRIPTION/
            descriptions[i] = file.readStringUntil('|');
            descriptions[i].trim(); // Trim newline from the string
        }

        file.close();
    }

    ///// END CODE /////

    SD.end();
    screen.init(); // i hate this
}

void SdCardController::readMsgDescriptionsFromSD(String *message_descriptions) {

    /*
    This is nasty code, a short explanation of this crime:

        The TFT library tries to hijack the SPI and run it at a higher frequency, therefore we don't init the SD card until
         we need to use it, as it will also take control of the SPI hardware.
        After having used the SD card, the SD card is closed and then the TFT init runs again to restart the SPI for the TFT.
        This will unfortunately turn off the screen for about a second after every read/write.
    */

    if (!SD.begin(SD_CS)) {
        sd_init_failed = true;
    } else {
        sd_init_failed = false;
    }

    ///// CODE INBETWEEN HERE /////

    File file = SD.open("/message_descriptions.txt", FILE_READ);
    if (!file) {
        sd_init_failed = true;
    }

    if (!sd_init_failed) {
        for (int i = 0; i < MESSAGE_DESCRIPTION_SLOTS; i++) {
            // Format:
            //      DESCRIPTION/
            message_descriptions[i] = file.readStringUntil('|');
            message_descriptions[i].trim(); // Trim newline from the string
        }

        file.close();
    }

    ///// END CODE /////

    SD.end();
    screen.init(); // i hate this
}

void SdCardController::readMsgEmergencyDescriptionsFromSD(String *emergency_descriptions) {

    /*
    This is nasty code, a short explanation of this crime:

        The TFT library tries to hijack the SPI and run it at a higher frequency, therefore we don't init the SD card until
         we need to use it, as it will also take control of the SPI hardware.
        After having used the SD card, the SD card is closed and then the TFT init runs again to restart the SPI for the TFT.
        This will unfortunately turn off the screen for about a second after every read/write.
    */

    if (!SD.begin(SD_CS)) {
        sd_init_failed = true;
    } else {
        sd_init_failed = false;
    }

    ///// CODE INBETWEEN HERE /////

    File file = SD.open("/emergency_message_descriptions.txt", FILE_READ);
    if (!file) {
        sd_init_failed = true;
    }

    if (!sd_init_failed) {
        for (int i = 0; i < EMERGENCY_DESCRIPTION_SLOTS; i++) {
            // Format:
            //      DESCRIPTION/
            emergency_descriptions[i] = file.readStringUntil('|');
            emergency_descriptions[i].trim(); // Trim newline from the string
        }

        file.close();
    }

    ///// END CODE /////

    SD.end();
    screen.init(); // i hate this
}