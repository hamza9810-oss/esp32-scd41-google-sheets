#include "PH.h"

#define ADC_RESOLUTION 4095.0

PHSensor::PHSensor(uint8_t pin, float vref)
{
    _pin = pin;
    _vref = vref;

    // Default calibration (can change later)
    _neutralVoltage = 2.5;
    _slope = 0.18;
}

void PHSensor::begin()
{
    pinMode(_pin, INPUT);
}

void PHSensor::setCalibration(float neutralVoltage, float slope)
{
    _neutralVoltage = neutralVoltage;
    _slope = slope;
}

float PHSensor::readPH()
{
    const int samples = 10;
    long total = 0;

    for (int i = 0; i < samples; i++)
    {
        total += analogRead(_pin);
        delay(10);
    }

    float avgRaw = total / (float)samples;
    float voltage = avgRaw * (_vref / ADC_RESOLUTION);

    float ph = 7.0 + ((_neutralVoltage - voltage) / _slope);

    if (ph < 0) ph = 0;
    if (ph > 14) ph = 14;

    return ph;
}
