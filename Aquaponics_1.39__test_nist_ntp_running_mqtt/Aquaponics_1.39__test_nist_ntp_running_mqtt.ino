/* <<<<<<<<<<<< Aquaponics 1.38 by Marco Heinzen >>>>>>>>>>>>>>>>>>>
      _ _
     _{ ' }_
    { `.!.` }
    ',_/Y\_,'
      {_,_}
        |
      (\|  /)
       \| //
        |//
     \\ |/  //
  ^^^^^^^^^^^^^^^

     |\   \\\\__     o
     |\_/    o \o    o
     |_   (( <==> oo
     |/ \__+___/
     /     |/
  -----------------------
*/

// ++++++++++++++ Libraries used: +++++++++++++++++


#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <Dhcp.h>
#include <Dns.h>

#include <Wire.h>
#include <Digital_Light_TSL2561.h>

#include <Time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "High_Temp.h"



// Grove - High Temperature Sensor
// Thmc -> A3
// RoomTemp -> A2
// Thmc -> A5
// RoomTemp -> A4
HighTemp ht1(A2, A3);
HighTemp ht2(A4, A5);

// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// Depends on the following Arduino libraries:
// - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
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


//  User Defined Variables
// ALARMS NOOOOOO ZEEEEROOOOOOS !!!! infront of single hour/min/sec
uint32_t LightsMorningONhour = 9; uint32_t LightsMorningONminute = 1; uint32_t LightsMorningONsec = 3;
uint32_t LightsMorningOFFhour = 15; uint32_t LightsMorningOFFminute = 1; uint32_t LightsMorningOFFsec = 3;
uint32_t LightsEveningONhour = 16; uint32_t LightsEveningONminute = 1; uint32_t LightsEveningONsec = 3;
uint32_t LightsEveningOFFhour = 22; uint32_t LightsEveningOFFminute = 45; uint32_t LightsEveningOFFsec = 3;

// Lights ON Morning
uint32_t TimeLightONhour = (LightsMorningONhour) * 3600000L;
uint32_t TimeLightONminute = (LightsMorningONminute) * 60000L;
uint32_t TimeLightONsec = (LightsMorningONsec) * 1000L;
uint32_t TimeON = (TimeLightONhour + TimeLightONminute + TimeLightONsec) * 1L;

// Midday
uint32_t TimeRainONhour = (LightsMorningOFFhour) * 3600000L;
uint32_t TimeRainONminute = (LightsMorningOFFminute) * 60000L;
uint32_t TimeRainONsec = (LightsMorningOFFsec) * 1000L;
uint32_t TimeRainON = (TimeRainONhour + TimeRainONminute + TimeRainONsec);

// AFternoon
uint32_t TimeRainOFFhour = (LightsEveningONhour) * 3600000L;
uint32_t TimeRainOFFminute = (LightsEveningONminute) * 60000L;
uint32_t TimeRainOFFsec = (LightsEveningONsec) * 1000L;
uint32_t TimeRainOFF = (TimeRainOFFhour + TimeRainOFFminute + TimeRainOFFsec);

// Lights OFF Evening
uint32_t TimeLightOFFhour = (LightsEveningOFFhour) * 3600000L;
uint32_t TimeLightOFFminute = (LightsEveningOFFminute) * 60000L;
uint32_t TimeLightOFFsec = (LightsEveningOFFsec) * 1000L;
uint32_t TimeOFF = (TimeLightOFFhour + TimeLightOFFminute + TimeLightOFFsec) * 1L;

uint32_t TimeNOWhour = 0L;
uint32_t TimeNOWminute = 0L;
uint32_t TimeNOWsec = 0L;
uint32_t TimeNOW = 0L;

uint32_t TimeState = 0;

// ++++++++++++++++++ CYCLE TIMES ++++++++++++++++++++
uint32_t WateringTime = 22023L;
uint32_t FlowbackTime = 76019L;
uint32_t CycleTime = (5 * 60 * 1000L);
uint32_t CycleTimeHigh = (180 * 1000L);
uint32_t NTPupdateTime = (4 * 60 * 1000L);
uint32_t SensorCycle = 10013;
uint32_t manualUpdate = 10000;
uint32_t FeedingQuantity = 1200;
// uint32_t FeedingCycle = (11 * 60 * 1000L);
uint32_t feeder = 0;
//  intervals for updating Display
uint32_t intervallUpdate = 1000;

// each "event" gets its own tracking variable
uint32_t previousMillisUpdate1 = 0;
uint32_t previousMillisUpdate2 = 0;
uint32_t previousMillisUpdate3 = 0;
uint32_t previousMillisUpdate4 = 0;
uint32_t previousMillisUpdate5 = 0;
uint32_t previousMillisWatering = 0;
uint32_t previousMillisafterWatering = 0;
uint32_t previousMillisFeeding = 0;

//   ** Water Sensor  **

// +++++++++++++++++ Network Setup +++++++++++++++++
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xF7
};
IPAddress ip(192, 168, 50, 109);
IPAddress myDns(192, 168, 50, 1);
IPAddress gateway(192, 168, 50, 1);
IPAddress subnet(255, 255, 255, 0);


