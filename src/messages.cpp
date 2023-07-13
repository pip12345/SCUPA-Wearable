#include "messages.h"

Message::Message() {
    this->emergency = false;
    this->text = "<<This message has no contents>>";
}

Message::Message(String text) {
    this->text = text;
    this->emergency = false;
}

Message::Message(String text, bool emergency) {
    this->text = text;
    this->emergency = emergency;
}

MessageEntry::MessageEntry() {
}

MessageEntry::MessageEntry(Message msg) {
    this->msg = msg;
}

// Add entry to the storage at the given slot
void MessageStorage::addEntry(Message msg, int slot) {
    if (slot < MESSAGE_STORAGE_SLOTS && slot >= 0) {
        arr[slot].msg = msg;
        arr[slot].empty = false; // Slot is taken, therefore empty set to false
    }
}

// Add entry to the storage at the given slot
void MessageStorage::addEntry(String text, int slot) {
    if (slot < MESSAGE_STORAGE_SLOTS && slot >= 0) {
        arr[slot].msg.text = text;
        arr[slot].empty = false; // Slot is taken, therefore empty set to false
    }
}

// Add entry to the storage at the given slot
void MessageStorage::addEntry(String text, bool emergency, int slot) {
    if (slot < MESSAGE_STORAGE_SLOTS && slot >= 0) {
        arr[slot].msg.text = text;
        arr[slot].msg.emergency = emergency;
        arr[slot].empty = false; // Slot is taken, therefore empty set to false
    }
}

// Add entry to the next free slot
void MessageStorage::addEntryNext(Message msg) {
    // Look through the storage array for the first free slot
    for (int i = 0; i < MESSAGE_STORAGE_SLOTS; i++) {
        if (arr[i].empty) {
            addEntry(msg, i);
            break;
        }

        // If no free spot available, replace last message
        if (i == (MESSAGE_STORAGE_SLOTS - 1) && (arr[i].empty == false)) {
            deleteEntry(i);
            addEntry(msg, i);
            break;
        }
    }
}

// Add entry to the next free slot
void MessageStorage::addEntryNext(String text) {
    // Look through the storage array for the first free slot
    for (int i = 0; i < MESSAGE_STORAGE_SLOTS; i++) {
        if (arr[i].empty) {
            addEntry(text, i);
            break;
        }

        // If no free spot available, replace last message
        if (i == (MESSAGE_STORAGE_SLOTS - 1) && (arr[i].empty == false)) {
            deleteEntry(i);
            addEntry(text, i);
            break;
        }
    }
}

// Add entry to the next free slot
void MessageStorage::addEntryNext(String text, bool emergency) {
    // Look through the storage array for the first free slot
    for (int i = 0; i < MESSAGE_STORAGE_SLOTS; i++) {
        if (arr[i].empty) {
            addEntry(text, emergency, i);
            break;
        }

        // If no free spot available, replace last message
        if (i == (MESSAGE_STORAGE_SLOTS - 1) && (arr[i].empty == false)) {
            deleteEntry(i);
            addEntry(text, emergency, i);
            break;
        }
    }
}

// Returns the slot of the first unread emergency message found, else returns -1
int MessageStorage::returnEmergencySlot() {
    for (int i = 0; i < MESSAGE_STORAGE_SLOTS; i++) {
        if (!returnIfEmpty(i) && !returnIfRead(i)) {
            if (returnEntry(i).emergency) {
                return i;
            }
        } else if (returnIfEmpty(i)) {
            // Break if it goes into the empty region
            return -1;
        }
    }

    return -1;
}

void MessageStorage::addEmergencyNext(String text) {
    addEntryNext(text, true); // add entry with emergency bool set to true;
}

void MessageStorage::deleteEntry(int slot) {
    arr[slot] = MessageEntry(); // Overwrite empty messageEntry to the slot, auto sets bool empty to true
}

void MessageStorage::deleteAll() {
    for (int i = 0; i < MESSAGE_STORAGE_SLOTS; i++) {
        deleteEntry(i);
    }
}

// Return contents of the given slot
Message MessageStorage::returnEntry(int slot) {
    return arr[slot].msg;
}

// Returns true if slot is empty
bool MessageStorage::returnIfEmpty(int slot) {
    return arr[slot].empty;
}

// Set the message of the slot to read
void MessageStorage::setRead(int slot) {
    arr[slot].read = true;
}

// Returns true if slot has been read
bool MessageStorage::returnIfRead(int slot) {
    return arr[slot].read;
}

// Returns true if there are any unread messages in the array
bool MessageStorage::returnAnyUnread() {
    for (int i = 0; i < MESSAGE_STORAGE_SLOTS; i++) {
        if (!returnIfEmpty(i)) {
            if (!returnIfRead(i)) {
                return true;
            }
        } else {
            // Break if it goes into the empty region
            return false;
        }
    }

    return false;
}

void MessageStorage::reorganize() {
    // Read 1 further than the empty spot, take that item and put it in the empty spot, stop if there's 2 empty spots in a row
    for (int i = 0; i < MESSAGE_STORAGE_SLOTS - 1; i++) {
        if (returnIfEmpty(i) && !returnIfEmpty(i + 1)) {
            // If the current spot is empty, and the next spot is not, then
            // add the entry past the empty spot in the empty spot
            // and delete the entry past the empty spot
            addMessageEntry(returnMessageEntry(i + 1), i);
            deleteEntry(i + 1);
        } else if (returnIfEmpty(i) && returnIfEmpty(i + 1)) {
            break; // Break out of loop if there's 2 empty spots in a row
        }
    }
}

// Return the full message entry of the message entry array, includes if the entry is empty or has been read
MessageEntry MessageStorage::returnMessageEntry(int slot) {
    return arr[slot];
}

void MessageStorage::addMessageEntry(MessageEntry entry, int slot) {
    arr[slot] = entry;
}