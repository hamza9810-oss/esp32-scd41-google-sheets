#include "SCD41Sensor.h"

bool SCD41Sensor::begin(int sda, int scl) {
    Wire.begin(sda, scl);
    scd4x.begin(Wire, 0x62);

    int16_t error;
    char errorMessage[256];

    // Stop any previous measurement
    scd4x.stopPeriodicMeasurement();
    delay(500);

    // Start measurement
    error = scd4x.startPeriodicMeasurement();
    if (error) {
        errorToString(error, errorMessage, 256);
        Serial.print("SCD41 start error: ");
        Serial.println(errorMessage);
        return false;
    }

    return true;
}

bool SCD41Sensor::read(SCD41Data &data) {
    bool ready = false;
    int16_t error;

    error = scd4x.getDataReadyStatus(ready);
    if (error || !ready) {
        return false;
    }

    error = scd4x.readMeasurement(
        data.co2,
        data.temperature,
        data.humidity
    );

    if (error || data.co2 == 0) {
        return false;
    }

    return true;
}
