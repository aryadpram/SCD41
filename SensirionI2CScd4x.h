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

  private:
    TwoWire* _i2cBus = nullptr;
};

#endif /* SENSIRIONI2CSCD4X_H */