void callback1(char* topic, byte* payload, uint32_t length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (uint32_t i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  /*
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
      // but actually the LED is on; this is because
      // it is acive low on the ESP-01)
    } else {
      digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }
  */
}

EthernetClient ethClient;

// ++++++++++++++++ Time +++++++++++++++++++++
// NTP Servers:
IPAddress timeServer1(129, 6, 15, 28); // time-a.timefreq.bldrdoc.gov
IPAddress timeServer2(132, 163, 96, 2); // time-b.timefreq.bldrdoc.gov
IPAddress timeServer3(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov
/*ETH: 130.60.128.3
  130.60.64.51
  130.60.204.10 */

EthernetUDP Udp;
uint32_t localPort = 8888;  // local port to listen for UDP packets

// Timezones
uint32_t timeZone = 2;     // Central European Time
//uint32_t timeZone = -5;  // Eastern Standard Time (USA)
time_t prevDisplay = 0; // when the digital clock was displayed

//setting up Ethernet
void setup_ethernet() {
  Alarm.delay(10);
  ethernet_setup();
  Alarm.delay(10);
}


// Ultra Sonic Range Sensor
#define ECHOPIN 42        // Pin to receive echo pulse 
#define TRIGPIN 43        // Pin to send trigger pulse
String usrsdiststr;
String usrswlstr;
String usrswvstr;
String usrswlistr;
char usrsdistance[50];
char usrswl[50];
char usrswv[50];
char usrswli[50];



// +++++++++++++++++++ RELAYS +++++++++++++++++++++
#define PumpeA_Relais1 30 // Pumpe Aquarium auf Relais 1
#define LichtA1_Relais2 31   // Pumpe Kreislauf auf Relais 2
#define LichtA2_Relais3 32    // Licht Weiss Aquarium auf Relais 3
#define PumpeK_Relais4 33    // Licht Rot Aquarium auf Relais 4

#define HeizstabA_Relais5 34  // Heizstab Aquarium auf Relais 5
#define LichtGrow1_Relais6 35  // Licht Grow 1 30W
#define LichtGrow2_Relais7 36  // Licht Grow 2 30W
#define Fishfeeder 37  // 3.3v Fishfeeder Relay

// Ventilation
#define Vent_Mosfet_Tech1OUT_Pin1 5
#define Vent_Mosfet_Tech2IN_Pin2 6
#define Vent_Mosfet_Tech3OUT_Pin3 7
#define Vent_Mosfet_Grow1IN_Pin4 8
#define Vent_Mosfet_Grow2OUT_Pin5 9
#define Vent_Mosfet_Aqua1_Pin6 11
#define Vent_Mosfet_Aqua2_Pin7 12

//  ** Temp Probe Related          //

//  ** Thermometer with thermistor  **

/*thermistor parameters:
   RT0: 10 000 Ω
   B: 3950 K +- 1.00%
   T0:  25 C
   +- 2%
*/

//These values are in the datasheet
uint32_t RT0 = 10000L;   // Ω
uint32_t B = 3950;      // K
//--------------------------------------


uint32_t VCC = 5;    //Supply voltage
uint32_t R = 10000;  //R=10KΩ

//Variables ------------------
float RT1;
float RT2;
float VR1;
float VR2;
float ln;
float TX1;
float TX2;
float T0;
float VRT1;
float VRT2;

// mqtt Variables for printing temp and humidity
String air_str; //see last code block below use these to convert the float that you get back from DHT to a string =str
String water_str;
char airtemp[50];
char watertemp[50];

//##################################################################################
//-----------  Do not Replace R1 with a resistor lower than 300 ohms    ------------
//##################################################################################
// Setup EC Meter

uint32_t R_1 = 1000;
uint32_t R_2 = 1000;

uint32_t Ra1 = 25; //Resistance of powering Pins
uint32_t Ra2 = 25; //Resistance of powering Pins

uint32_t EC1Pin = A2;
uint32_t EC1Power = A3;
uint32_t EC1Ground = A4;

uint32_t EC2Pin = A5;
uint32_t EC2Ground = A7;
uint32_t EC2Power = A6;

//  * Converting to ppm [Learn to use EC it is much better  ****//
// Hana      [USA]        PPMconverion:  0.5
// Eutech    [EU]          PPMconversion:  0.64
//Tranchen  [Australia]  PPMconversion:  0.7
// Why didnt anyone standardise this?

float PPMconversion = 0.64;

//  ***Compensating for temperature        *//
//The value below will change depending on what chemical solution we are measuring
//0.019 is generaly considered the standard for plant nutrients [google "Temperature compensation EC" for more info
float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring

//    ** Cell Constant For Ec Measurements     *//
//Mine was around 2.9 with plugs being a standard size they should all be around the same
//But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
float K1 = 2.88;
float K2 = 2.88;


//     **** END Of Recomended User Inputs              //


float AirTemperature = 0;
float WaterTemperature = 0;
float EC1 = 0;
float EC251 = 0;
uint32_t ppm1 = 0;
float EC2 = 0;
float EC252 = 0;
uint32_t ppm2 = 0;


float raw1 = 0;
float Vin1 = 5;
float Vdrop1 = 0;
float Rc1 = 0;

float raw2 = 0;
float Vin2 = 5;
float Vdrop2 = 0;
float Rc2 = 0;


// +++++++++++++++++++++++++++ END OF VARIABLES ++++++++++++++++++++++++
// voids for Task Scheduler
// ++++++++++++++++++++TIMER EVENTS++++++++++++++++++++++++++++


#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_PRIORITY
#define _TASK_WDT_IDS
#define _TASK_TIMECRITICAL
#include <TaskScheduler.h>


// Callback methods prototypes
void Sensors();
void Watering();
void getTime();
void displayTime();
void Feeding();

// Tasks

Scheduler r, hpr, cpr;
Task t1(SensorCycle, TASK_FOREVER, &Sensors, &r);
Task t2(intervallUpdate, TASK_FOREVER, &displayTime, &r);
Task t3(NTPupdateTime, TASK_FOREVER, &getTime, &r);
Task t4(CycleTime, TASK_FOREVER, &Watering, &hpr);
// Task t5(FeedingCycle, TASK_FOREVER, &Feeding, &r);

void displayTime() {
  digitalClockDisplay();
}

void Feeding() {
  fishfeeder();
 
}

void Sensors() {
  readSensors();
}

void Watering() {

  abc();
  time_now();
  timestate();
  timestate_switch();
switch (TimeState) {
    case 1:
fishfeeder();
break;
    
    case 3:
fishfeeder();
      break;
}
}

void getTime() {
  getNtpTime1();
}

void setup_relays() {
  // initialize Relais 1-4 as Output
  Serial.println(" ");
  Serial.println("initializing RELAYS");
  Serial.println("defining Outputs...");

  pinMode(PumpeA_Relais1, OUTPUT);
  pinMode(LichtA1_Relais2, OUTPUT);
  pinMode(LichtA2_Relais3, OUTPUT);
  pinMode(PumpeK_Relais4, OUTPUT);
  pinMode(HeizstabA_Relais5, OUTPUT);
  pinMode(LichtGrow1_Relais6, OUTPUT);
  pinMode(LichtGrow2_Relais7, OUTPUT);

  pinMode(Vent_Mosfet_Tech1OUT_Pin1, OUTPUT);
  pinMode(Vent_Mosfet_Tech2IN_Pin2, OUTPUT);
  pinMode(Vent_Mosfet_Tech3OUT_Pin3, OUTPUT);
  pinMode(Vent_Mosfet_Grow1IN_Pin4, OUTPUT);
  pinMode(Vent_Mosfet_Grow2OUT_Pin5, OUTPUT);
  pinMode(Vent_Mosfet_Aqua1_Pin6, OUTPUT);
  pinMode(Vent_Mosfet_Aqua2_Pin7, OUTPUT);


  Serial.println(" ");

  Serial.println("RELAYS");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: LOW");
  digitalWrite(LichtA1_Relais2, LOW);
  Serial.println("setting LichtA1_Relais2: LOW");
  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: LOW");
  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais4: LOW");
  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais5: HIGH");
  Serial.println("");
  digitalWrite(LichtGrow1_Relais6, LOW);
  Serial.println("setting LichtGrow1_Relais6: LOW");
  Serial.println("");
  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: LOW");
  Serial.println("");

  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 255);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 255);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 255);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 0);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 0);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 0);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);

}

