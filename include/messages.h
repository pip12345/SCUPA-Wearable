#pragma once

#include "gps_storage.h"

// All non-drawing message related code

#define MESSAGE_STORAGE_SLOTS 64
#define MESSAGE_DESCRIPTION_SLOTS 12
#define EMERGENCY_DESCRIPTION_SLOTS 12

// Single instance of a message with text, coordinates and an emergency flag
struct Message {
    String text{};
    GpsCoordinates coords{};
    bool emergency{};

    Message();
    Message(String text);
    Message(GpsCoordinates coords);
    Message(String text, GpsCoordinates coords);
    Message(String text, bool emergency);
    Message(String text, GpsCoordinates coords, bool emergency);
};

// Message entry in the storage, indicates if the entry is empty or not
struct MessageEntry {
    Message msg{};
    bool empty{true};

    MessageEntry();
    MessageEntry(Message msg);
};

// Stores messages in an array with a specific id
// Starts at slot 0
class MessageStorage {
  public:
    MessageStorage();
    void addEntry(Message msg, int slot);
    void addEntry(String text, int slot);
    void addEntry(String text, GpsCoordinates coords, int slot);
    void addEntry(String text, bool emergency, int slot);
    void addEntry(String text, GpsCoordinates coords, bool emergency, int slot);
    void deleteEntry(int slot);

    Message returnEntry(int slot);
    bool returnIfEmpty(int slot);

    void debugPrintContents(); // print contents of entire array;

    String message_descriptions[MESSAGE_DESCRIPTION_SLOTS]{};
    String emergency_descriptions[EMERGENCY_DESCRIPTION_SLOTS]{};
    
  private:
    MessageEntry arr[MESSAGE_STORAGE_SLOTS]{}; // Array of MessageEntry's that holds all saved messages
};