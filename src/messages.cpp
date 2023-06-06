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

MessageEntry::MessageEntry() {
}

MessageEntry::MessageEntry(Message msg) {
    this->msg = msg;
}

void MessageStorage::addEntry(Message msg, int slot) {
    arr[slot].msg = msg;
    arr[slot].empty = false; // Slot is taken, therefore empty set to false
}

void MessageStorage::addEntry(String text, int slot)
{
    arr[slot].msg.text = text;
    arr[slot].empty = false; // Slot is taken, therefore empty set to false
}

void MessageStorage::addEntry(String text, GpsCoordinates coords, int slot)
{
    arr[slot].msg.text = text;
    arr[slot].msg.coords = coords;
    arr[slot].empty = false; // Slot is taken, therefore empty set to false
}

void MessageStorage::addEntry(String text, bool emergency, int slot)
{
    arr[slot].msg.text = text;
    arr[slot].msg.emergency = emergency;
    arr[slot].empty = false; // Slot is taken, therefore empty set to false
}

void MessageStorage::addEntry(String text, GpsCoordinates coords, bool emergency, int slot)
{
    arr[slot].msg.text = text;
    arr[slot].msg.coords = coords;
    arr[slot].msg.emergency = emergency;
    arr[slot].empty = false; // Slot is taken, therefore empty set to false
}

void MessageStorage::deleteEntry(int slot) {
    arr[slot] = MessageEntry(); // Overwrite empty messageEntry to the slot, auto sets bool empty to true
}

Message MessageStorage::returnEntry(int slot)
{
    return arr[slot].msg;
}

bool MessageStorage::returnIfEmpty(int slot)
{
    return arr[slot].empty;
}
