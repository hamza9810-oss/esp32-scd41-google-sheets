#include <Arduino.h>
#include "SCD41Sensor.h"
#include "PH.h"

// ================== PIN DEFINITIONS ==================
#define SDA_PIN   8
#define SCL_PIN   9
#define SOIL_PIN  4
#define PH_PIN    5   // change if your pH is on different pin

// ================== SOIL CALIBRATION ==================
#define SOIL_DRY 4095
#define SOIL_WET 1800

// ================== OBJECTS ==================
SCD41Sensor co2Sensor;
SCD41Data   co2Data;

PHSensor ph(PH_PIN);

// =====================================================

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("SYSTEM STARTING...");

    // ---- Init CO2 ----
    if (!co2Sensor.begin(SDA_PIN, SCL_PIN)) {
        Serial.println("❌ CO2 Sensor init failed");
        while (true);
    }

    // ---- Init pH ----
    ph.begin();

    Serial.println("✅ CO2 Sensor initialized");
    Serial.println("✅ Soil Sensor ready");
    Serial.println("✅ pH Sensor ready");
}

// =====================================================

void loop() {

    if (co2Sensor.read(co2Data)) {

        // -------- SOIL --------
        int soilRaw = analogRead(SOIL_PIN);

        float soilPercent =
            (float)(SOIL_DRY - soilRaw) * 100.0 /
            (SOIL_DRY - SOIL_WET);

        if (soilPercent < 0) soilPercent = 0;
        if (soilPercent > 100) soilPercent = 100;

        // -------- PH --------
        float phValue = ph.readPH();

        // -------- PRINT --------
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
        Serial.print(soilRaw);
        Serial.print(" | Soil Moisture: ");
        Serial.print(soilPercent);
        Serial.println(" %");

        Serial.print("pH: ");
        Serial.println(phValue, 2);

        Serial.println("-----------------------------------");
    }

    delay(5000);  // SCD41 refresh ~5 seconds
}
