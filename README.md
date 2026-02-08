# ESP32-S3 SCD41 CO₂ Sensor (CO2-only)

Clean, modular ESP32-S3 project using the Sensirion SCD41 CO₂ sensor.

## Features
- CO₂, temperature, humidity measurement
- Modular driver design
- PlatformIO project

## Hardware
- ESP32-S3 DevKit N16R8
- Sensirion SCD41
- SDA = GPIO 8
- SCL = GPIO 9
- 3.3V power

## Build & Upload
```bash
pio run -t upload
pio device monitor
