#ifndef SENSIRIONI2CSCD4X_H
#define SENSIRIONI2CSCD4X_H

#include <Wire.h>
#include <string>

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
    uint16_t getAutomaticSelfCalibration(uint16_t& ascEnabled);
    uint16_t setAutomaticSelfCalibration(uint16_t ascEnabled) ;
    uint16_t getAutomaticSelfCalibrationInitialPeriod(uint16_t& ascInitialPeriod) ;
    uint16_t setAutomaticSelfCalibrationInitialPeriod(uint16_t& ascInitialPeriod) ;
    uint16_t getAutomaticSelfCalibrationStandardPeriod(uint16_t& ascStandardPeriod) ;
    uint16_t setAutomaticSelfCalibrationStandardPeriod(uint16_t& ascStandardPeriod) ;
    uint16_t getDataReadyFlag(bool& dataReady) ;
    uint16_t persistSettings() ;
    uint16_t getSerialNumber(uint16_t& serial0, uint16_t& serial1, uint16_t& serial2) ;
    uint16_t performSelfTest(uint16_t& sensorStatus) ;
    uint16_t performFactoryReset() ;
    uint16_t reinit() ;
    uint16_t measureSingleShot() ;
    uint16_t measureSingleShotRhtOnly() ;
    uint16_t powerDown() ;
    uint16_t wakeUp() ;

    ~SensirionI2CScd4x();
    void startRollingTWAMeasurement();
    uint16_t updateRollingTWAAccumulation();
    float getRollingTWACO2();

private:
    TwoWire* _i2cBus = nullptr;

    static const uint16_t MAX_READINGS = 5760; // Max readings in 8 hours (1 reading per 5 seconds)
    uint16_t* _co2Readings;
    uint16_t _readingCount;
    uint16_t _readingIndex;
    uint32_t _lastReadingTime;
    uint32_t _runningSum;
    bool _dataReady;

    static uint32_t elapsedTime(uint32_t start, uint32_t end);
};

#endif /* SENSIRIONI2CSCD4X_H */