#include "messages.h"

Message::Message() {
    this->emergency = false;
}

Message::Message(String text) {
    this->text = text;
    this->emergency = false;
}

Message::Message(String text, GpsCoordinates coords) {
    this->text = text;
    this->coords = coords;
    this->emergency = false;
}

Message::Message(String text, bool emergency) {
    this->text = text;
    this->emergency = emergency;
}

Message::Message(String text, GpsCoordinates coords, bool emergency) {
    this->text = text;
    this->coords = coords;
    this->emergency = emergency;
}

Message::Message(GpsCoordinates coords) {
    this->coords = coords;
}

MessageEntry::MessageEntry() {
}

MessageEntry::MessageEntry(Message msg) {
    this->msg = msg;
}

void MessageStorage::addEntry(Message msg, int slot) {
    if (slot < MESSAGE_STORAGE_SLOTS && slot >= 0) {
        arr[slot].msg = msg;
        arr[slot].empty = false; // Slot is taken, therefore empty set to false
    }
}

void MessageStorage::addEntry(String text, int slot) {
    if (slot < MESSAGE_STORAGE_SLOTS && slot >= 0) {
        arr[slot].msg.text = text;
        arr[slot].empty = false; // Slot is taken, therefore empty set to false
    }
}

void MessageStorage::addEntry(String text, GpsCoordinates coords, int slot) {
    if (slot < MESSAGE_STORAGE_SLOTS && slot >= 0) {
        arr[slot].msg.text = text;
        arr[slot].msg.coords = coords;
        arr[slot].empty = false; // Slot is taken, therefore empty set to false
    }
}

void MessageStorage::addEntry(String text, bool emergency, int slot) {
    if (slot < MESSAGE_STORAGE_SLOTS && slot >= 0) {
        arr[slot].msg.text = text;
        arr[slot].msg.emergency = emergency;
        arr[slot].empty = false; // Slot is taken, therefore empty set to false
    }
}

void MessageStorage::addEntry(String text, GpsCoordinates coords, bool emergency, int slot) {
    if (slot < MESSAGE_STORAGE_SLOTS && slot >= 0) {
        arr[slot].msg.text = text;
        arr[slot].msg.coords = coords;
        arr[slot].msg.emergency = emergency;
        arr[slot].empty = false; // Slot is taken, therefore empty set to false
    }
}

void MessageStorage::deleteEntry(int slot) {
    arr[slot] = MessageEntry(); // Overwrite empty messageEntry to the slot, auto sets bool empty to true
}

void MessageStorage::debugPrintContents() {
    for (int i = 0; i < MESSAGE_STORAGE_SLOTS; i++) {
        Serial.print("---------------- entry ");
        Serial.print(i);
        Serial.println(" ----------------");
        Serial.print("text: ");
        Serial.println(returnEntry(i).text);
        Serial.print("latitude: ");
        Serial.println(returnEntry(i).coords.latitude, 7);
        Serial.print("longitude: ");
        Serial.println(returnEntry(i).coords.longitude, 7);
        Serial.print("depth: ");
        Serial.println(returnEntry(i).coords.depth);
        Serial.print("gps description: ");
        Serial.println(returnEntry(i).coords.description);
        Serial.print("emergency: ");
        Serial.println(returnEntry(i).emergency);
        Serial.print("empty: ");
        Serial.println(returnIfEmpty(i));
    }
}

Message MessageStorage::returnEntry(int slot) {
    return arr[slot].msg;
}

bool MessageStorage::returnIfEmpty(int slot) {
    return arr[slot].empty;
}
