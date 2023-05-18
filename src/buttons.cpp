#include "buttons.h"

ButtonController::ButtonController() {
    up.begin(UP_PIN);
    down.begin(DOWN_PIN);
    left.begin(LEFT_PIN);
    right.begin(RIGHT_PIN);

    // up.setPressedHandler(pressed);
    // down.setPressedHandler(pressed);
    // left.setPressedHandler(pressed);
    // right.setPressedHandler(pressed);
}

void ButtonController::loopButtons() {
    up.loop();
    down.loop();
    left.loop();
    right.loop();
}

void ButtonController::init()
{
    
}

void ButtonController::pressed(Button2& btn)
{
    Serial.println("pressed");
}
