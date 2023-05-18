#include "map.h"
#include "tft_drawing.h"

// Returns compass sine for defined COMPASS_RADIUS
float getCompassSine(float angle) {
    return sin(angle * 2 * PI / 360) * COMPASS_RADIUS + 0.5;
}

// Returns compass cosine for defined COMPASS_RADIUS
// Cosine(angle) == Sine(angle-90)
float getCompassCosine(float angle) {
    // Keep angle-90 between 0 and 359
    if (angle < 90)
        angle += 360;

    return getCompassSine(angle - 90);
}

// Only works from values 0 to 9999, temp must be a char array of at least 5 elements
// Puts the resulting characters in the char array passed to the function
void intToCharArray(int value, char *temp) {
    if (value < 100 && value >= 0) {
        dtostrf(value, 2, 0, temp);
    } else if (value >= 100 && value < 1000) {
        dtostrf(value, 3, 0, temp);
    } else if (value >= 1000) {
        dtostrf(value, 4, 0, temp);
    }
}

// Returns distance in meters between 2 GPS points
float distGPSPoints(double lat1, double lon1, double lat2, double lon2) {
    double lat_avg = (lat1 + lat2) / 2;
    double coslat = cos(lat_avg * RADIANS_PER_DEGREE); // 1.000 @ 0° ,  0.500 @ 60°

    double dlat = lat1 - lat2;            // dy
    double dlon = (lon2 - lon1) * coslat; // dx

    return sqrt(dlat * dlat + dlon * dlon) * METERS_PER_DEGREE;
}

// Returns distance between the user and a bookmark saved in the storage under a numeric id
float distGPStoUser(GpsStorage storage, int id) {
    return distGPSPoints(storage.returnUser().latitude, storage.returnUser().longitude, storage.returnBookmark(id).latitude, storage.returnBookmark(id).longitude);
}

// Returns angle between 2 GPS points in degrees
float angleGPSPoints(double lat_current, double lon_current, double lat_waypoint, double lon_waypoint) {
    float angle;

    double dy = (lat_waypoint - lat_current) * METERS_PER_DEGREE;                                         // dy (sin)
    double dx = (lon_waypoint - lon_current) * cos(lat_current * RADIANS_PER_DEGREE) * METERS_PER_DEGREE; // dx (cos)

    if (dy > 0 && dx > 0) {
        // Unit circle quadrant 1
        // sin and cos positive
        angle = atan2(dy, dx) * DEGREES_PER_RADIAN; // atan(dy/dx) = atan(dlat/dlon)
    } else if ((dy > 0 && dx < 0)) {
        // Unit circle quadrant 2
        // sin positive cos negative, or both negative
        angle = atan2(dy, dx) * DEGREES_PER_RADIAN;
    } else if ((dy < 0 && dx > 0) || (dy < 0 && dx < 0)) {
        // Unit circle quadrant 3 & 4
        // sin negative and cos positive
        angle = atan2(dy, dx) * DEGREES_PER_RADIAN + 360;
    } else if (dy == 0 && dx > 0) {
        // sin zero, cos positive
        angle = 0;
    } else if (dy == 0 && dx < 0) {
        // sin zero, cos negative
        angle = 180;
    } else if (dy > 0 && dx == 0) {
        // cos zero, sin positive
        angle = 90;
    } else if (dy < 0 && dx == 0) {
        // cos zero, sin negative
        angle = 270;
    } else {
        angle = 0.404; // 404 angle not found ;)
    }

    return angle;
}

float angleGPStoUser(GpsStorage storage, int storage_id) {
    return angleGPSPoints(storage.returnUser().latitude, storage.returnUser().longitude, storage.returnBookmark(storage_id).latitude, storage.returnBookmark(storage_id).longitude);
}

// converts GPS distance (in meters) to a distance in pixels on the screen
float toScreenDistance(float gps_distance, float pixels_per_meter) {
    return gps_distance * pixels_per_meter;
}

// converts Screen distance to a distance in meters
float toGPSDistance(float screen_distance, float pixels_per_meter) {
    return screen_distance / pixels_per_meter;
}

// Calculate the screen coordinates of the waypoint relative to the coordinates of the user
ScreenCoordinates convertGPSToScreenCoords(GpsCoordinates user, GpsCoordinates waypoint, float pixels_per_meter) {
    if (user.longitude == -1 || user.latitude == -1 || waypoint.longitude == -1 || waypoint.latitude == -1) { // Catch for empty coordinate
        return ScreenCoordinates(-1, -1);
    } else {
        ScreenCoordinates waypoint_screen_coords;

        float distance_gps = distGPSPoints(user.latitude, user.longitude, waypoint.latitude, waypoint.longitude);
        float distance_screen = toScreenDistance(distance_gps, pixels_per_meter);

        float angle = angleGPSPoints(user.latitude, user.longitude, waypoint.latitude, waypoint.longitude) * RADIANS_PER_DEGREE; // angle in radians

        // Distance and angle are polar coordinate system, calculate dx and dy in pixels.
        // x = r * cos(theta)
        // y = r * sin(theta)

        float dx = distance_screen * cos(angle);
        float dy = distance_screen * sin(angle);

        waypoint_screen_coords.x = TFT_CENTER_X + dx;
        waypoint_screen_coords.y = TFT_CENTER_Y - dy; // Subtract because y in screen coordinate system

        return waypoint_screen_coords;
    }
}
ScreenCoordinates::ScreenCoordinates() {
    x = -1;
    y = -1;
}

ScreenCoordinates::ScreenCoordinates(int x, int y) {
    this->x = x;
    this->y = y;
}
