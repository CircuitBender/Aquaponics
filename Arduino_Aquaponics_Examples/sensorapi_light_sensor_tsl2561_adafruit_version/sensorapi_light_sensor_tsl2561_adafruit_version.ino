#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.

   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).

   Connections
   ===========
   Connect SCL to analog 13
   Connect SDA to analog 12
   Connect VDD to 3.3V DC - 5.1 DC Max
   Connect GROUND to common ground

   I2C Address
   ===========
   The address will be different depending on whether you leave
   the ADDR pin floating (addr 0x39), or tie it to ground or vcc.
   The default addess is 0x39, which assumes the ADDR pin is floating
   (not connected to anything).  If you set the ADDR pin high
   or low, use TSL2561_ADDR_HIGH (0x49) or TSL2561_ADDR_LOW
   (0x29) respectively.

   History
   =======
   2013/JAN/31  - First version (KTOWN)
*/

Adafruit_TSL2561_Unified tsl1 = Adafruit_TSL2561_Unified(TSL2561_ADDR_HIGH, 11223);
Adafruit_TSL2561_Unified tsl2 = Adafruit_TSL2561_Unified(TSL2561_ADDR_LOW, 22334);


/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor1;
  tsl1.getSensor(&sensor1);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor1.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor1.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor1.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor1.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor1.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor1.resolution); Serial.println(" lux");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(1500);

  sensor_t sensor2;
  tsl2.getSensor(&sensor2);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor2.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor2.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor2.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor2.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor2.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor2.resolution); Serial.println(" lux");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(1500);
}

void configureSensor(void)
{
 // You can also manually set the gain or enable auto-gain support */
  // tsl1.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl1.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl1.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
//  tsl2.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
 // tsl2.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl2.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

// Changing the integration time gives you better sensor resolution (402ms = 16-bit data) 
  // tsl1.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  tsl1.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl1.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  // tsl2.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  tsl2.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl2.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */
  Serial.println("---------------tsl1-----------------");
  Serial.print  ("Gain:        "); Serial.println("Auto");
  Serial.print  ("Timing:      "); Serial.println(" 101 ms");
  Serial.println("------------------------------------");
  Serial.println("");


  Serial.println("---------------tsl2-----------------");
  Serial.print  ("Gain:        "); Serial.println("Auto");
  Serial.print  ("Timing:      "); Serial.println(" 101 ms");
  Serial.println("------------------------------------");
  Serial.println("");

}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
  Serial.begin(9600);
  Serial.println("Light Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  if (!tsl1.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561_1 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

    if (!tsl2.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561_2 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Setup the sensor gain and integration time */
  configureSensor();

  /* We're ready to go! */
  Serial.println("");
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event1;
  tsl1.getEvent(&event1);

  /* Display the results (light is measured in lux) */
  if (event1.light)
  {
    Serial.print(event1.light); Serial.println(" lux");
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
  }
  delay(250);

   /* Get a new sensor event */
  sensors_event_t event2;
  tsl1.getEvent(&event2);

  /* Display the results (light is measured in lux) */
  if (event2.light)
  {
    Serial.print(event2.light); Serial.println(" lux");
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
  }
  delay(250);
}
