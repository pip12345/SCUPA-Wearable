#pragma once

#include <MechaQMC5883.h>
#include <Wire.h>


class Sensors {
  public:
    void initCompass();

    void readDepth();
    void readCompass();

    void loopDepth();
    void loopCompass();

    float depth{-1};           // Last measured depth
    int compass_azimuth{}; // Last measured compass azimuth

    MechaQMC5883 compass;

    bool compass_connected = false; // used for debugging

  private:

  
    int current_time_depth{}; // Used for refreshing the loop
    int previous_time_depth{};
    const int LOOP_UPDATE_INTERVAL_DEPTH = 2000; // update interval in ms

    int current_time_compass{}; // Used for refreshing the loop
    int previous_time_compass{};
    const int LOOP_UPDATE_INTERVAL_COMPASS = 2000; // update interval in ms
};