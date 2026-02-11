#include <Arduino.h>
#include "SCD41Sensor.h"

#define SDA_PIN 8
#define SCL_PIN 9
#define SOIL_PIN 4

// Soil calibration values
#define SOIL_DRY 4095
#define SOIL_WET 1800

SCD41Sensor co2Sensor;
SCD41Data co2Data;

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("SYSTEM STARTING...");

    // Initialize CO2 sensor
    if (!co2Sensor.begin(SDA_PIN, SCL_PIN)) {
        Serial.println("❌ CO2 Sensor init failed");
        while (true);
    }

    Serial.println("✅ CO2 Sensor initialized");
    Serial.println("✅ Soil Sensor ready");
}

void loop() {

    // ======== CO2 READING ========
    if (co2Sensor.read(co2Data)) {

        // ======== SOIL READING ========
        int raw = analogRead(SOIL_PIN);

        float soilPercent =
            (float)(SOIL_DRY - raw) * 100.0 / (SOIL_DRY - SOIL_WET);

        if (soilPercent < 0) soilPercent = 0;
        if (soilPercent > 100) soilPercent = 100;

        // ======== PRINT DATA ========
        Serial.println("----------- SENSOR DATA -----------");

        Serial.print("CO2: ");
        Serial.print(co2Data.co2);
        Serial.println(" ppm");

        Serial.print("Temperature: ");
        Serial.print(co2Data.temperature, 2);
        Serial.println(" °C");

        Serial.print("Humidity: ");
        Serial.print(co2Data.humidity, 2);
        Serial.println(" %");

        Serial.print("Soil Raw: ");
        Serial.print(raw);
        Serial.print(" | Soil Moisture: ");
        Serial.print(soilPercent);
        Serial.println(" %");

        Serial.println("-----------------------------------");
    }

    delay(5000); // SCD41 updates every ~5 sec
}
