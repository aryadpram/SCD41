//ARYA

#include "SensirionI2CScd4x.h"
#include <Arduino.h>
#include <Wire.h>
#include "CRC8.h"

#define SCD4X_I2C_ADDRESS 0x62

SensirionI2CScd4x::SensirionI2CScd4x() {
}

void SensirionI2CScd4x::begin(TwoWire& i2cBus) {
    _i2cBus = &i2cBus;
}

uint16_t SensirionI2CScd4x::startPeriodicMeasurement() {
    uint8_t command[2] = {0x21, 0xB1};

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    uint16_t error = Wire.endTransmission();
    
    delay(1);
    
    return error;
}

uint16_t SensirionI2CScd4x::readMeasurementTicks(uint16_t& co2,
                                                 uint16_t& temperature,
                                                 uint16_t& humidity) {
    uint8_t command[2] = {0xEC, 0x05};
    uint8_t buffer[9];
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    if (error) {
        return error;
    }
    
    delay(1);

    Wire.requestFrom(SCD4X_I2C_ADDRESS, sizeof(buffer));
    if (Wire.available() < sizeof(buffer)) {
        return 6;
    }
    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = Wire.read();
    }
    
    uint8_t crc_co2 = generateCRC(buffer, 2, CRC31_ff);
    uint8_t crc_temperature = generateCRC(&buffer[3], 2, CRC31_ff);
    uint8_t crc_humidity = generateCRC(&buffer[6], 2, CRC31_ff);

    if (crc_co2 != buffer[2] || crc_temperature != buffer[5] || crc_humidity != buffer[8]) {
        return 7;
    }

    co2 = (buffer[0] << 8) | buffer[1];
    temperature = (buffer[3] << 8) | buffer[4];
    humidity = (buffer[6] << 8) | buffer[7];

    return error;
}

uint16_t SensirionI2CScd4x::readMeasurement(uint16_t& co2, 
                                            float& temperature,
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
    uint8_t command[2] = {0x3F, 0x86};

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    uint16_t error = Wire.endTransmission();

    delay(500);

    return error;
}

uint16_t SensirionI2CScd4x::getTemperatureOffsetTicks(uint16_t& tOffset) {
    uint8_t command[2] = {0x23, 0x18};
    uint8_t buffer[3];
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    if (error) {
        return error;
    }
    
    delay(1);

    Wire.requestFrom(SCD4X_I2C_ADDRESS, sizeof(buffer));
    if (Wire.available() < sizeof(buffer)) {
        return 6;
    }
    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = Wire.read();
    
    }

    uint8_t crc_tOffset = generateCRC(buffer, 2, CRC31_ff);

    if (crc_tOffset != buffer[2]){
        return 7;
    }

    tOffset = (buffer[0] << 8) | buffer[1];

    return error;
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
    uint8_t command[5] = {0x24, 0x1D, (tOffset >> 8) & 0xFF, tOffset & 0xFF, 0};
    command[4] = generateCRC(&command[2], 2, CRC31_ff);

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    uint16_t error = Wire.endTransmission();

    delay(1);

    return error;
}

uint16_t SensirionI2CScd4x::setTemperatureOffset(float tOffset) {
    uint16_t tOffsetTicks =
        static_cast<uint16_t>(tOffset * 65536.0 / 175.0 + 0.5f);
    return setTemperatureOffsetTicks(tOffsetTicks);
}

uint16_t SensirionI2CScd4x::getSensorAltitude(uint16_t& sensorAltitude) {
    uint8_t command[2] = {0x23, 0x22};
    uint8_t buffer[3];
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    if (error) {
        return error;
    }

    delay(1);

    Wire.requestFrom(SCD4X_I2C_ADDRESS, sizeof(buffer));
    if (Wire.available() < sizeof(buffer)) {
        return 6;
    }
    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = Wire.read();
    }

    uint8_t crc_sensorAltitude = generateCRC(buffer, 2, CRC31_ff);

    if (crc_sensorAltitude != buffer[2]){
        return 7;
    }

    sensorAltitude = (buffer[0] << 8) | buffer[1];

    return error;
}

uint16_t SensirionI2CScd4x::setSensorAltitude(uint16_t sensorAltitude) {
    uint8_t command[5] = {0x24, 0x27, (sensorAltitude >> 8) & 0xFF, sensorAltitude & 0xFF, 0};
    command[4] = generateCRC(&command[2], 2, CRC31_ff);

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    uint16_t error = Wire.endTransmission();

    delay(1);

    return error;
}

uint16_t SensirionI2CScd4x::getAmbientPressure(uint16_t& ambientPressure) {
    uint8_t command[2] = {0xE0, 0x00};
    uint8_t buffer[3];
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    if (error) {
        return error;
    }
    
    delay(1);

    Wire.requestFrom(SCD4X_I2C_ADDRESS, sizeof(buffer));
    if (Wire.available() < sizeof(buffer)) {
        return 6;
    }
    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = Wire.read();
    }

    uint8_t crc_ambientPressure = generateCRC(buffer, 2, CRC31_ff);

    if (crc_ambientPressure != buffer[2]){
        return 7;
    }

    ambientPressure = (buffer[0] << 8) | buffer[1];

    return error;
}

uint16_t SensirionI2CScd4x::setAmbientPressure(uint16_t ambientPressure) {
    uint8_t command[5] = {0x24, 0x27, (ambientPressure >> 8) & 0xFF, ambientPressure & 0xFF, 0};
    command[4] = generateCRC(&command[2], 2, CRC31_ff);

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    uint16_t error = Wire.endTransmission();

    delay(1);

    return error;
}