void setup_alarms() {
  // ALARM TIMER RELAIS
  // LIGHTS
  Alarm.alarmRepeat(LightsMorningONhour, LightsMorningONminute, LightsMorningONsec, morning_setup); // Morning Aquarium Lights ON every day
  Alarm.alarmRepeat(LightsMorningOFFhour, LightsMorningOFFminute, LightsMorningOFFsec, afternoon_setup); // Midday Aquarium Lights OFF every day
  Alarm.alarmRepeat(LightsEveningONhour, LightsEveningONminute, LightsEveningONsec, evening_setup); // Afternoon Aquarium Lights ON every day
  Alarm.alarmRepeat(LightsEveningOFFhour, LightsEveningONminute, LightsEveningOFFsec, night_setup); // Evening Aquarium Lights OFF every day
  Alarm.delay(11);
  Serial.print("Lights in the morning ON from ");
  Serial.print(LightsMorningONhour);
  Serial.print(":");
  Serial.print(LightsMorningONminute);
  Serial.print(":");
  Serial.print(LightsMorningONsec);
  Serial.print(" to ");
  Serial.print(LightsMorningOFFhour);
  Serial.print(":");
  Serial.print(LightsMorningOFFminute);
  Serial.print(":");
  Serial.println(LightsMorningOFFsec);
  Serial.print("Lights in the evening ON from ");
  Serial.print(LightsEveningONhour);
  Serial.print(":");
  Serial.print(LightsEveningONminute);
  Serial.print(":");
  Serial.print(LightsEveningONsec);
  Serial.print(" to ");
  Serial.print(LightsEveningOFFhour);
  Serial.print(":");
  Serial.print(LightsEveningOFFminute);
  Serial.print(":");
  Serial.println(LightsEveningOFFsec);
  Serial.println("");
  Alarm.delay(11);
}
void setup_fishfeeder () {
  Serial.println("Fishfeeder Setup");

  pinMode(Fishfeeder, OUTPUT);
  digitalWrite(Fishfeeder, LOW);
  digitalWrite(Fishfeeder, HIGH);
  Alarm.delay(500);
  digitalWrite(Fishfeeder, LOW);
  Serial.println("Fishfeeder OK");



}
void setup_dht11() {
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

void setup_groveTemp() {
  Serial.println("grove - high temperature sensor  1");
  Serial.println("");

  ht1.begin();
  Serial.println("grove - high temperature sensor  2");

  ht2.begin();
  Serial.println("");
}

void setup_thermistor() {
  // Temperature Module
  T0 = 25 + 273.15;                 //Temperature T0 from datasheet, conversion from Celsius to kelvin
  VRT1 = analogRead(A0);              //Acquisition analog value of VRT
  VRT2 = analogRead(A1);              //Acquisition analog value of VRT
}
void setup_ec() {
  // EC Modules
  //EC1
  pinMode(EC1Pin, INPUT);
  pinMode(EC1Power, OUTPUT); //Setting pin for sourcing current
  pinMode(EC1Ground, OUTPUT); //setting pin for sinking current
  digitalWrite(EC1Ground, LOW); //We can leave the ground connected permanantly
  //EC2
  pinMode(EC2Pin, INPUT);
  pinMode(EC2Power, OUTPUT); //Setting pin for sourcing current
  pinMode(EC2Ground, OUTPUT); //setting pin for sinking current
  digitalWrite(EC2Ground, LOW); //We can leave the ground connected permanantly


  //** Adding Digital Pin Resistance to [25 ohm] to the static Resistor  ****//
  // Consule Read-Me for Why, or just accept it as true
  R_1 = (R_1 + Ra1); // Taking into acount Powering Pin Resitance
  R_2 = (R_2 + Ra2); // Taking into acount Powering Pin Resitance
}
void setup_usrs() {
  // Ultra Sonic Range Sensor
  Serial.println("initializing Ultra Sonic Range Sensor");
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  Serial.println("");
}

void logo() {
  Serial.println("Aquaponics 1.21");
  Serial.println("by Marco Heinzen © 2017");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println("_________________________________");
  Serial.println("|        _ _      ---O---       |");
  Serial.println("|      _{ ' }_      /|          |");
  Serial.println("|     { `.!.` }    / |          |");
  Serial.println("|     ',_/Y\_,'                 |");
  Serial.println("|       {_,_}                   |");
  Serial.println("|         |                     |");
  Serial.println("|       (\|  /)                 |");
  Serial.println("|        \| //                  |");
  Serial.println("|         |//                   |");
  Serial.println("|      \\ |/  //                |");
  Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
  Serial.println("|                               |");
  Serial.println("|   |\   \\\\__     o           |");
  Serial.println("|   |\_/    o \o    o           |");
  Serial.println("|   |_   (( <==> oo             |");
  Serial.println("|   |/ \__+___/                 |");
  Serial.println("|   /     |/                    |");
  Serial.println("--------------------------------");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
}

void time_now() {
  // Time Now
  TimeNOWhour = (hour()) * 3600000L;
  TimeNOWminute = (minute()) * 60000L;
  TimeNOWsec = (second()) * 1000L;
  TimeNOW = (TimeNOWhour + TimeNOWminute + TimeNOWsec);

}
void timestate() {
  if ((TimeON <= TimeNOW && TimeNOW < TimeRainON)) {
    TimeState = 1;
    Serial.println("It is Morning");
  }
  if ( TimeRainON <= TimeNOW && TimeNOW < TimeRainOFF) {
    TimeState = 2;
    Serial.println("It is Afternoon");
  }
  if (TimeNOW >= TimeRainOFF && TimeNOW < TimeOFF ) {
    TimeState = 3;
    Serial.println("It is Evening");
  }
  if (TimeOFF <= TimeNOW || TimeNOW < TimeON) {
    TimeState = 4;
    Serial.println("It is Night");
  }
}

void timestate_switch() {
  switch (TimeState) {
    case 1:
      morning_setup();
      break;
    case 2:
      afternoon_setup();
      break;
    case 3:
      evening_setup();
      break;
    case 4:
      night_setup();
      break;
    default:
      default_setup();
      break;
  }
}

void morning_setup() {

  // set Relays to standard startup

  Serial.println("initializing morning_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: LOW /ON");
  digitalWrite(LichtA1_Relais2, HIGH);
  Serial.println("setting LichtA1_Relais2: HIGH");
  digitalWrite(LichtA2_Relais3, HIGH);
  Serial.println("setting LichtA2_Relais3: HIGH");
  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais4: LOW");
  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais5: HIGH");
  Serial.println("");
  digitalWrite(LichtGrow1_Relais6, HIGH);
  Serial.println("setting LichtGrow1_Relais6: HIGH");
  Serial.println("");
  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: LOW");
  Serial.println("");

  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 255);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 255);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 255);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 255);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 255);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 0);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);

  t4.setId(40);
