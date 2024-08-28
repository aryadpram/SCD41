//ARYA

#include "SensirionI2CScd4x.h"
#include <Arduino.h>
#include <Wire.h>
#include "CRC8.h"

#define SCD4X_I2C_ADDRESS 0x62

SensirionI2CScd4x::SensirionI2CScd4x() {
    _co2Readings = new uint16_t[MAX_READINGS];
}

void SensirionI2CScd4x::begin(TwoWire& i2cBus) {
    _i2cBus = &i2cBus;
}

uint16_t SensirionI2CScd4x::startPeriodicMeasurement() {
    uint8_t command[2] = {0x21, 0xB1};
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    
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
    uint16_t error = 0;
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
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();

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

    uint8_t crc = generateCRC(buffer, 2, CRC31_ff);

    if (crc != buffer[2]){
        return 7;
    }

    tOffset = (buffer[0] << 8) | buffer[1];

    return error;
}

uint16_t SensirionI2CScd4x::getTemperatureOffset(float& tOffset) {
    uint16_t tOffsetTicks;
    uint16_t error = 0;

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
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();

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

    uint8_t crc = generateCRC(buffer, 2, CRC31_ff);

    if (crc != buffer[2]){
        return 7;
    }

    sensorAltitude = (buffer[0] << 8) | buffer[1];

    return error;
}

uint16_t SensirionI2CScd4x::setSensorAltitude(uint16_t sensorAltitude) {
    uint8_t command[5] = {0x24, 0x27, (sensorAltitude >> 8) & 0xFF, sensorAltitude & 0xFF, 0};
    command[4] = generateCRC(&command[2], 2, CRC31_ff);
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();

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

    uint8_t crc = generateCRC(buffer, 2, CRC31_ff);

    if (crc != buffer[2]){
        return 7;
    }

    ambientPressure = (buffer[0] << 8) | buffer[1];

    return error;
}

uint16_t SensirionI2CScd4x::setAmbientPressure(uint16_t ambientPressure) {
    uint8_t command[5] = {0xE0, 0x00, (ambientPressure >> 8) & 0xFF, ambientPressure & 0xFF, 0};
    command[4] = generateCRC(&command[2], 2, CRC31_ff);
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();

    delay(1);

    return error;
}

uint16_t SensirionI2CScd4x::performForcedRecalibration(uint16_t targetCo2Concentration,
                                                        uint16_t& frcCorrection) {
    uint8_t command[5] = {0x24, 0x27, (targetCo2Concentration >> 8) & 0xFF, targetCo2Concentration & 0xFF, 0};
    command[4] = generateCRC(&command[2], 2, CRC31_ff);
    uint16_t error = 0;
    uint8_t buffer[3];

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();

    delay(400);

    Wire.requestFrom(SCD4X_I2C_ADDRESS, sizeof(buffer));
    if (Wire.available() < sizeof(buffer)) {
        return 6;
    }
    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = Wire.read();
    }

    uint8_t crc = generateCRC(buffer, 2, CRC31_ff);

    if (crc != buffer[2]){
        return 7;
    }

    frcCorrection = (buffer[0] << 8) | buffer[1];

    return error;
}

uint16_t SensirionI2CScd4x::getAutomaticSelfCalibration(uint16_t& ascEnabled) {
    uint8_t command[2] = {0x23, 0x13};
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

    uint8_t crc = generateCRC(buffer, 2, CRC31_ff);

    if (crc != buffer[2]){
        return 7;
    }

    ascEnabled = (buffer[0] << 8) | buffer[1];

    return error;
}

uint16_t SensirionI2CScd4x::setAutomaticSelfCalibration(uint16_t ascEnabled) {
    uint8_t command[5] = {0x24, 0x16, (ascEnabled >> 8) & 0xFF, ascEnabled & 0xFF, 0};
    command[4] = generateCRC(&command[2], 2, CRC31_ff);
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();

    delay(1);

    return error;
}

