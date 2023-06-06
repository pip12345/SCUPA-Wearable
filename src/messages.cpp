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

MessageStorage::MessageStorage()
{
    // Message descriptions, preprogrammed for now.
    message_descriptions[0] = "Ok.";
    message_descriptions[1] = "Yes.";
    message_descriptions[2] = "No.";
    message_descriptions[3] = "Not sure.";
    message_descriptions[4] = "Maybe.";
    message_descriptions[5] = "Check this out.";
    message_descriptions[6] = "Come here.";
    message_descriptions[7] = "Closer.";
    message_descriptions[8] = "Farther.";
    message_descriptions[9] = "Returning soon.";
    message_descriptions[10] = "Surfaced.";
    message_descriptions[11] = "Underwater.";

    emergency_descriptions[0] = "Help!";
    emergency_descriptions[1] = "Partner in trouble!";
    emergency_descriptions[2] = "Not feeling well!";
    emergency_descriptions[3] = "Stuck!";
    emergency_descriptions[4] = "Oxygen low!";
    emergency_descriptions[5] = "Lost!";
    emergency_descriptions[6] = "Hurt!";
    emergency_descriptions[7] = "Equipment malfunction!";
    emergency_descriptions[8] = "Dangerous animal!";
    emergency_descriptions[9] = "Where are you?";
    emergency_descriptions[10] = "Cold!";
    emergency_descriptions[11] = "Sea horses in danger!";
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
