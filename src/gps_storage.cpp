#include "gps_storage.h"

GpsCoordinates::GpsCoordinates() {
    this->longitude = 404;
    this->latitude = 404;
    this->depth = -1;
    this->description = "None";
}

GpsCoordinates::GpsCoordinates(double latitude, double longitude, float depth, String description) {
    this->latitude = latitude;
    this->longitude = longitude;
    this->depth = depth;
    this->description = description;
}

GpsCoordinates::GpsCoordinates(double latitude, double longitude, float depth) {
    this->latitude = latitude;
    this->longitude = longitude;
    this->depth = depth;
}

GpsStorage::GpsStorage() {
    // Preprogrammed for now
    descriptions[0] = "Starting Location";
    descriptions[1] = "Boat";
    descriptions[2] = "Rendezvous Point";
    descriptions[3] = "Sea Horses";
    descriptions[4] = "Coral";
    descriptions[5] = "Shipwreck";
    descriptions[6] = "Rock Formation";
    descriptions[7] = "Cavern";
    descriptions[8] = "Interest 1";
    descriptions[9] = "Interest 2";
    descriptions[10] = "Interest 3";
    descriptions[11] = "Interest 4";
}

void GpsStorage::addBookmark(GpsCoordinates location, int slot) {
    if (slot >= 1 && slot < GPS_STORAGE_SLOTS) {
        arr[slot] = location;
    }
}

void GpsStorage::addBookmark(double latitude, double longitude, float depth, int slot) {
    if (slot >= 1 && slot < GPS_STORAGE_SLOTS) {
        arr[slot] = GpsCoordinates(latitude, longitude, depth);
    }
}

void GpsStorage::addBookmark(double latitude, double longitude, float depth, String description, int slot) {
    if (slot >= 1 && slot < GPS_STORAGE_SLOTS) {
        arr[slot] = GpsCoordinates(latitude, longitude, depth, description);
    }
}

void GpsStorage::deleteBookmark(int slot) {
    if (slot >= 1 && slot < GPS_STORAGE_SLOTS) {
        arr[slot] = GpsCoordinates(); // set value back to uninitialized values
    }
}

GpsCoordinates GpsStorage::returnBookmark(int slot) {
    if (slot >= 1 && slot < GPS_STORAGE_SLOTS) {
        return arr[slot];
    } else if (slot == 0) {
        return returnUser();
    } else {
        return GpsCoordinates(404, 404, -404, "ERROR OUTSIDE BOUNDS");
    }
}

void GpsStorage::setUser(GpsCoordinates location) {
    arr[0] = location;
    arr[0].description = "&USER";
}

void GpsStorage::setUser(double latitude, double longitude, float depth) {
    arr[0] = GpsCoordinates(latitude, longitude, depth);
    arr[0].description = "&USER";
}

GpsCoordinates GpsStorage::returnUser() {
    return arr[0];
}
