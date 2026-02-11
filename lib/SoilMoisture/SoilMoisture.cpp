#include "SoilMoisture.h"

SoilMoisture::SoilMoisture(uint8_t adcPin)
    : _adcPin(adcPin), _dryCal(3500), _wetCal(1500)
{
}

void SoilMoisture::begin()
{
    analogReadResolution(12);
    pinMode(_adcPin, INPUT);
}

uint16_t SoilMoisture::readRaw()
{
    return analogRead(_adcPin);
}

void SoilMoisture::setCalibration(uint16_t dryValue, uint16_t wetValue)
{
    _dryCal = dryValue;
    _wetCal = wetValue;
}

float SoilMoisture::readPercentage()
{
    uint16_t raw = readRaw();

    if (raw > _dryCal) raw = _dryCal;
    if (raw < _wetCal) raw = _wetCal;

    return 100.0f * (_dryCal - raw) / (_dryCal - _wetCal);
}