//  t5.setId(50);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well

  t1.enable();
  r.addTask(t1);
  t2.enable();
  r.addTask(t2);
  t3.enable();
  r.addTask(t3);
  t4.enable();
  r.addTask(t4);
  t4.setInterval(CycleTime);
  Serial.println("WATERING IS ON");
 /* t5.enable();
  r.addTask(t5);
  t5.setInterval(FeedingCycle);
  Serial.println("Fishfeeder is ON");*/
}
void afternoon_setup() {
  // set Relays to standard startup

  Serial.println("initializing afternoon_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: LOW / ON");
  digitalWrite(LichtA1_Relais2, LOW);
  Serial.println("setting LichtA1_Relais2: LOW");
  digitalWrite(LichtA2_Relais3, HIGH);
  Serial.println("setting LichtA2_Relais3: HIGH");
  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais4: LOW");
  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais5: HIGH");
  Serial.println("");
  digitalWrite(LichtGrow1_Relais6, HIGH);
  Serial.println("setting LichtGrow1_Relais6: HIGH");
  Serial.println("");
  digitalWrite(LichtGrow2_Relais7, HIGH);
  Serial.println("setting LichtGrow2_Relais7: HIGH");
  Serial.println("");

  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 255);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 255);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 255);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 255);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 255);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 0);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);


  t4.setId(40);
 //   t5.setId(50);

  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well

  t1.enable();
  r.addTask(t1);
  t2.enable();
  r.addTask(t2);
  t3.enable();
  r.addTask(t3);
  t4.enable();
  r.addTask(t4);
  Serial.println("RAIN IS ON");
  t4.setInterval(CycleTimeHigh);
  /*t5.enable();
  r.addTask(t5);
  t5.setInterval(FeedingCycle);
  Serial.println("Fishfeeder is ON");*/
}

