//ARYA

#include "SensirionI2CScd4x.h"
#include <Arduino.h>
#include <Wire.h>

#define SCD4X_I2C_ADDRESS 0x62

SensirionI2CScd4x::SensirionI2CScd4x() {
}

void SensirionI2CScd4x::begin(TwoWire& i2cBus) {
    _i2cBus = &i2cBus;
}

uint16_t SensirionI2CScd4x::startPeriodicMeasurement() {
    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(0x21);
    Wire.write(0xB1);
    uint16_t error = Wire.endTransmission();
    delay(1);
    return error;
}

uint16_t SensirionI2CScd4x::readMeasurementTicks(uint16_t& co2,
                                                 uint16_t& temperature,
                                                 uint16_t& humidity) {
    uint8_t command[2] = {0xEC, 0x05}; // Command bytes
    uint8_t buffer[9]; // Buffer to hold received data
    uint16_t error = 0; // Initialize error variable

    // Send command to the device
    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, 2); // Send command bytes
    error = Wire.endTransmission();
    if (error) {
        return error; // Return error if transmission failed
    }

    // Request data from the device
    
    delay(1); // Wait for the device to process the request

    Wire.requestFrom(SCD4X_I2C_ADDRESS, 9); // Request 9 bytes of data
    if (Wire.available() < 9) {
        return 3; // Return error if insufficient data received
    }
    for (int i = 0; i < 9; i++) {
        buffer[i] = Wire.read(); // Read data into buffer
    }

    // Parse received data
    co2 = (buffer[0] << 8) | buffer[1];
    temperature = (buffer[3] << 8) | buffer[4];
    humidity = (buffer[6] << 8) | buffer[7];

    return error; // Return any transmission error (0 if successful)
}

uint16_t SensirionI2CScd4x::readMeasurement(uint16_t& co2, float& temperature,
                                            float& humidity) {
    uint16_t error;
    uint16_t temperatureTicks;
    uint16_t humidityTicks;

    error = readMeasurementTicks(co2, temperatureTicks, humidityTicks);
    if (error) {
        return error;
    }

    temperature = static_cast<float>(temperatureTicks * 175.0 / 65535.0 - 45.0);
    humidity = static_cast<float>(humidityTicks * 100.0 / 65535.0);
    return error;
}

uint16_t SensirionI2CScd4x::stopPeriodicMeasurement() {
    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(0x3F);
    Wire.write(0x86);
    uint16_t error = Wire.endTransmission();
    delay(500);
    return error;
}

uint16_t SensirionI2CScd4x::getTemperatureOffsetTicks(uint16_t& tOffset) {
    uint8_t command[2] = {0x23, 0x18}; // Command bytes
    uint8_t buffer[3]; // Buffer to hold received data
    uint16_t error = 0; // Initialize error variable

    // Send command to the device
    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command)); // Send command bytes
    error = Wire.endTransmission();
    if (error) {
        return error; // Return error if transmission failed
    }

    // Request data from the device
    
    delay(1); // Wait for the device to process the request

    Wire.requestFrom(SCD4X_I2C_ADDRESS, 3); // Request 9 bytes of data
    if (Wire.available() < 3) {
        return 3; // Return error if insufficient data received
    }
    for (int i = 0; i < 3; i++) {
        buffer[i] = Wire.read(); // Read data into buffer
    }

    // Parse received data
    tOffset = (buffer[0] << 8) | buffer[1];

    return error; // Return any transmission error (0 if successful)
}

uint16_t SensirionI2CScd4x::getTemperatureOffset(float& tOffset) {
    uint16_t error;
    uint16_t tOffsetTicks;

    error = getTemperatureOffsetTicks(tOffsetTicks);
    if (error) {
        return error;
    }

    tOffset = static_cast<float>(tOffsetTicks * 175.0 / 65536.0);
    return error;
}

uint16_t SensirionI2CScd4x::setTemperatureOffsetTicks(uint16_t tOffset) {
    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    uint8_t command[4] = {0x24, 0x1D, (tOffset >> 8) & 0xFF, tOffset & 0xFF};
    Wire.write(command, sizeof(command)); // Send command bytes
    uint8_t error = Wire.endTransmission();
    delay(1);
    return error;
}

uint16_t SensirionI2CScd4x::setTemperatureOffset(float tOffset) {
    uint16_t tOffsetTicks =
        static_cast<uint16_t>(tOffset * 65536.0 / 175.0 + 0.5f);
    return setTemperatureOffsetTicks(tOffsetTicks);
}

uint16_t SensirionI2CScd4x::getSensorAltitude(uint16_t& sensorAltitude) {
    uint8_t command[2] = {0x23, 0x22}; // Command bytes
    uint8_t buffer[3]; // Buffer to hold received data
    uint16_t error = 0; // Initialize error variable

    // Send command to the device
    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command)); // Send command bytes
    error = Wire.endTransmission();
    if (error) {
        return error; // Return error if transmission failed
    }

    // Request data from the device
    
    delay(1); // Wait for the device to process the request

    Wire.requestFrom(SCD4X_I2C_ADDRESS, 3); // Request 9 bytes of data
    if (Wire.available() < 3) {
        return 3; // Return error if insufficient data received
    }
    for (int i = 0; i < 3; i++) {
        buffer[i] = Wire.read(); // Read data into buffer
    }

    // Parse received data
    sensorAltitude = (buffer[0] << 8) | buffer[1];

    return error; // Return any transmission error (0 if successful)
}