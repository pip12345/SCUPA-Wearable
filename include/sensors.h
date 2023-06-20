#pragma once

class Sensors {
public:
    Sensors(); // init all sensors

    int readDepth();
    int readCompass();

    int depth{}; // Last measured depth

private:

};