void evening_setup() {

  // set Relays to standard startup
  Serial.println("initializing evening_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: LOW / ON");
  digitalWrite(LichtA1_Relais2, HIGH);
  Serial.println("setting LichtA1_Relais2: HIGH");
  digitalWrite(LichtA2_Relais3, HIGH);
  Serial.println("setting LichtA2_Relais3: HIGH");
  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais4: LOW");
  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais5: HIGH");
  Serial.println("");
  digitalWrite(LichtGrow1_Relais6, HIGH);
  Serial.println("setting LichtGrow1_Relais6: HIGH");
  Serial.println("");
  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: LOW");
  Serial.println("");

  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 255);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 255);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 255);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 255);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 255);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 0);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);



  t4.setId(40);
//    t5.setId(50);

  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well

  t1.enable();
  r.addTask(t1);
  t2.enable();
  r.addTask(t2);
  t3.enable();
  r.addTask(t3);
  t4.enable();
  r.addTask(t4);
  t4.setInterval(CycleTime);
  Serial.println("WATERING IS ON");
  /*t5.enable();
  r.addTask(t5);
  t5.setInterval(FeedingCycle);
  Serial.println("Fishfeeder is ON");*/
}

void night_setup() {

  // set Relays to standard startup
  Serial.println("initializing night_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: LOW / ON");
  digitalWrite(LichtA1_Relais2, LOW);
  Serial.println("setting LichtA1_Relais2: LOW");
  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: LOW");
  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais4: LOW");
  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais5: HIGH");
  Serial.println("");
  digitalWrite(LichtGrow1_Relais6, LOW);
  Serial.println("setting LichtGrow1_Relais6: LOW");
  Serial.println("");
  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: LOW");
  Serial.println("");

  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 255);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 0);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 255);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 0);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 0);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 0);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);


  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well

//  t5.disable();
//  r.deleteTask(t5);
  t4.disable();
  r.deleteTask(t4);
  r.addTask(t1);
  t1.enable();
  r.addTask(t2);
  t2.enable();
  r.addTask(t3);
  t3.enable();
  Serial.println("WATERING IS OFF");

}
void default_setup() {

  // set Relays to standard startup
  Serial.println("initializing default_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: LOW / ON");
  digitalWrite(LichtA1_Relais2, LOW);
  Serial.println("setting LichtA1_Relais2: LOW");
  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: LOW");
  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais4: LOW");
  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais5: HIGH");
  Serial.println("");
  digitalWrite(LichtGrow1_Relais6, LOW);
  Serial.println("setting LichtGrow1_Relais6: LOW");
  Serial.println("");
  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: LOW");
  Serial.println("");

  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 255);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 255);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 255);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 0);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 255);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 0);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);

 // fishfeeder();

  t4.setId(40);
//    t5.setId(50);

  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well

  t1.enable();
  r.addTask(t1);
  t2.enable();
  r.addTask(t2);
  t3.enable();
  r.addTask(t3);
  t4.setInterval(CycleTime);
  t4.disable();
  r.deleteTask(t4);
  Serial.println("WATERING IS OFF");
  /*t5.enable();
  r.addTask(t4);
  t5.setInterval(FeedingCycle);
  Serial.println("Fishfeeder is ON");*/
}

void setup () {
  Serial.begin(9600);
  Serial.println("Initializing Setup:");
  Serial.println("loading ... ");
  Serial.println(" ");

  logo();
  Wire.begin();
  setup_ethernet();
  setup_relays();
  setup_alarms();

  Serial.println("Initializing Sensors");
 // TSL2561.init();
  setup_dht11();
  setup_groveTemp();
  setup_thermistor();
  setup_ec();
  setup_usrs();

  Serial.println("Check Time State");
  time_now();
  timestate();
  timestate_switch();
  setup_fishfeeder ();

  Serial.println("SUCCESS !");
  Serial.println("End of Setup !");
  Serial.println("Starting Main Loop:");
  Serial.println("");
  Serial.println("");
}

// END OF SETUP

// ++++++++++++ MAIN LOOP ++++++++++++



