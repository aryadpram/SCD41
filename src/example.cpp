#include <Arduino.h>
#include "SensirionI2CScd4x.h"

SensirionI2CScd4x scd4x;

unsigned long previousMillis = 0;   // Store the last time the sensor data was published
const long interval = 3000;       // Interval at which to publish data (5 minutes)

void setup() {
  Serial.begin(115200);
  Wire.begin();
  uint16_t error;

  scd4x.begin(Wire);

  error = scd4x.stopPeriodicMeasurement();
  if (error) {
      Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
      Serial.println(error);
  }

  // Start Measurement
  error = scd4x.startPeriodicMeasurement();
  if (error) {
      Serial.print("Error trying to execute startPeriodicMeasurement(): ");
      Serial.println(error);
  }

}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    uint16_t error;
    uint16_t co2 = 0;
    float temperature = 0.0f;
    float humidity = 0.0f;
    bool isDataReady = false;

    error = scd4x.getDataReadyFlag(isDataReady);
    if (error) {
        Serial.print("Error trying to execute getDataReadyFlag(): ");
        Serial.println(error);
        return;
    }
    if (!isDataReady) {
        return;
    }

    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        Serial.print("Error trying to execute readMeasurement(): ");
        Serial.println(error);
    } else {
        Serial.print("Co2:");
        Serial.print(co2);
        Serial.print("\t");
        Serial.print("Temperature:");
        Serial.print(temperature);
        Serial.print("\t");
        Serial.print("Humidity:");
        Serial.println(humidity);
    }
  }
}