#include "sensors.h"

void Sensors::readDepth() {
    // Logic for reading the sensor here
    depth_sensor.read();
    depth = depth_sensor.depth();
}

void Sensors::readCompass() {
    if (compass_connected) {
        int x, y, z;

        compass.read(&x, &y, &z);
        compass_azimuth = compass.azimuth(&y, &x);
        compass_azimuth += 90; // Compass is 90 degrees offset on the v2 PCB
    } else {
        // Debug program that rotates the compass in circles when there's no compass connected
        compass_azimuth += 10;
        if (compass_azimuth >= 360) {
            compass_azimuth = 0;
        }
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

void Sensors::initDepth()
{
    depth_sensor.init();
    depth_sensor.setModel(MS5837::MS5837_30BA);
}