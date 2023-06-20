#include "communication.h"

extern GpsStorage gps_storage;
extern MessageStorage msg_storage;
extern Sensors sensors;

CommHandler::CommHandler() {
    Serial.begin(9600);
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
            }
        }
        Serial.begin(baud); // Flush buffers for new string
        // Serial.println(last_received);
    }

    return saveGPStoSD;
}

void CommHandler::sendUserGPS() {

    GpsCoordinates user = gps_storage.returnUser();

    String latitude = String(user.latitude, 6);
    String longitude = String(user.longitude, 6);
    String depth = String(user.depth, 2);

    // |"GPS"|LATITUDE|LONGITUDE|DEPTH|TEXT DESCRIPTION|
    String send_str = "|GPS|" + latitude + "|" + longitude + "|" + depth + "|" + user.description + "|";

    Serial.println(send_str);
}

void CommHandler::sendMSG(String msg) {
    String send_str = "|MSG|" + msg + "|";

    Serial.println(send_str);
}

void CommHandler::sendEMR(String emr_msg) {
    String send_str = "|EMR|" + emr_msg + "|";

    Serial.println(send_str);
}