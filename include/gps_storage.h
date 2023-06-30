#pragma once

// All GPS storage related code

#include <Arduino.h>
#define GPS_STORAGE_SLOTS 64
#define GPS_DESCRIPTION_STRINGS 12

struct GpsCoordinates {
    float latitude{};     // Latitude example: 12.195722 (N) - y axis, 404 is uninitialized
    float longitude{};    // Longitude example: -69.046859 (W) - x axis, 404 is uninitialized
    float depth{};        // depth in meters, stored as a positive number, -1 means no depth stored
    String description{}; /// Description that goes along with the coordinates

    GpsCoordinates();
    GpsCoordinates(double latitude, double longitude, float depth, String description);
    GpsCoordinates(double latitude, double longitude, float depth);

    // sending:   latitude,longitude,depth,description
    //            12.194572,-69.047380,12,Sea horses
    //            float,float,int,string
};

// Stores GPS coordinates in an array with a specific id, id 0 is reserved for the user itself. Bookmark ids go from 1 to GPS_STORAGE_SLOTS (default 64)
class GpsStorage {
  public:
    GpsStorage();
    void addBookmark(GpsCoordinates location, int slot); // bookmark location to slot 1 to GPS_STORAGE_SLOTS (default 64)
    void addBookmark(double latitude, double longitude, float depth, String description, int slot);
    void addBookmark(double latitude, double longitude, float depth, int slot);
    void addBookmarkNext(double latitude, double longitude, float depth, String description);
    void deleteBookmark(int slot);           // delete bookmark location in passed slot
    GpsCoordinates returnBookmark(int slot); // return GPS GpsCoordinates of slot

    void setUser(GpsCoordinates location);                        // Set user coordinates
    void setUser(double latitude, double longitude, float depth); // Set user coordinates
    GpsCoordinates returnUser();                                  // Return user coordinates

    String descriptions[GPS_DESCRIPTION_STRINGS]{}; // Holds preprogrammed descriptions. Stretch goal: read this from SD card instead?

    GpsCoordinates arr[GPS_STORAGE_SLOTS]{}; // Holds bookmarked GPS coordinates
};