void loop () {

  r.execute();

  Alarm.delay(1);
}


//  END OF MAIN LOOP
// SUBROUTINES
// EVENT ROUTINE
void abc () {
  Scheduler &s = Scheduler::currentScheduler();
  Task &t = s.currentTask();

  Serial.print("It is ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println("");
  Serial.print("Watering Plants for: ");
  Serial.print (WateringTime / 1000);
  Serial.println ("  Seconds");
  digitalWrite(PumpeA_Relais1, HIGH);
  digitalWrite(PumpeK_Relais4, HIGH);
  Water_Detection();
  uint32_t x = 0;
  do {
    unsigned long currentMillisWatering = millis();
    if ((unsigned long)(currentMillisWatering - previousMillisWatering) >= intervallUpdate) {

      digitalClockDisplay();
      Water_Detection();
      UltraSonic();
      PrintECReadings();
      x++;
      previousMillisWatering = currentMillisWatering;
    }
  }
  while (x < (WateringTime / ((unsigned)1000)));

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("Watering Plants STOP");
  Serial.print("Watered Plants for: ");
  Serial.print (WateringTime / ((unsigned)1000));
  Serial.println ("  Seconds");
  Serial.print("Waiting: ");
  Serial.print (FlowbackTime / ((unsigned)1000));
  Serial.println ("  Seconds to let Water Flow back");
  uint32_t y = 0;
  do {
    unsigned long currentMillisafterWatering = millis();
    if ((unsigned long)(currentMillisafterWatering - previousMillisafterWatering) >= intervallUpdate) {

      digitalClockDisplay();
      Water_Detection();
      UltraSonic();
      PrintECReadings();
      y++;
      previousMillisafterWatering = currentMillisafterWatering;
    }
  }
  while (y < (FlowbackTime / (unsigned)1000));

  Serial.print("Waited: ");
  Serial.print (FlowbackTime / (unsigned)1000);
  Serial.println ("  Seconds to let Water Flow back");
  Serial.println("Turning Aquarium Pump PumpeA_Relais1 ON ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("Aquarium Pump PumpeA_Relais1 is: LOW / ON ");
  Serial.print("Wating for:");
  Serial.print((CycleTime - WateringTime - FlowbackTime)  / (unsigned)1000);
  Serial.println(" Seconds to activate the next Cycle");
  // Time in ms

}

// AFTER EVENT ROUTINE

// +++++++++++++++++++++Sensors Sub++++++++++++++++++++++++++++++++++
// read sensors
void readSensors() {

  UltraSonic();
  LightSensor();
  dht11();
  getGroveWaterTemp();
  getGroveSoilTemp();
  GetWaterTEMP();
  GetAirTEMP();
  PrintECReadings();
  Water_Detection();
  Water_Temp_Regulation();
  Alarm.delay(11);

}
void Water_Temp_Regulation(){
  if (WaterTemperature <= 22){
 analogWrite(Vent_Mosfet_Aqua1_Pin6, 0);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);
  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais5: HIGH");
  }
    if (WaterTemperature > 25){
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 255);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);
  digitalWrite(HeizstabA_Relais5, LOW);
  Serial.println("setting HeizstabA_Relais5: LOW");
  }
     if (WaterTemperature > 27){
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 255);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 255);
  digitalWrite(HeizstabA_Relais5, LOW);
  Serial.println("setting HeizstabA_Relais5: LOW");
  }
}
void fishfeeder() {
  Scheduler &s = Scheduler::currentScheduler();
  Task &t = s.currentTask();
   Serial.println(feeder);
   Serial.println(" feeder Variable");
  int f = feeder;
   Serial.println(f / 3);
      Serial.println(" feeder Variable / 3");

if (f / 3 != 0) {
  

  Serial.println(" Fishfeeder ON");
/*
uint32_t q = 0;

  do {
    unsigned long currentMillisFeeding = millis();
    if ((unsigned long)(currentMillisFeeding - previousMillisFeeding) >= FeedingQuantity) {

      digitalWrite(Fishfeeder, HIGH);
      q++;
      previousMillisFeeding = currentMillisFeeding;
    }
  }
  while (q * 100 < ((FeedingQuantity) / 10));
 */  
  digitalWrite(Fishfeeder, HIGH);
  Alarm.delay(FeedingQuantity); 
  digitalWrite(Fishfeeder, LOW);
  Serial.println(" Fishfeeder OFF");
  Alarm.delay(15);
}
else {
 Serial.println("Fishfeeder Skipped");
}
  feeder++;



}
// Ultra Sonic Range Sensor Subroutine // Waterlevel Measurement
void UltraSonic() {
  // Start Ranging -Generating a trigger of 10us burst
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  // Distance Calculation

  float distance = pulseIn(ECHOPIN, HIGH);
  distance = distance / 58;
  float WaterLevel = 0;
  WaterLevel = (unsigned)38 - distance;
  float WaterVolume = 0;
  WaterVolume = WaterLevel * (unsigned)2100L;
  float WaterLitres = 0;
  WaterLitres = (unsigned)WaterVolume / 1000;

  /* The speed of sound is 340 m/s or 29 us per cm.The Ultrasonic burst travels out & back.So to find the distance of object we divide by 58  */

  if (distance <= 8) {
    Serial.print(WaterLevel);
    Serial.println(" cm Wasserstand ALARM !!");

    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(WaterLevel);
    Serial.println(" cm Wasserstand ALARM !!");
    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(WaterLevel);
    Serial.println(" cm Wasserstand ALARM !!");
    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(WaterLevel);

    Serial.println(" cm Wasserstand ALARM !!");
    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(WaterLevel);
    Serial.println(" cm Wasserstand ALARM !!");
    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(WaterLevel);
    Serial.println(" cm Wasserstand ALARM !!");

    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(distance);
    Serial.println(" cm von Decke (9 cm MAX)");
  }
  else {
    Serial.print(WaterLevel);
    Serial.print(" cm Waterlevel,  ");

    Serial.print(distance);
    Serial.print(" cm Sensor Distance,  ");


    Serial.print(WaterVolume);
    Serial.print(" cm^2 (60900 cm^2 MAX)");

    Serial.print(WaterLitres);
    Serial.println(" l Liter Wasser (");



  
  }
}



