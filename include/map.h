#pragma once

// This file includes all non TFT drawing related code of the map

#include "gps_storage.h"
#include <Arduino.h>

struct ScreenCoordinates {
    int x{};
    int y{};

    ScreenCoordinates();
    ScreenCoordinates(int x, int y);
};

const int COMPASS_RADIUS = 30; // Radius of the compass being drawn

// Flat Earth Estimation: 111,111 meters (111.111 km) in the N direction is +1 degree (of latitude)
//                        111,111 * cos(latitude) meters in the W direction is +1 degree (of longitude).

const float METERS_PER_DEGREE = 1e7 / 90; // m per degree
const float RADIANS_PER_DEGREE = PI / 180;
const float DEGREES_PER_RADIAN = 180 / PI;

float getCompassSine(float angle);
float getCompassCosine(float angle);

float distGPSPoints(double lat1, double lon1, double lat2, double lon2);
float distGPStoUser(GpsStorage storage, int storage_id);
float angleGPSPoints(double lat1, double lon1, double lat2, double lon2);
float angleGPStoUser(GpsStorage storage, int storage_id);

float toScreenDistance(float gps_distance, float pixels_per_meter);
float toGPSDistance(float screen_distance, float pixels_per_meter);

ScreenCoordinates convertGPSToScreenCoords(GpsCoordinates user, GpsCoordinates waypoint, float pixels_per_meter);