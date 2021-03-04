/* <<<<<<<<<<<< Aquaponics 1.32 by Marco Heinzen >>>>>>>>>>>>>>>>>>>
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

//DHT11 Sensor
uint32_t DHT11_1PIN = 2;         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 
DHT_Unified dht(DHT11_1PIN, DHTTYPE);
uint32_t delayMS;
// mqtt Variables for printing temp and humidity
String temp_str; //see last code block below use these to convert the float that you get back from DHT to a string =str
String hum_str;
char dht11temp[50];
char dht11hum[50];


//  User Defined Variables
// ALARMS NOOOOOO ZEEEEROOOOOOS !!!! infront of single hour/min/sec
uint32_t LightsMorningONhour = 18; uint32_t LightsMorningONminute = 0; uint32_t LightsMorningONsec = 3;
uint32_t LightsMorningOFFhour = 18; uint32_t LightsMorningOFFminute = 15; uint32_t LightsMorningOFFsec = 3;
uint32_t LightsEveningONhour = 18; uint32_t LightsEveningONminute = 35; uint32_t LightsEveningONsec = 3;
uint32_t LightsEveningOFFhour = 18; uint32_t LightsEveningOFFminute = 59; uint32_t LightsEveningOFFsec = 3;

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
uint32_t CycleTimeHigh = (110 * 1000L);
uint32_t NTPupdateTime = (4 * 60 * 1000L);
uint32_t SensorCycle = 10013;
uint32_t manualUpdate = 5000;
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

//   ** Water Sensor  **

// +++++++++++++++++ Network Setup +++++++++++++++++
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xF7
};
IPAddress ip(192, 168, 50, 109);
IPAddress myDns(192, 168, 50, 1);
IPAddress gateway(192, 168, 50, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress mqtt_server(192, 168, 50, 108);


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
PubSubClient client(mqtt_server, 1883, callback1, ethClient);
char message_buff[100]; // this buffers our incoming messages so we can do something on certain commands

// ++++++++++++++++ Time +++++++++++++++++++++
// NTP Servers:
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov
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
  ethernet_setup();
}


// Ultra Sonic Range Sensor
#define ECHOPIN 42        // Pin to receive echo pulse 
#define TRIGPIN 41        // Pin to send trigger pulse
String usrsdiststr;
String usrswlstr;
String usrswvstr;
String usrswlistr;
char usrsdistance[50];
char usrswl[50];
char usrswv[50];
char usrswli[50];

// +++++++++++++++++++ RELAYS +++++++++++++++++++++
#define PumpeA_Relais1 22    // Pumpe Aquarium auf Relais 1
#define PumpeK_Relais2 23   // Pumpe Kreislauf auf Relais 2
#define LichtA_Relais3 24    // Licht Aquarium auf Relais 3
#define HeizstabA_Relais4 25  // Heizstab Aquarium auf Relais 4

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

uint32_t R1 = 1000;
uint32_t Ra = 25; //Resistance of powering Pins

uint32_t ECPin = A2;
uint32_t ECPower = A3;
uint32_t ECGround = A4;

uint32_t EC1Pin = A5;
uint32_t EC1Ground = A7;
uint32_t EC1Power = A6;

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
float K = 2.88;

//     **** END Of Recomended User Inputs              //


float AirTemperature = 0;
float WaterTemperature = 0;
float EC = 0;
float EC25 = 0;
uint32_t ppm = 0;


float raw = 0;
float Vin = 5;
float Vdrop = 0;
float Rc = 0;
float buffer = 0;
String rcstr;
String ec25str;
char mqttRc[50];
char mqttEC[50];

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

// Tasks

Scheduler r, hpr;
Task t1(SensorCycle, TASK_FOREVER, &Sensors, &r);
Task t2(intervallUpdate, TASK_FOREVER, &displayTime, &r);
Task t4(CycleTime, TASK_FOREVER, &Watering, &hpr);
Task t3(NTPupdateTime, TASK_FOREVER, &getTime, &r);

void displayTime() {
  digitalClockDisplay();

}

void Sensors() {
  readSensors();
}

void Watering() {

  abc();
  time_now();
  timestate();
  timestate_switch();

}

void getTime() {
  getNtpTime();
}

void setup_relays() {
  // initialize Relais 1-4 as Output
  Serial.println(" ");
  Serial.println("initializing RELAYS");
  Serial.println("defining Outputs...");

  pinMode(PumpeA_Relais1, OUTPUT);
  pinMode(PumpeK_Relais2, OUTPUT);
  pinMode(LichtA_Relais3, OUTPUT);
  pinMode(HeizstabA_Relais4, OUTPUT);
  Serial.println(" ");
  Serial.println("RELAYS");
  digitalWrite(PumpeA_Relais1, HIGH);
  Serial.println("setting PumpeA_Relais1: HIGH");
  digitalWrite(PumpeK_Relais2, LOW);
  Serial.println("setting PumpeK_Relais2: LOW");
  digitalWrite(LichtA_Relais3, LOW);
  Serial.println("setting LichtA_Relais3: LOW");
  digitalWrite(HeizstabA_Relais4, HIGH);
  Serial.println("setting HeizstabA_Relais4: HIGH");
  Serial.println("");
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

void setup_dht11() {
  dht.begin();

  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");
  Alarm.delay(11);
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

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
  pinMode(ECPin, INPUT);
  pinMode(ECPower, OUTPUT); //Setting pin for sourcing current
  pinMode(ECGround, OUTPUT); //setting pin for sinking current
  digitalWrite(ECGround, LOW); //We can leave the ground connected permanantly
  //EC2
  pinMode(EC1Pin, INPUT);
  pinMode(EC1Power, OUTPUT); //Setting pin for sourcing current
  pinMode(EC1Ground, OUTPUT); //setting pin for sinking current
  digitalWrite(EC1Ground, LOW); //We can leave the ground connected permanantly


  //** Adding Digital Pin Resistance to [25 ohm] to the static Resistor  ****//
  // Consule Read-Me for Why, or just accept it as true
  R1 = (R1 + Ra); // Taking into acount Powering Pin Resitance
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
  digitalWrite(PumpeA_Relais1, HIGH);
  Serial.println("setting PumpeA_Relais1: HIGH");
  digitalWrite(PumpeK_Relais2, LOW);
  Serial.println("setting PumpeK_Relais2: LOW");
  digitalWrite(LichtA_Relais3, HIGH);
  Serial.println("setting LichtA_Relais3: HIGH");
  digitalWrite(HeizstabA_Relais4, HIGH);
  Serial.println("setting HeizstabA_Relais4: HIGH");


  t4.setId(40);
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
}
void afternoon_setup() {
// set Relays to standard startup

  Serial.println("initializing afternoon_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, HIGH);
  Serial.println("setting PumpeA_Relais1: HIGH");
  digitalWrite(PumpeK_Relais2, LOW);
  Serial.println("setting PumpeK_Relais2: LOW");
  digitalWrite(LichtA_Relais3, LOW);
  Serial.println("setting LichtA_Relais3: LOW");
  digitalWrite(HeizstabA_Relais4, HIGH);
  Serial.println("setting HeizstabA_Relais4: HIGH");


  t4.setId(40);
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
}
void evening_setup() {


 // set Relays to standard startup
  Serial.println("initializing evening_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, HIGH);
  Serial.println("setting PumpeA_Relais1: HIGH");
  digitalWrite(PumpeK_Relais2, LOW);
  Serial.println("setting PumpeK_Relais2: LOW");
  digitalWrite(LichtA_Relais3, HIGH);
  Serial.println("setting LichtA_Relais3: HIGH");
  digitalWrite(HeizstabA_Relais4, HIGH);
  Serial.println("setting HeizstabA_Relais4: HIGH");


  t4.setId(40);
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
}
void night_setup() {

   // set Relays to standard startup
  Serial.println("initializing night_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, HIGH);
  Serial.println("setting PumpeA_Relais1: HIGH");
  digitalWrite(PumpeK_Relais2, LOW);
  Serial.println("setting PumpeK_Relais2: LOW");
  digitalWrite(LichtA_Relais3, LOW);
  Serial.println("setting LichtA_Relais3: LOW");
  digitalWrite(HeizstabA_Relais4, HIGH);
  Serial.println("setting HeizstabA_Relais4: HIGH");


  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well


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
  digitalWrite(PumpeA_Relais1, HIGH);
  Serial.println("setting PumpeA_Relais1: HIGH");
  digitalWrite(PumpeK_Relais2, LOW);
  Serial.println("setting PumpeK_Relais2: LOW");
  digitalWrite(LichtA_Relais3, LOW);
  Serial.println("setting LichtA_Relais3: LOW");
  digitalWrite(HeizstabA_Relais4, HIGH);
  Serial.println("setting HeizstabA_Relais4: HIGH");


  t4.setId(40);
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
  TSL2561.init();
  setup_dht11();
  setup_thermistor();
  setup_ec();
  setup_usrs();

  Serial.println("Check Time State");
  time_now();
  timestate();
  timestate_switch();

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
  client.loop();

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
  digitalWrite(PumpeA_Relais1, LOW);
  digitalWrite(PumpeK_Relais2, HIGH);
  uint32_t x = 0;
  do {
    unsigned long currentMillisWatering = millis();
    if ((unsigned long)(currentMillisWatering - previousMillisWatering) >= intervallUpdate) {

      digitalClockDisplay();
      UltraSonic();
      x++;
      previousMillisWatering = currentMillisWatering;
    }
  }
  while (x < (WateringTime / ((unsigned)1000)));

  digitalWrite(PumpeK_Relais2, LOW);
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
      UltraSonic();
      y++;
      previousMillisafterWatering = currentMillisafterWatering;
    }
  }
  while (y < (FlowbackTime / (unsigned)1000));

  Serial.print("Waited: ");
  Serial.print (FlowbackTime / (unsigned)1000);
  Serial.println ("  Seconds to let Water Flow back");
  Serial.println("Turning Aquarium Pump PumpeA_Relais1 ON ");
  digitalWrite(PumpeA_Relais1, HIGH);
  Serial.println("Aquarium Pump PumpeA_Relais1 is ON ");
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
  dht11();
  LightSensor();

  GetWaterTEMP();

  GetAirTEMP();

  PrintECReadings();
  Alarm.delay(11);

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
  WaterVolume = WaterLevel * ((unsigned) 60 * 35);
  float WaterLitres = 0;
  WaterLitres = WaterVolume / (unsigned)1000;

  /* The speed of sound is 340 m/s or 29 us per cm.The Ultrasonic burst travels out & back.So to find the distance of object we divide by 58  */

  if (distance <= 9) {
    Serial.print(WaterLevel);
    Serial.println(" cm Wasserstand ALARM !!");
    client.publish("Aquaponics/Sensors/WaterLevelAlarm", "ALARM WATERLEVEL!");

    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(WaterLevel);
    Serial.println(" cm Wasserstand ALARM !!");
    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(WaterLevel);
    Serial.println(" cm Wasserstand ALARM !!");
    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(WaterLevel);
    client.publish("Aquaponics/Sensors/WaterLevelAlarm", "ALARM WATERLEVEL!");

    Serial.println(" cm Wasserstand ALARM !!");
    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(WaterLevel);
    Serial.println(" cm Wasserstand ALARM !!");
    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(WaterLevel);
    Serial.println(" cm Wasserstand ALARM !!");
    client.publish("Aquaponics/Sensors/WaterLevelAlarm", "ALARM WATERLEVEL!");

    Serial.println(" MAXIMAL 29 cm Wasserhöhe)");
    Serial.print(distance);
    Serial.println(" cm von Decke (9 cm MAX)");
  }
  else {
    Serial.print(WaterLevel);
    Serial.print(" cm Waterlevel (29cm MAX) // ");

    Serial.print(distance);

    Serial.print(WaterLitres);
    Serial.print(" l Liter Wasser (");

    Serial.print(WaterVolume);
    Serial.println(" cm^2 (60900 cm^2 MAX)");

    usrsdiststr = String(distance); //converting ftemp (the float variable above) to a string
    usrsdiststr.toCharArray(usrsdistance, usrsdiststr.length() + 1); //packaging up the data to publish to mqtt whoa...
    client.publish("Aquaponics/Sensors/distance", usrsdistance);;

    usrswlstr = String(WaterLevel); //converting ftemp (the float variable above) to a string
    usrswlstr.toCharArray(usrswl, usrswlstr.length() + 1); //packaging up the data to publish to mqtt whoa...
    client.publish("Aquaponics/Sensors/WaterLevel", usrswl);;

    usrswvstr = String(WaterVolume); //converting ftemp (the float variable above) to a string
    usrswvstr.toCharArray(usrswv, usrswvstr.length() + 1); //packaging up the data to publish to mqtt whoa...
    client.publish("Aquaponics/Sensors/WaterVolume", usrswv);;

    usrswlistr = String(WaterLitres); //converting ftemp (the float variable above) to a string
    usrswlistr.toCharArray(usrswli, usrswlistr.length() + 1); //packaging up the data to publish to mqtt whoa...
    client.publish("Aquaponics/Sensors/WaterLitres", usrswli);;

  }
}

// DHT11
void dht11() {
  /*
    //counter for the messages, see if I am missing any on the Mqtt broker
    long now = millis();
    if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
  */
  //this is where you get the data from the sensor


  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("DHT11 Error reading temperature!");
  }
  else {
    Serial.print(event.temperature);
    Serial.println(" °C Air Temp (DHT11)");
    float dht11temperature = event.temperature;
    temp_str = String(dht11temperature); //converting ftemp (the float variable above) to a string
    temp_str.toCharArray(dht11temp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa...
    client.publish("Aquaponics/Sensors/DHT11_Temp", dht11temp);
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("DHT11 Error reading humidity!");
  }
  else {
    Serial.print(event.relative_humidity);
    Serial.println(" %  Humidity (DHT11)");
    float dht11humidity = event.relative_humidity;
    hum_str = String(dht11humidity); //converting Humidity (the float variable above) to a string
    hum_str.toCharArray(dht11hum, hum_str.length() + 1); //packaging up the data to publish to mqtt whoa...
    client.publish("Aquaponics/Sensors/DHT11_Hum", dht11hum);
  }
}

// TSL2561 Light Sensor
void LightSensor() {
  // Light Sensor
  Serial.print(TSL2561.readVisibleLux());
  Serial.println(" Lux (Light Value TSL2561)");
  float LV = TSL2561.readVisibleLux();
  char buffer[10];
  dtostrf(LV, 0, 0, buffer);
  client.publish("Aquaponics/Sensors/TSL2561", buffer);

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
  float WaterTemperature = TX1 - 21.5; // Calibration
  uint32_t WaTemp = TX1 - 21.5;
  Serial.print(WaterTemperature);
  Serial.println(" °C (Thermistor Water Temperature) ");

  water_str = String(WaterTemperature); //converting ftemp (the float variable above) to a string
  water_str.toCharArray(watertemp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa...
  client.publish("Aquaponics/Sensors/Thermistor_Water", watertemp);


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
  float  AirTemperature = TX2 - 10;
  Serial.print(AirTemperature);
  Serial.println(" °C (Thermistor Air Temperature)");
  air_str = String(AirTemperature); //converting ftemp (the float variable above) to a string
  air_str.toCharArray(airtemp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa...
  client.publish("Aquaponics/Sensors/Thermistor_Air", airtemp);;
};

//EC
void GetEC() {


  // Estimates Resistance of Liquid
  digitalWrite(ECPower, HIGH);
  raw = analogRead(ECPin);
  raw = analogRead(ECPin); // This is not a mistake, First reading will be low beause if charged a capacitor
  digitalWrite(ECPower, LOW);

  // Converts to EC
  Vdrop = (Vin * raw) / 1024.0;
  Rc = (Vdrop * R1) / (Vin - Vdrop);
  Rc = Rc - Ra; //acounting for Digital Pin Resitance
  EC = 1000 / (Rc * K);

  // Compensating For Temperaure
  EC25  =  EC / (1 + TemperatureCoef * (WaterTemperature - 25.0));
  ppm = (EC25) * (PPMconversion * 1000);

  rcstr = String(Rc); //converting ftemp (the float variable above) to a string
  rcstr.toCharArray(mqttRc, rcstr.length() + 1); //packaging up the data to publish to mqtt whoa...
  client.publish("Aquaponics/Sensors/EC1_RC", mqttRc);;

  ec25str = String(EC25); //converting ftemp (the float variable above) to a string
  ec25str.toCharArray(mqttEC, ec25str.length() + 1); //packaging up the data to publish to mqtt whoa...
  client.publish("Aquaponics/Sensors/EC1_EC25", mqttEC);;

  char buffer[10];
  dtostrf(ppm, 0, 0, buffer);
  client.publish("Aquaponics/Sensors/EC1_ppm", buffer);

}
void PrintECReadings() {
  GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
  Serial.print("Rc: ");
  Serial.print(Rc);
  Serial.print(" EC: ");
  Serial.print(EC25);
  Serial.print(" Siemens (SI)  ");
  Serial.print("ppm: ");
  Serial.print(ppm);
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
    reconnect();
      Serial.println("Failed to configure Ethernet using DHCP");
    }
  
  Serial.print("IP number assigned by DHCP is ");
  Serial.println(Ethernet.localIP());
  Serial.println(" ");
  Udp.begin(localPort);
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  Serial.println(" ");
  Serial.print("It is ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println("");
  Serial.println("successfully connected to the Internet !");
  Serial.println(" ");
  // MQTT Reconnection
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback1);
    if (client.connect("arduinoClient", "marcoheinzen", "8506mz")) {
      Serial.println("MQTT connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      client.publish("Aquaponics/MQTT/MQTTARDUINO_OUT", "MQTT Connected");

      // ... and resubscribe
      client.subscribe("inTopic");
      client.subscribe("Aquaponics/MQTT/MQTTARDUINO_IN");

    }   
  }

void reconnect() {
  uint32_t previousMillisreconnect = 0;
  uint32_t interval_reconnect = 5000;         // Wait 5 seconds before retrying
  uint32_t currentMillisreconnect = millis();
  uint32_t z = 0;
  if (currentMillisreconnect - previousMillisreconnect >= interval_reconnect && z<= 10) {
    previousMillisreconnect = currentMillisreconnect;
    z++;
    // Internet and NTP
  Ethernet.begin(mac);
  Serial.print("IP number assigned by DHCP is ");
  Serial.println(Ethernet.localIP());
  Serial.println(" ");
  Udp.begin(localPort);
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  Serial.println(" ");
  Serial.print("It is ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println("");
  Serial.println("successfully connected to the Internet !");
  Serial.println(" ");
  // MQTT Reconnection
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback1);
client.connect("arduinoClient", "marcoheinzen", "8506mz");
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient", "marcoheinzen", "8506mz"));
    if (!client.connect("arduinoClient", "marcoheinzen", "8506mz"));
      Serial.print("failed, rc=");
      Serial.println(client.state());
    }
  }



//NTP
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime() {
  Scheduler &s = Scheduler::currentScheduler();

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  client.publish("Aquaponics/NTP/Response", "Transmit NTP Request");

  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    uint32_t size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      client.publish("Aquaponics/NTP/Response", "Received NTP Response");

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

  client.publish("Aquaponics/NTP/Response", "No NTP Response!");
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


