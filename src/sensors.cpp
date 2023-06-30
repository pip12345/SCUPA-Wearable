#include "sensors.h"

int Sensors::readDepth() {
    return -1;
}

int Sensors::readCompass() {
    if (compass_connected) {
        compass_azimuth = compass.getAzimuth();
        return compass_azimuth;
    } else {
        // Debug program that rotates the compass in circles when there's no compass connected
        compass_azimuth += 10;
        if (compass_azimuth >= 360) {
            compass_azimuth = 0;
        }
        return compass_azimuth;
    }
}

void Sensors::loopDepth() {
    current_time_depth = millis();
    if (current_time_depth - previous_time_depth >= LOOP_UPDATE_INTERVAL_DEPTH) {
        previous_time_depth = current_time_depth;

        readDepth();
    }
}

void Sensors::loopCompass() {
    current_time_compass = millis();
    if (current_time_compass - previous_time_compass >= LOOP_UPDATE_INTERVAL_COMPASS) {
        previous_time_compass = current_time_compass;

        readCompass();
    }
}

void Sensors::initCompass() {
    compass_connected = true;
    compass.init();
}