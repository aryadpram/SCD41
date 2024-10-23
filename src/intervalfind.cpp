#include <Arduino.h>
#include "SensirionI2CScd4x.h"
#include <SDFat.h>
#include <RTClib.h>
#include <HX711.h>

// SCD41 and SD card configuration
SensirionI2CScd4x scd4x;
SdFat32 sd;
File32 file;
RTC_DS3231 rtc;

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(8)
#define SD_CS_PIN 5

#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS

// HX711 configuration
HX711 hx711;
const float shuntResistor = 0.15;  // Shunt resistor value in ohms
const float Vref = 4.64;  // HX711 reference voltage (in volts) 
const int gain = 128;  // Gain for the HX711

unsigned long previousMillis = 0;    // Store the last time the sensor data was published
unsigned long interval = 0;       // Initial interval of 5 seconds
unsigned long lastIntervalUpdateMillis = 0; // Track when the interval was last updated

// Define preset intervals and corresponding times in ms
const unsigned long presetTimes[] = {1800000}; 
const unsigned long presetIntervals[] = {1, 5000, 15000, 35000, 75000, 155000, 315000, 635000, 1275000}; // Intervals for each period (5s, 10s, 30s, 60s)
const int presetCount = sizeof(presetIntervals) / sizeof(presetIntervals[0]); // Number of preset intervals
int currentPresetIndex = 0; // Current preset index

void setup() {
  Wire.begin();
  scd4x.begin(Wire);

  uint16_t error;
  error = scd4x.wakeUp(); // Wake up the sensor

  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  sd.begin(SD_CONFIG);

  // HX711 initialization (DOUT, SCK)
  hx711.begin(4, 3);  // Connect DOUT to pin 4, SCK to pin 3

  Serial.begin(115200);
  interval = presetIntervals[currentPresetIndex]; // Set initial interval
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long timeElapsed = currentMillis - lastIntervalUpdateMillis;

  // Manually update the interval based on predefined times
  for (int i = 0; i < sizeof(presetTimes) / sizeof(presetTimes[0]); i++) {
    if (timeElapsed >= presetTimes[i]) {
      // Update to the next preset interval
      currentPresetIndex = (currentPresetIndex + 1) % presetCount; // Loop back to the start
      interval = presetIntervals[currentPresetIndex];
      lastIntervalUpdateMillis = currentMillis; // Reset the timer
    }
  }

  // Check if it's time to perform a new measurement
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Wake up and measure SCD41 data
    uint16_t error = scd4x.wakeUp();
    error = scd4x.measureSingleShot();
    
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
    
    long rawValue = hx711.read();
    
    float voltage = ((float)rawValue / (1L << 23)) * (Vref / gain);
    
    float current = voltage / shuntResistor;
    
    DateTime timestamp = rtc.now();

    String csvData = timestamp.timestamp() + ", " + String(interval) + ", " + String(co2) + ", " + String(temperature) + ", " + String(humidity) + ", " + String(voltage, 6) + ", " + String(current, 6);

    Serial.println(csvData);
    
    file.open("data.csv", FILE_WRITE);
    file.println(csvData);
    file.close();

    error = scd4x.powerDown();
  }
}
