#pragma once

#include "gps_storage.h"

// All non-drawing text message related code

#define MESSAGE_STORAGE_SLOTS 64
#define MESSAGE_DESCRIPTION_SLOTS 12
#define EMERGENCY_DESCRIPTION_SLOTS 12

// Single instance of a message with text and an emergency flag
struct Message {
    String text{};
    bool emergency{false};

    Message();
    Message(String text);
    Message(String text, bool emergency);
};

// Message entry in the storage, includes extra information such as if the spot is empty or if the message has been read
struct MessageEntry {
    Message msg{};
    bool empty{true}; // used to indicate if a spot in the array is fully empty
    bool read{false}; // used to indicate if a message has been read or not

    MessageEntry();
    MessageEntry(Message msg);
};

// Stores (text) messages in an array with a specific id
// Starts at slot 0
class MessageStorage {
  public:
    void addEntryNext(String text);
    void addEmergencyNext(String text);

    void deleteEntry(int slot);
    void deleteAll();

    void setRead(int slot);

    Message returnEntry(int slot);
    bool returnIfEmpty(int slot);
    bool returnIfRead(int slot);

    int returnEmergencySlot();
    bool returnAnyUnread();

    void reorganize(); // Reorganize all messages to remove blank spaces

    String message_descriptions[MESSAGE_DESCRIPTION_SLOTS]{};     // Holds preprogrammed descriptions for sending messages
    String emergency_descriptions[EMERGENCY_DESCRIPTION_SLOTS]{}; // Holds preprogrammed descriptions for sending emergency messages

    MessageEntry arr[MESSAGE_STORAGE_SLOTS]{}; // Array of MessageEntry's that holds all saved messages

  private:
    void addEntry(Message msg, int slot);
    void addEntry(String text, int slot);
    void addEntry(String text, bool emergency, int slot);

    void addEntryNext(Message msg); // Unused
    void addEntryNext(String text, bool emergency);

    MessageEntry returnMessageEntry(int slot);
    void addMessageEntry(MessageEntry entry, int slot);
};