// DHT11
void dht11() {
  // Delay between measurements.
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

// TSL2561 Light Sensor
void LightSensor() {
/*  // Light Sensor
  Serial.print(TSL2561.readVisibleLux());
  Serial.println(" Lux (Light Value TSL2561)");
  float LV = TSL2561.readVisibleLux();
  char buffer[10];
  dtostrf(LV, 0, 0, buffer);
  client.publish("Aquaponics/Sensors/TSL2561", buffer);
*/
}

//Thermistor Water Temp
void GetWaterTEMP() {
  VRT1 = analogRead(A0);              //Acquisition analog value of VRT
  VRT1 = (5.00 / 1023.00) * VRT1;      //Conversion to voltage
  VR1 = VCC - VRT1;
  RT1 = VRT1 / (VR1 / R);               //Resistance of RT
  ln = log(RT1 / RT0);
  TX1 = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor
  TX1 = TX1 - 273.15;                 //Conversion to Celsius
  float WaterTemperature = TX1 - 0; // Calibration
  uint32_t WaTemp = TX1 - 21.5;
  Serial.print(WaterTemperature);
  Serial.println(" °C (Thermistor Water Temperature) ");
};

//Thermistor Air Temp
void GetAirTEMP() {
  VRT2 = analogRead(A1);              //Acquisition analog value of VRT
  VRT2 = (5.00 / 1023.00) * VRT2;      //Conversion to voltage
  VR2 = VCC - VRT2;
  RT2 = VRT2 / (VR2 / R);               //Resistance of RT

  ln = log(RT2 / RT0);
  TX2 = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor

  TX2 = TX2 - 273.15;                 //Conversion to Celsius
  float  AirTemperature = TX2 - 0;
  Serial.print(AirTemperature);
  Serial.println(" °C (Thermistor Air Temperature)");
};

void getGroveSoilTemp(){
  Serial.println("GroveHighTempSensor1: Soil Temp: ");
  Serial.println(ht1.getThmc());
  Serial.print("GroveHighTempSensor1: Room Temp: ");

  Serial.print(ht1.getRoomTmp());
  Serial.println(" °C");
}

void getGroveWaterTemp(){
  Serial.print("GroveHighTempSensor2: Water Temp: ");
  Serial.println(ht2.getThmc());
  Serial.print("GroveHighTempSensor2: Room Temp: ");
  Serial.print(ht2.getRoomTmp());
  Serial.println(" °C");

}

// Water_Detection
void Water_Detection () {
  int water_detection1 = 0;
  int water_detection2 = 0;

  water_detection1 = analogRead(A12);
  water_detection2 = analogRead(A13);

  Serial.print("Water Detection 1 (GROWBED): ");
  Serial.print(water_detection1);
  Serial.println(" (0-1024)");
  Serial.print("Water Detection 2 (FLOOR): ");
  Serial.print(water_detection2);
  Serial.println(" (0-1024)");

  if (water_detection1 > 100) {
    digitalWrite(PumpeA_Relais1, LOW);
    digitalWrite(PumpeK_Relais4, LOW);
    Serial.println(water_detection1);
    Serial.println("ALARM ! WASSER ! GROW !");


  }
  if (water_detection2 > 100) {
    digitalWrite(PumpeA_Relais1, LOW);
    digitalWrite(PumpeK_Relais4, LOW);
    Serial.println(water_detection1);
    Serial.println("ALARM ! WASSER ! BODEN !");
  }

}

//EC
void GetEC() {

  // EC1
  // Estimates Resistance of Liquid
  digitalWrite(EC1Power, HIGH);
  raw1 = analogRead(EC1Pin);
  raw1 = analogRead(EC1Pin); // This is not a mistake, First reading will be low beause if charged a capacitor
  digitalWrite(EC1Power, LOW);

  // Converts to EC
  Vdrop1 = (Vin1 * raw1) / 1024.0;
  Rc1 = (Vdrop1 * R_1) / (Vin1 - Vdrop1);
  Rc1 = Rc1 - Ra1; //acounting for Digital Pin Resitance
  EC1 = 1000 / (Rc1 * K1);

  // Compensating For Temperaure
  EC251  =  EC1 / (1 + TemperatureCoef * (WaterTemperature - 25.0));
  ppm1 = (EC251) * (PPMconversion * 1000);

  // EC2
  // Estimates Resistance of Liquid
  digitalWrite(EC2Power, HIGH);
  raw2 = analogRead(EC2Pin);
  raw2 = analogRead(EC2Pin); // This is not a mistake, First reading will be low beause if charged a capacitor
  digitalWrite(EC2Power, LOW);

  // Converts to EC
  Vdrop2 = (Vin2 * raw2) / 1024.0;
  Rc2 = (Vdrop2 * R_2) / (Vin2 - Vdrop2);
  Rc2 = Rc2 - Ra2; //acounting for Digital Pin Resitance
  EC2 = 1000 / (Rc2 * K2);

  // Compensating For Temperaure
  EC252  =  EC2 / (1 + TemperatureCoef * (WaterTemperature - 25.0));
  ppm2 = (EC252) * (PPMconversion * 1000);


}
void PrintECReadings() {
  GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
  Serial.print("WATER EC Sensor - ");
  Serial.print("Rc1: ");
  Serial.print(Rc1);
  Serial.print(" EC1: ");
  Serial.print(EC251);
  Serial.println(" Siemens (SI)  ");
  Serial.print("ppm1: ");
  Serial.print(ppm1);
  Serial.print(" at ");
  GetWaterTEMP();
  Serial.println("SOIL EC Sensor - ");
  Serial.print("Rc2: ");
  Serial.print(Rc2);
  Serial.print(" EC2: ");
  Serial.print(EC252);
  Serial.println(" Siemens (SI)  ");
  Serial.print("ppm2: ");
  Serial.print(ppm2);
  Serial.print(" at ");
  GetWaterTEMP();


  /*
    //   Usued for Debugging
    Serial.print("Vdrop: ");
    Serial.println(Vdrop);
    Serial.print("Rc: ");
    Serial.println(Rc);
    Serial.print(EC);
    Serial.println("Siemens");
    //   end of Debugging Prints
  */
};

// DIGITAL CLOCK AND NTP SUBROUTINES
void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" / ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
}
void printDigits(uint32_t digits) {
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

// Ethernet
void ethernet_setup() {
  // Trying to connect the Ethernet:
  Serial.println("connecting to the Internet ...");
  Serial.println(" ");
  Ethernet.begin(mac);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  reconnect();
  }

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(Ethernet.localIP());
  Serial.println(" ");
  Udp.begin(localPort);
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime1);
  Serial.println(" ");
  Serial.print("It is ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println("");
  Serial.println("successfully connected to the Internet !");
  Serial.println(" ");
  

  
}

