#include <SparkFunTSL2561.h>
#include <Wire.h>
boolean gain1;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms1;  // Integration ("shutter") time in milliseconds
boolean gain2;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms2;  // Integration ("shutter") time in milliseconds

// TSL2561_ADDR_0 address with '0' shorted on board (0x29)
// TSL2561_ADDR   default address (0x39)
// TSL2561_ADDR_1 address with '1' shorted on board (0x49)

SFE_TSL2561 LightSensor1; // Declare a TSL2561 called "LightSensor1"
SFE_TSL2561 LightSensor2; // Declare a TSL2561 called "LightSensor2"
// SFE_TSL2561 LightSensor3; // Declare a TSL2561 called "LightSensor3"

void setup()
{
  LightSensor1.begin();     // Initialize LightSensor1 to address 0x39
  LightSensor2.begin(0x29); // Initialize LightSensor2 to address 0x29
  // LightSensor3.begin(TSL2561_ADDR_1); // Initialize LightSensor3 to address 0x49

  unsigned char ID1;

  if (LightSensor1.getID(ID1))
  {
    Serial.print("Got factory ID: 0X");
    Serial.print(ID1, HEX);
  }
  // Most library commands will return true if communications was successful,
  // and false if there was a problem. You can ignore this returned value,
  // or check whether a command worked correctly and retrieve an error code:
  else
  {
  //  byte error1 = LightSensor1.getError();
//    printError1(error1);
  }
  unsigned char ID2;

  if (LightSensor2.getID(ID2))
  {
    Serial.print("Got factory ID: 0X");
    Serial.print(ID2, HEX);
  }
  // Most library commands will return true if communications was successful,
  // and false if there was a problem. You can ignore this returned value,
  // or check whether a command worked correctly and retrieve an error code:
  else
  {
   // byte error2 = LightSensor2.getError();
  //  printError2(error2);
  }


  // The light sensor has a default integration time of 402ms,
  // and a default gain of low (1X).

  // If you would like to change either of these, you can
  // do so using the setTiming() command.

  // If gain = false (0), device is set to low gain (1X)
  // If gain = high (1), device is set to high gain (16X)

  gain1 = 0;
  gain2 = 0;


  // If time = 0, integration will be 13.7ms
  // If time = 1, integration will be 101ms
  // If time = 2, integration will be 402ms
  // If time = 3, use manual start / stop to perform your own integration

  unsigned char time = 2;

  // setTiming() will set the third parameter (ms) to the
  // requested integration time in ms (this will be useful later):

  Serial.println("Set timing...");
  LightSensor1.setTiming(gain1, time, ms1);

  Serial.println("Set timing...");
  LightSensor2.setTiming(gain2, time, ms2);

  // To start taking measurements, power up the sensor:

  Serial.println("Powerup...");
  LightSensor1.setPowerUp();
  LightSensor2.setPowerUp();

  // The sensor will now gather light during the integration time.
  // After the specified time, you can retrieve the result from the sensor.
  // Once a measurement occurs, another integration period will start.


}


void loop(){
  // Wait between measurements before retrieving the result
  // (You can also configure the sensor to issue an interrupt
  // when measurements are complete)

  // This sketch uses the TSL2561's built-in integration timer.
  // You can also perform your own manual integration timing
  // by setting "time" to 3 (manual) in setTiming(),
  // then performing a manualStart() and a manualStop() as in the below
  // commented statements:

  // ms = 1000;
  // LightSensor1.manualStart();
  delay(ms1);
  // LightSensor1.manualStop();

  // ms = 1000;
  // LightSensor2.manualStart();
  delay(ms1);
  // LightSensor2.manualStop();

  // Once integration is complete, we'll retrieve the data.

  // There are two light sensors on the device, one for visible light
  // and one for infrared. Both sensors are needed for lux calculations.

  // Retrieve the data from the device:

  unsigned int data10, data11;
  unsigned int data20, data21;

  if (LightSensor1.getData(data10, data11))
  {
    // getData() returned true, communication was successful

    Serial.print("data10: ");
    Serial.print(data10);
    Serial.print(" data11: ");
    Serial.print(data11);

    // To calculate lux, pass all your settings and readings
    // to the getLux() function.

    // The getLux() function will return 1 if the calculation
    // was successful, or 0 if one or both of the sensors was
    // saturated (too much light). If this happens, you can
    // reduce the integration time and/or gain.
    // For more information see the hookup guide at: https://learn.sparkfun.com/tutorials/getting-started-with-the-tsl2561-luminosity-sensor

    double lux1;    // Resulting lux value
    boolean good;  // True if neither sensor is saturated

    // Perform lux calculation:

    good = LightSensor1.getLux(gain1, ms1, data10, data11, lux1);

    // Print out the results:

    Serial.print(" lux: ");
    Serial.print(lux1);
    if (good) Serial.println(" (good)"); else Serial.println(" (BAD)");
  }
  else {
    // getData() returned false because of an I2C error, inform the user.

  //  byte error1 = LightSensor1.getError();
  //  printError1(error1);
  }
}

if (LightSensor2.getData(data20, data21)) {
  // getData() returned true, communication was successful

  Serial.print("data20: ");
  Serial.print(data20);
  Serial.print(" data21: ");
  Serial.print(data21);

  // To calculate lux, pass all your settings and readings
  // to the getLux() function.

  // The getLux() function will return 1 if the calculation
  // was successful, or 0 if one or both of the sensors was
  // saturated (too much light). If this happens, you can
  // reduce the integration time and/or gain.
  // For more information see the hookup guide at: https://learn.sparkfun.com/tutorials/getting-started-with-the-tsl2561-luminosity-sensor

  double lux2;    // Resulting lux value

  // Perform lux calculation:

  good = LightSensor2.getLux(gain2, ms2, data20, data21, lux2);

  // Print out the results:

  Serial.print(" lux: ");
  Serial.print(lux2);
  if (good) Serial.println(" (good)"); else Serial.println(" (BAD)");
}
else {
  // getData() returned false because of an I2C error, inform the user.

 // byte error2 = LightSensor2.getError();
//  printError2(error2);
}
}

