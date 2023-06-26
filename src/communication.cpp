#include "communication.h"

extern GpsStorage gps_storage;
extern MessageStorage msg_storage;
extern Sensors sensors;

CommHandler::CommHandler() {
    Serial.begin(baud);
    pinMode(RTS_PIN, OUTPUT);
    digitalWrite(RTS_PIN, LOW);
}

bool CommHandler::readReceived() {
    bool saveGPStoSD = false;

    if (Serial.available()) {
        // Only do anything if the received string starts with a '|'
        if (Serial.read() == '|') {
            type = Serial.readStringUntil('|');
            if (type == "GPS") {
                // |"GPS"|LATITUDE|LONGITUDE|DEPTH|TEXT DESCRIPTION|
                gps_lat = Serial.readStringUntil('|');
                gps_lon = Serial.readStringUntil('|');
                depth = Serial.readStringUntil('|');
                gps_text = Serial.readStringUntil('|');
                last_received = "|" + type + "|" + gps_lat + "|" + gps_lon + "|" + depth + "|" + gps_text + "|";

                gps_storage.addBookmarkNext(gps_lat.toFloat(), gps_lon.toFloat(), depth.toFloat(), gps_text);
                saveGPStoSD = true;

            } else if (type == "MSG") {
                // |"MSG"|MESSAGE CONTENTS|
                msg_text = Serial.readStringUntil('|');
                last_received = "|" + type + "|" + msg_text + "|";

                msg_storage.addEntryNext(msg_text);

            } else if (type == "EMR") {
                // |"EMR"|MESSAGE CONTENTS|
                emr_text = Serial.readStringUntil('|');
                last_received = "|" + type + "|" + emr_text + "|";

                msg_storage.addEmergencyNext(emr_text);

            } else if (type == "GPB") {
                // |"GPB"|LATITUDE|LONGITUDE|
                gpb_lat = Serial.readStringUntil('|');
                gpb_lon = Serial.readStringUntil('|');
                last_received = "|" + type + "|" + gpb_lat + "|" + gpb_lon + "|";

                gps_storage.setUser(gpb_lat.toFloat(), gpb_lon.toFloat(), sensors.depth);
                GPB_received = true; // Received first coordinate from GPB, GPB is active.
            }
        }

        Serial.begin(baud); // Flush buffers for new string
        // Serial.println(last_received);
    }

    return saveGPStoSD;
}

void CommHandler::sendBookmarkGPS(int slot) {

    GpsCoordinates bookmark = gps_storage.returnBookmark(slot);

    String latitude = String(bookmark.latitude, 6);
    String longitude = String(bookmark.longitude, 6);
    String depth = String(bookmark.depth, 2);

    // |"GPS"|LATITUDE|LONGITUDE|DEPTH|TEXT DESCRIPTION|
    String send_str = "|GPS|" + latitude + "|" + longitude + "|" + depth + "|" + bookmark.description + "|";

    digitalWrite(RTS_PIN, HIGH);
    delayMicroseconds(8); // Length of 1 byte
    Serial.println(send_str);
    delayMicroseconds(2300); // Length of 256 bytes
    digitalWrite(RTS_PIN, LOW);
}

void CommHandler::sendUserGPS() {

    GpsCoordinates user = gps_storage.returnUser();

    String latitude = String(user.latitude, 6);
    String longitude = String(user.longitude, 6);
    String depth = String(user.depth, 2);

    // |"GPS"|LATITUDE|LONGITUDE|DEPTH|TEXT DESCRIPTION|
    String send_str = "|GPS|" + latitude + "|" + longitude + "|" + depth + "|" + user.description + "|";

    digitalWrite(RTS_PIN, HIGH);
    delayMicroseconds(8);
    Serial.println(send_str);
    delayMicroseconds(2300);
    digitalWrite(RTS_PIN, LOW);
}

void CommHandler::sendMSG(String msg) {
    String send_str = "|MSG|" + msg + "|";

    digitalWrite(RTS_PIN, HIGH);
    delayMicroseconds(8);
    Serial.println(send_str);
    delayMicroseconds(2300);
    digitalWrite(RTS_PIN, LOW);
}

void CommHandler::sendEMR(String emr_msg) {
    String send_str = "|EMR|" + emr_msg + "|";

    digitalWrite(RTS_PIN, HIGH);
    delayMicroseconds(8);
    Serial.println(send_str);
    delayMicroseconds(2300);
    digitalWrite(RTS_PIN, LOW);
}