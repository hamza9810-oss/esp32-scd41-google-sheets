#include <Arduino.h>
#include "SCD41Sensor.h"

#define SDA_PIN 8
#define SCL_PIN 9

SCD41Sensor co2Sensor;
SCD41Data co2Data;

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("Starting CO2 application...");

    if (!co2Sensor.begin(SDA_PIN, SCL_PIN)) {
        Serial.println("❌ Sensor initialization failed");
        while (true); // stop here
    }

    Serial.println("✅ Sensor initialized");
}

void loop() {
    if (co2Sensor.read(co2Data)) {
        Serial.print("CO2: ");
        Serial.print(co2Data.co2);
        Serial.print(" ppm | T: ");
        Serial.print(co2Data.temperature, 2);
        Serial.print(" °C | RH: ");
        Serial.print(co2Data.humidity, 2);
        Serial.println(" %");
    }

    delay(5000);
}
