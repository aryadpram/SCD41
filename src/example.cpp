#include <Arduino.h>
#include <Wire.h>
#include <SensirionI2CScd4x.h>

/* scl kuning
scd puith
*/

SensirionI2CScd4x scd4x;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  scd4x.begin(Wire);
 
  uint16_t error;
  
  error = scd4x.stopPeriodicMeasurement();
  error = scd4x.startPeriodicMeasurement();

} 

void loop() {
  uint16_t error;
  uint16_t co2 = 0;
  float temperature = 0.0f;
  float humidity = 0.0f;
  float toff = 0.0f;
  float t = 0.0f;

  error = scd4x.readMeasurement(co2, temperature, humidity);
  if (error){
    Serial.print(error);
    return;
  }
  Serial.print("Co2:");
  Serial.print(co2);
  Serial.print("\t");
  Serial.print("Temperature:");
  Serial.print(temperature);
  Serial.print("\t");
  Serial.print("Humidity:");
  Serial.println(humidity);

  error = scd4x.getTemperatureOffset(t);
  Serial.print (t);
  
  toff = -10;
  error = scd4x.setTemperatureOffset(toff);

  delay(5000);
}