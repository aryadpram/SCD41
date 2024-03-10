#ifndef SENSIRIONI2CSCD4X_H
#define SENSIRIONI2CSCD4X_H

#include <Wire.h>

class SensirionI2CScd4x {

  public:
    SensirionI2CScd4x();
    void begin(TwoWire& i2cBus);
    uint16_t startPeriodicMeasurement(void);
    uint16_t readMeasurementTicks(uint16_t& co2, uint16_t& temperature,uint16_t& humidity);
    uint16_t readMeasurement(uint16_t& co2, float& temperature,float& humidity);
    uint16_t stopPeriodicMeasurement(void);
    uint16_t getTemperatureOffsetTicks(uint16_t& tOffset);
    uint16_t getTemperatureOffset(float& tOffset);
    uint16_t setTemperatureOffsetTicks(uint16_t tOffset);
    uint16_t setTemperatureOffset(float tOffset);
    uint16_t getSensorAltitude(uint16_t& sensorAltitude);
    uint16_t setSensorAltitude(uint16_t sensorAltitude);
    uint16_t getAmbientPressure(uint16_t& ambientPressure);
    uint16_t setAmbientPressure(uint16_t ambientPressure);
    uint16_t performForcedRecalibration(uint16_t targetCo2Concentration, uint16_t &frcCorrection);
    uint16_t SensirionI2CScd4x::getAutomaticSelfCalibration(uint16_t& ascEnabled);
    uint16_t SensirionI2CScd4x::setAutomaticSelfCalibration(uint16_t ascEnabled) ;
    uint16_t SensirionI2CScd4x::getAutomaticSelfCalibrationInitialPeriod(uint16_t& ascInitialPeriod) ;
    uint16_t SensirionI2CScd4x::setAutomaticSelfCalibrationInitialPeriod(uint16_t& ascInitialPeriod) ;
    uint16_t SensirionI2CScd4x::getAutomaticSelfCalibrationStandardPeriod(uint16_t& ascStandardPeriod) ;
    uint16_t SensirionI2CScd4x::setAutomaticSelfCalibrationStandardPeriod(uint16_t& ascStandardPeriod) ;
    uint16_t SensirionI2CScd4x::getDataReadyFlag(bool& dataReady) ;
    uint16_t SensirionI2CScd4x::persistSettings() ;
    uint16_t SensirionI2CScd4x::getSerialNumber(uint16_t& serial0, uint16_t& serial1, uint16_t& serial2) ;
    uint16_t SensirionI2CScd4x::performSelfTest(uint16_t& sensorStatus) ;
    uint16_t SensirionI2CScd4x::performFactoryReset() ;
    uint16_t SensirionI2CScd4x::reinit() ;
    uint16_t SensirionI2CScd4x::measureSingleShot() ;
    uint16_t SensirionI2CScd4x::measureSingleShotRhtOnly() ;
    uint16_t SensirionI2CScd4x::powerDown() ;
    uint16_t SensirionI2CScd4x::wakeUp() ;

private:
    TwoWire* _i2cBus = nullptr;
};

#endif /* SENSIRIONI2CSCD4X_H */