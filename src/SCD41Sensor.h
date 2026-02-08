#ifndef SCD41_SENSOR_H
#define SCD41_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <SensirionI2cScd4x.h>

/*
  This struct holds one measurement from the SCD41 sensor
*/
struct SCD41Data {
    uint16_t co2;
    float temperature;
    float humidity;
};

/*
  This class represents the SCD41 CO2 sensor
*/
class SCD41Sensor {
public:
    // Initialize sensor and I2C
    bool begin(int sda, int scl);

    // Read sensor data
    bool read(SCD41Data &data);

private:
    SensirionI2cScd4x scd4x;
};

#endif
