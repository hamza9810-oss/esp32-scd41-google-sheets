#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include <Arduino.h>

class PHSensor
{
public:
    PHSensor(uint8_t pin, float vref = 3.3);

    void begin();
    float readPH();

    void setCalibration(float neutralVoltage, float slope);

private:
    uint8_t _pin;
    float _vref;

    float _neutralVoltage;  // voltage at pH 7
    float _slope;           // sensitivity
};

#endif
