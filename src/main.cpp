#include <Arduino.h>
#include <Wire.h>
#include <SensirionI2cScd4x.h>

// I2C pins (ESP32-S3)
#define SDA_PIN 17
#define SCL_PIN 18

SensirionI2cScd4x scd4x;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("ESP32-S3 + SCD41 normal test");

  // Start I2C
  Wire.setPins(SDA_PIN, SCL_PIN);
  Wire.begin();

  // Init sensor (address 0x62)
  scd4x.begin(Wire, 0x62);

  // Make sure sensor is in a clean state
  scd4x.stopPeriodicMeasurement();
  delay(1000);

  // Start periodic measurement
  scd4x.startPeriodicMeasurement();

  Serial.println("SCD41 started. Waiting for data...");
}

void loop() {
  uint16_t co2;
  float temperature, humidity;

  // SCD41 updates every ~5 seconds
  delay(5000);

  int16_t err = scd4x.readMeasurement(co2, temperature, humidity);
  if (err) {
    Serial.print("Read error: ");
    Serial.println(err);
    return;
  }

  // Ignore invalid startup readings
  if (co2 == 0) {
    Serial.println("Invalid reading, retrying...");
    return;
  }

  Serial.print("CO2: ");
  Serial.print(co2);
  Serial.print(" ppm | T: ");
  Serial.print(temperature, 2);
  Serial.print(" °C | RH: ");
  Serial.print(humidity, 2);
  Serial.println(" %");
}
