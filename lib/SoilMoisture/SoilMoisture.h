#ifndef SOIL_MOISTURE_H
#define SOIL_MOISTURE_H

#include <Arduino.h>

class SoilMoisture
{
public:
    explicit SoilMoisture(uint8_t adcPin);

    void begin();
    uint16_t readRaw();
    float readPercentage();
    void setCalibration(uint16_t dryValue, uint16_t wetValue);

private:
    uint8_t _adcPin;
    uint16_t _dryCal;
    uint16_t _wetCal;
};

#endif
