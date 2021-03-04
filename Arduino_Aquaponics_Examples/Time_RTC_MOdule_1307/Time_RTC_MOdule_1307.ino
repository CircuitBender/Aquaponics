// Timur Maksiomv 2014
//
// A quick demo of how to use DS1302-library to make a quick
// clock using a DS1302 and a 16x2 LCD.
//
// I assume you know how to connect the DS1302 and LCD.
// DS1302:  CE pin    -> Arduino Digital 26
//          I/O pin   -> Arduino Digital 25
//          SCLK pin  -> Arduino Digital 24
//          VCC pin   -> Arduino Digital 22
//          GND pin   -> Arduino Digital 23
//

#include <DS1302RTC.h>
#include <Time.h>

// Init the DS1302
// Set pins:  CE, IO,CLK
DS1302RTC RTC(26, 25, 24);

// Optional connection for RTC module
//#define DS1302_GND_PIN 23
//#define DS1302_VCC_PIN 22

void setup()
{
Serial.begin (9600)

  Serial.print("RTC activated");

  delay(500);

  // Check clock oscillation  
  if (RTC.haltRTC())
    Serial.print("Clock stopped!");
  else
    Serial.print("Clock working.");

  delay ( 2000 );

  // Setup Time library  
  Serial.print("RTC Sync");
  setSyncProvider(RTC.get); // the function to get the time from the RTC
  if(timeStatus() == timeSet)
    Serial.print(" Ok!");
  else
    Serial.print(" FAIL!");

  delay ( 2000 );

}

void loop()
{

  // Display time centered on the upper line
  Serial.print (hour())
  Serial.print(minute());
  Serial.print(second());

  // Display abbreviated Day-of-Week in the lower left corner


  // Display date in the lower right corner
  Serial.print(" ");
  Serial.print(day());
  Serial.print("/");
 Serial.print(month());
  Serial.print("/");
  Serial.print(year());

  // Warning!
  if(timeStatus() != timeSet) {
    Serial.print(F("RTC ERROR: SYNC!"));
  }

  delay ( 1000 ); // Wait approx 1 sec
};


