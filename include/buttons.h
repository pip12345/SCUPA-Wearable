#pragma once

// SCUPA wearable button control interface

#include "Button2.h"
#include <arduino.h>

// !! PINS USED FOR UP DOWN LEFT AND RIGHT !!
#define UP_PIN 33
#define DOWN_PIN 25
#define LEFT_PIN 26
#define RIGHT_PIN 13

class ButtonController {
 public:
    ButtonController();
    void loopButtons();
    void init();

    Button2 up;
    Button2 down;
    Button2 left;
    Button2 right;
  private:
    void pressed(Button2& button);
};