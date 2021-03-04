// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// Depends on the following Arduino libraries:
// - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN1            2         // Pin which is connected to the DHT sensor.
#define DHTPIN2            3         // Pin which is connected to the DHT sensor.

// Uncomment the type of sensor in use:
#define DHTTYPE1           DHT11     // DHT 11 
// #define DHTTYPE1           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE1           DHT21     // DHT 21 (AM2301)
// Uncomment the type of sensor in use:
#define DHTTYPE2           DHT11     // DHT 11 
// #define DHTTYPE2           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE2          DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht1(DHTPIN1, DHTTYPE1);
DHT_Unified dht2(DHTPIN2, DHTTYPE2);


uint32_t delayMS1;
uint32_t delayMS2;


void setup() {
  Serial.begin(9600); 
  // Initialize device.
  dht1.begin();
  Serial.println("DHT11 Sensor 1 Example");
  // Print temperature sensor details.
  sensor_t sensor1;
  dht1.temperature().getSensor(&sensor1);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor1.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor1.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor1.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor1.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor1.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor1.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht1.humidity().getSensor(&sensor1);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor1.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor1.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor1.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor1.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor1.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor1.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  delayMS1 = sensor1.min_delay / 1000;

    // Initialize device.
  dht2.begin();
  Serial.println("DHT11 Sensor 2");
  // Print temperature sensor details.
  sensor_t sensor2;
  dht2.temperature().getSensor(&sensor2);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor2.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor2.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor2.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor2.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor2.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor2.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht2.humidity().getSensor(&sensor2);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor2.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor2.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor2.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor2.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor2.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor2.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  delayMS2 = sensor2.min_delay / 1000;
}

void loop() {
  // Delay between measurements.
  delay(delayMS1);
  // Get temperature event and print its value.
  sensors_event_t event1;  
  dht1.temperature().getEvent(&event1);
  if (isnan(event1.temperature)) {
    Serial.println("DHT Sensor 1 Error reading temperature!");
  }
  else {
    Serial.print("DHT Sensor 1 Temperature: ");
    Serial.print(event1.temperature);
    Serial.println(" *C");
  }
  // Get humidity event and print its value.
  dht1.humidity().getEvent(&event1);
  if (isnan(event1.relative_humidity)) {
    Serial.println("DHT Sensor 1 Error reading humidity!");
  }
  else {
    Serial.print("DHT Sensor 1 Humidity: ");
    Serial.print(event1.relative_humidity);
    Serial.println("%");
  }
   // Delay between measurements.
  delay(delayMS2);
  // Get temperature event and print its value.
  sensors_event_t event2;  
  dht2.temperature().getEvent(&event2);
  if (isnan(event2.temperature)) {
    Serial.println("DHT Sensor 2 Error reading temperature!");
  }
  else {
    Serial.print("DHT Sensor 2 Temperature: ");
    Serial.print(event2.temperature);
    Serial.println(" *C");
  }
  // Get humidity event and print its value.
  dht2.humidity().getEvent(&event2);
  if (isnan(event2.relative_humidity)) {
    Serial.println("DHT Sensor 2 Error reading humidity!");
  }
  else {
    Serial.print("DHT Sensor 2 Humidity: ");
    Serial.print(event2.relative_humidity);
    Serial.println("%");
  }
}