uint16_t SensirionI2CScd4x::getAutomaticSelfCalibrationInitialPeriod(uint16_t& ascInitialPeriod) {
    uint8_t command[2] = {0x23, 0x40};
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

    uint8_t crc = generateCRC(buffer, 2, CRC31_ff);

    if (crc != buffer[2]){
        return 7;
    }

    ascInitialPeriod = (buffer[0] << 8) | buffer[1];

    return error;
}

uint16_t SensirionI2CScd4x::setAutomaticSelfCalibrationInitialPeriod(uint16_t& ascInitialPeriod) {
    uint8_t command[5] = {0x24, 0x45, (ascInitialPeriod >> 8) & 0xFF, ascInitialPeriod & 0xFF, 0};
    command[4] = generateCRC(&command[2], 2, CRC31_ff);
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();

    delay(1);

    return error;
}

uint16_t SensirionI2CScd4x::getAutomaticSelfCalibrationStandardPeriod(uint16_t& ascStandardPeriod) {
    uint8_t command[2] = {0x23, 0x4B};
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

    uint8_t crc = generateCRC(buffer, 2, CRC31_ff);

    if (crc != buffer[2]){
        return 7;
    }

    ascStandardPeriod = (buffer[0] << 8) | buffer[1];

    return error;
}

uint16_t SensirionI2CScd4x::setAutomaticSelfCalibrationStandardPeriod(uint16_t& ascStandardPeriod) {
    uint8_t command[5] = {0x24, 0x4E, (ascStandardPeriod >> 8) & 0xFF, ascStandardPeriod & 0xFF, 0};
    command[4] = generateCRC(&command[2], 2, CRC31_ff);
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();

    delay(1);

    return error;
}

uint16_t SensirionI2CScd4x::getDataReadyFlag(bool& dataReady) {
    uint8_t command[2] = {0xE4, 0xB8};
    uint16_t localDataReady = 0;
    uint16_t error = 0;
    uint8_t buffer[3];

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();

    delay(1);

    Wire.requestFrom(SCD4X_I2C_ADDRESS, sizeof(buffer));
    if (Wire.available() < sizeof(buffer)) {
        return 6;
    }
    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = Wire.read();
    }

    uint8_t crc = generateCRC(buffer, 2, CRC31_ff);

    if (crc != buffer[2]){
        return 7;
    }

    localDataReady = (buffer[0] << 8) | buffer[1];
    dataReady = (localDataReady & 0x07FF) != 0;

    return error;
}

uint16_t SensirionI2CScd4x::persistSettings() {
    uint8_t command[2] = {0x36, 0x15};
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    
    delay(800);
    
    return error;
}

uint16_t SensirionI2CScd4x::getSerialNumber(uint16_t& serial0,
                                            uint16_t& serial1,
                                            uint16_t& serial2) {
    uint8_t command[2] = {0x36, 0x82};
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
    
    uint8_t crc_serial0 = generateCRC(buffer, 2, CRC31_ff);
    uint8_t crc_serial1 = generateCRC(&buffer[3], 2, CRC31_ff);
    uint8_t crc_serial2 = generateCRC(&buffer[6], 2, CRC31_ff);

    if (crc_serial0 != buffer[2] || crc_serial1 != buffer[5] || crc_serial2 != buffer[8]) {
        return 7;
    }

    serial0 = (buffer[0] << 8) | buffer[1];
    serial1 = (buffer[3] << 8) | buffer[4];
    serial2 = (buffer[6] << 8) | buffer[7];

    return error;
}

uint16_t SensirionI2CScd4x::performSelfTest(uint16_t& sensorStatus) {
    uint8_t command[2] = {0x36, 0x39};
    uint8_t buffer[3];
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    if (error) {
        return error;
    }
    
    delay(10000);

    Wire.requestFrom(SCD4X_I2C_ADDRESS, sizeof(buffer));
    if (Wire.available() < sizeof(buffer)) {
        return 6;
    }
    for (int i = 0; i < sizeof(buffer); i++) {
        buffer[i] = Wire.read();
    
    }

    uint8_t crc = generateCRC(buffer, 2, CRC31_ff);

    if (crc != buffer[2]){
        return 7;
    }

    sensorStatus = (buffer[0] << 8) | buffer[1];

    return error;
}