void reconnect() {
  uint32_t previousMillisreconnect = 0;
  uint32_t interval_reconnect = 5000;         // Wait 5 seconds before retrying
  uint32_t currentMillisreconnect = millis();
  uint32_t z = 0;
  if (currentMillisreconnect - previousMillisreconnect >= interval_reconnect && z <= 10) {
    previousMillisreconnect = currentMillisreconnect;
    z++;
    // Internet and NTP
    Ethernet.begin(mac);
    Serial.print("IP number assigned by DHCP is ");
    Serial.println(Ethernet.localIP());
    Serial.println(" ");
    Udp.begin(localPort);
    Serial.println("waiting for sync");
    setSyncProvider(getNtpTime1);
    Serial.println(" ");
    Serial.print("It is ");
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.println("");
    Serial.println("successfully connected to the Internet !");
    Serial.println(" ");
    
  }
}



//NTP
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime1() {
  Scheduler &s = Scheduler::currentScheduler();

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");

  sendNTPpacket(timeServer1);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 2000) {
    uint32_t size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");

      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      uint32_t secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (uint32_t)packetBuffer[40] << 24;
      secsSince1900 |= (uint32_t)packetBuffer[41] << 16;
      secsSince1900 |= (uint32_t)packetBuffer[42] << 8;
      secsSince1900 |= (uint32_t)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");

  return 0; // return 0 if unable to get the time

Udp.begin(localPort);
    Serial.println("waiting for sync");
    setSyncProvider(getNtpTime2); 
    getNtpTime2(); 

}

time_t getNtpTime2() {
  Scheduler &s = Scheduler::currentScheduler();

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");

  sendNTPpacket(timeServer2);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 2000) {
    uint32_t size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");

      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      uint32_t secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (uint32_t)packetBuffer[40] << 24;
      secsSince1900 |= (uint32_t)packetBuffer[41] << 16;
      secsSince1900 |= (uint32_t)packetBuffer[42] << 8;
      secsSince1900 |= (uint32_t)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");

  return 0; // return 0 if unable to get the time

Udp.begin(localPort);
    Serial.println("waiting for sync");
    setSyncProvider(getNtpTime3);  
    getNtpTime3();
}

time_t getNtpTime3() {
  Scheduler &s = Scheduler::currentScheduler();

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");

  sendNTPpacket(timeServer3);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 2000) {
    uint32_t size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");

      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      uint32_t secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (uint32_t)packetBuffer[40] << 24;
      secsSince1900 |= (uint32_t)packetBuffer[41] << 16;
      secsSince1900 |= (uint32_t)packetBuffer[42] << 8;
      secsSince1900 |= (uint32_t)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");

  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  Scheduler &s = Scheduler::currentScheduler();
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}