uint16_t SensirionI2CScd4x::performFactoryReset() {
    uint8_t command[2] = {0x36, 0x32};
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    
    delay(800);
    
    return error;
}

uint16_t SensirionI2CScd4x::reinit() {
    uint8_t command[2] = {0x36, 0x46};
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    
    delay(20);
    
    return error;
}

uint16_t SensirionI2CScd4x::measureSingleShot() {
    uint8_t command[2] = {0x21, 0x9D};
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    
    delay(5000);
    
    return error;
}

uint16_t SensirionI2CScd4x::measureSingleShotRhtOnly() {
    uint8_t command[2] = {0x21, 0x96};
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    
    delay(50);
    
    return error;
}

uint16_t SensirionI2CScd4x::powerDown() {
    uint8_t command[2] = {0x36, 0xE0};
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission();
    
    delay(1);
    
    return error;
}

uint16_t SensirionI2CScd4x::wakeUp() {
    uint8_t command[2] = {0x36, 0xF6};
    uint16_t error = 0;

    Wire.beginTransmission(SCD4X_I2C_ADDRESS);
    Wire.write(command, sizeof(command));
    error = Wire.endTransmission(); //error is igonred since the sensor doesnt acknolwedge the wake up call
    
    delay(1);

    return 0;
}

// std::string SensirionI2CScd4x::co2Standard() {
//     uint16_t co2 = 0;
//     uint16_t error = 0;
//     uint16_t temperatureTicks;
//     uint16_t humidityTicks;
//     uint16_t outdoorCO2 = 400;

//     error = readMeasurementTicks(co2, temperatureTicks, humidityTicks);
//     if (error) {
//         return "Error reading measurement (Code: " + std::to_string(error) + ")";
//     }

//     if (co2 <= outdoorCO2 + 400) {
//         return "IDA 1: Very High Indoor Air Quality (CO2 Level: " + std::to_string(co2) + " ppm)";
//     } else if (co2 <= outdoorCO2 + 600) {
//         return "IDA 2: Medium Indoor Air Quality (CO2 Level: " + std::to_string(co2) + " ppm)";
//     } else if (co2 <= outdoorCO2 + 1000) {
//         return "IDA 3: Moderate Indoor Air Quality (CO2 Level: " + std::to_string(co2) + " ppm)";
//     } else {
//         return "IDA 4: Low Indoor Air Quality (CO2 Level: " + std::to_string(co2) + " ppm)";
//     }
// }

SensirionI2CScd4x::~SensirionI2CScd4x() {
    delete[] _co2Readings;
    delete[] _timestamps;
}

void SensirionI2CScd4x::startRollingTWAMeasurement() {
    _co2Readings = new uint16_t[MAX_READINGS];
    _timestamps = new uint32_t[MAX_READINGS];
    _readingCount = 0;
    _readingIndex = 0;
    _runningSum = 0;
    _sumOfWeights = 0;
}


uint32_t SensirionI2CScd4x::elapsedTime(uint32_t start, uint32_t end) {
    return (end >= start) ? (end - start) : (UINT32_MAX - start + end + 1);
}

void SensirionI2CScd4x::updateRollingTWAAccumulation(uint16_t co2, uint32_t timestamp) {
    uint32_t timeDiff = elapsedTime(_timestamps[_readingIndex], timestamp);

    if (_readingCount == MAX_READINGS) {
        _runningSum -= _co2Readings[_readingIndex] * (timestamp - _timestamps[_readingIndex]);
        _sumOfWeights -= timeDiff;
    } else {
        _readingCount++;
    }

    _co2Readings[_readingIndex] = co2;
    _timestamps[_readingIndex] = timestamp;
    _runningSum += co2 * timeDiff;
    _sumOfWeights += timeDiff;

    _readingIndex = (_readingIndex + 1) % MAX_READINGS;
}

uint16_t SensirionI2CScd4x::getRollingTWACO2() const {
    if (_readingCount == 0 || _sumOfWeights == 0) {
        return 0;  // Return 0 if no readings have been taken
    }

    return static_cast<uint16_t>(_runningSum) / _sumOfWeights;
}