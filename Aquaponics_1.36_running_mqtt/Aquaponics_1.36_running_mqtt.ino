/* <<<<<<<<<<<< Aquaponics 1.35 by Marco Heinzen >>>>>>>>>>>>>>>>>>>
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
uint32_t DHT11_1PIN = 7;         // Pin which is connected to the DHT sensor.
uint32_t DHT11_2PIN = 8;         // Pin which is connected to the DHT sensor.
#define DHTTYPE1           DHT11     // DHT 11 
#define DHTTYPE2           DHT11     // DHT 11 

DHT_Unified dht1(DHT11_1PIN, DHTTYPE1);
DHT_Unified dht2(DHT11_2PIN, DHTTYPE2);

uint32_t delayMS;
// mqtt Variables for printing temp and humidity
String temp_str; //see last code block below use these to convert the float that you get back from DHT to a string =str
String hum_str;
char dht11temp[50];
char dht11hum[50];





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

// Time Now
  uint32_t TimeNOWhour = 0L;
  uint32_t TimeNOWminute = 0L;
  uint32_t TimeNOWsec = 0L;
  uint32_t TimeNOW = 0L;
  uint32_t TimeState = 0;

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
#define LichtA1_Relais2 23   // Pumpe Kreislauf auf Relais 2
#define LichtA2_Relais3 24    // Licht Weiss Aquarium auf Relais 3
#define PumpeK_Relais4 25    // Licht Rot Aquarium auf Relais 4

#define HeizstabA_Relais5 26  // Heizstab Aquarium auf Relais 5
#define LichtGrow1_Relais6 27  // Licht Grow 1 30W
#define LichtGrow2_Relais7 28  // Licht Grow 2 30W
#define GrowAir1_Relais8 29  // Ventilation GROW IN 

#define GrowAir2_Relais9 30  // Ventilation GROW OUT 
#define TechnoAir1_Relais10 31  // Ventilation Technik OUT
#define TechnoAir2_Relais11 32  // Ventilation Technik IN
#define TechnoAir3_Relais12 33  //  Ventilation LED OUT 

#define AquaAir1_Relais13 34  // Ventilation Auqua 1 
#define AquaAir2_Relais14 35  // Ventilation Auqua 2

//   Temp Probe Related

//   Thermometer with thermistor

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



void method_prototypes() {
  // Callback methods prototypes
  void Sensors();
  void Watering();
  void getTime();
  void displayTime();
  void reconnect();
}

void tasks() {
  // ++++++++++++++++++ CYCLE TIMES ++++++++++++++++++++
  uint32_t WateringTime = 22023L;
  uint32_t FlowbackTime = 76019L;
  uint32_t CycleTime = (6 * 60 * 1000L);
  uint32_t CycleTimeHigh = (2 * 60  * 1000L);
  uint32_t NTPupdateTime = (5 * 60 * 1000L);
  uint32_t SensorCycle = 10013;
  uint32_t manualUpdate = 5000;
  uint32_t ReconnectTime = (30 * 60 * 1000L);
  //  intervals for updating Display
  uint32_t intervallUpdate = 1000;

  // Tasks
  Scheduler r, hpr;
  Task t1(SensorCycle, TASK_FOREVER, &Sensors, &r);
  Task t2(intervallUpdate, TASK_FOREVER, &displayTime, &r);
  Task t4(CycleTime, TASK_FOREVER, &Watering, &hpr);
  Task t3(NTPupdateTime, TASK_FOREVER, &getTime, &r);
  Task t5(ReconnectTime, TASK_FOREVER, &reconnect, &r);

}


void displayTime() {
  Alarm.delay(1);

  digitalClockDisplay();
  Alarm.delay(1);

}

void Sensors() {
  Alarm.delay(1);

  readSensors();
  Alarm.delay(1);

}

void Watering() {
  Alarm.delay(10);

  abc();
  Alarm.delay(10);

  time_now();
  Alarm.delay(10);

  aqua_timestate();
  Alarm.delay(10);

  aqua_timestate_switch();
  Alarm.delay(10);  
  
  grow_timestate();
  Alarm.delay(10);

  grow_timestate_switch();
  Alarm.delay(10);


}

void getTime() {
  Alarm.delay(10);

  getNtpTime();
  Alarm.delay(10);

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
  pinMode(GrowAir1_Relais8, OUTPUT);

  pinMode(GrowAir2_Relais9, OUTPUT);
  pinMode(TechnoAir1_Relais10, OUTPUT);
  pinMode(TechnoAir2_Relais11, OUTPUT);
  pinMode(TechnoAir3_Relais12, OUTPUT);

  pinMode(AquaAir1_Relais13, OUTPUT);
  pinMode(AquaAir2_Relais14, OUTPUT);

  Serial.println(" ");
  Serial.println("RELAYS");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: ON");

  digitalWrite(LichtA1_Relais2, LOW);
  Serial.println("setting LichtA1_Relais2: OFF");

  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: OFF");

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais5: OFF");

  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais4: ON");

  digitalWrite(LichtGrow1_Relais6, LOW);
  Serial.println("setting LichtGrow1_Relais6: OFF");

  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: OFF");

  digitalWrite(GrowAir1_Relais8, LOW);
  Serial.println("setting GrowAir1_Relais8: OFF");

  digitalWrite(GrowAir2_Relais9, LOW);
  Serial.println("setting GrowAir1_Relais9: OFF");

  digitalWrite(TechnoAir1_Relais10, HIGH);
  Serial.println("setting TechnoAir1_Relais10: OFF");

  digitalWrite(TechnoAir2_Relais11, HIGH);
  Serial.println("setting TechnoAir2_Relais11: OFF");

  digitalWrite(TechnoAir3_Relais12, LOW);
  Serial.println("setting TechnoAir3_Relais12: OFF");

  digitalWrite(AquaAir1_Relais13, LOW);
  Serial.println("setting AquaAir1_Relais13: OFF");

  digitalWrite(AquaAir2_Relais14, LOW);
  Serial.println("setting AquaAir2_Relais14: OFF");
  Serial.println("");
}



void setup_alarms() {
  //  User Defined Variables
  uint32_t DailySetupHour = 9; uint32_t DailySetupMinute = 35; uint32_t DailySetupSec = 3;
  uint32_t AquaSunriseHour = 14; uint32_t AquaSunriseMinute = 15; uint32_t AquaSunriseSec = 3;
  uint32_t GrowMorningHour = 15; uint32_t GrowMorningMinute = 15; uint32_t GrowMorningSec = 3;
  uint32_t AuqaMorningHour = 21; uint32_t AuqaMorningMinute = 30; uint32_t AuqaMorningSec = 3;
  uint32_t AquaAfternoonHour = 9; uint32_t AquaAfternoonMinute = 35; uint32_t AquaAfternoonSec = 3;
  uint32_t GrowAfternoonHour = 14; uint32_t GrowAfternoonMinute = 15; uint32_t GrowAfternoonSec = 3;
  uint32_t GrowEveningHour = 15; uint32_t GrowEveningMinute = 15; uint32_t GrowEveningSec = 3;
  uint32_t AquaEveningHour = 21; uint32_t AquaEveningMinute = 30; uint32_t AquaEveningSec = 3;
  uint32_t GrowNightHour = 9; uint32_t GrowNightMinute = 35; uint32_t GrowNightSec = 3;
  uint32_t AquaNightHour = 21; uint32_t AquaNightMinute = 30; uint32_t AquaNightSec = 3;

  // ms from midnight
  uint32_t DailySetup = (((DailySetupHour) * 3600000L) + ((DailySetupMinute) * 60000L) + ((DailySetupSec) * 1000L) * 1L);
  uint32_t AquaSunrise = (((AquaSunriseHour) * 3600000L) + ((AquaSunriseMinute) * 60000L) + ((AquaSunriseSec) * 1000L) * 1L);
  uint32_t GrowMorning = (((GrowMorningHour) * 3600000L) + ((GrowMorningMinute) * 60000L) + ((GrowMorningSec) * 1000L) * 1L);
  uint32_t AuqaMorning = (((AuqaMorningHour) * 3600000L) + ((AuqaMorningMinute) * 60000L) + ((AuqaMorningSec) * 1000L) * 1L);
  uint32_t AquaAfternoon = (((AquaAfternoonHour) * 3600000L) + ((AquaAfternoonMinute) * 60000L) + ((AquaAfternoonSec) * 1000L) * 1L);
  uint32_t GrowAfternoon = (((GrowAfternoonHour) * 3600000L) + ((GrowAfternoonMinute) * 60000L) + ((GrowAfternoonSec) * 1000L) * 1L);
  uint32_t GrowEvening = (((GrowEveningHour) * 3600000L) + ((GrowEveningMinute) * 60000L) + ((GrowEveningSec) * 1000L) * 1L);
  uint32_t AquaEvening = (((AquaEveningHour) * 3600000L) + ((AquaEveningMinute) * 60000L) + ((AquaEveningSec) * 1000L) * 1L);
  uint32_t GrowNight = (((GrowNightHour) * 3600000L) + ((GrowNightMinute) * 60000L) + ((GrowNightSec) * 1000L) * 1L);
  uint32_t AquaNight = (((AquaNightHour) * 3600000L) + ((AquaNightMinute) * 60000L) + ((AquaNightSec) * 1000L) * 1L);


  // ALARM TIMER RELAIS
  Alarm.alarmRepeat(DailySetupHour, DailySetupMinute, DailySetupSec, daily_setup); // Sunrise
  Alarm.alarmRepeat(AquaSunriseHour, AquaSunriseMinute, AquaSunriseSec, aqua_sunrise_setup); // Aqua Sunrise
  Alarm.alarmRepeat(GrowMorningHour, GrowMorningMinute, GrowMorningSec, grow_morning_setup); // Grow Sunrise
  Alarm.alarmRepeat(AuqaMorningHour, AuqaMorningMinute, AuqaMorningSec, aqua_morning_setup); // Aqua Morning
  Alarm.alarmRepeat(AquaAfternoonHour, AquaAfternoonMinute, AquaAfternoonSec, aqua_afternoon_setup); // Aqua Afternoon
  Alarm.alarmRepeat(GrowAfternoonHour, GrowAfternoonMinute, GrowAfternoonSec, grow_afternoon_setup); // Grow Afternoon
  Alarm.alarmRepeat(GrowEveningHour, GrowEveningMinute, GrowEveningSec, grow_evening_setup); // Grow Evening
  Alarm.alarmRepeat(AquaEveningHour, AquaEveningMinute, AquaEveningSec, aqua_evening_setup); // Grow Evening
  Alarm.alarmRepeat(GrowNightHour, GrowNightMinute, GrowNightSec, grow_night_setup); // Grow Night
  Alarm.alarmRepeat(AquaNightHour, AquaNightMinute, AquaNightSec, aqua_night_setup); // Grow Night

  Alarm.delay(11);
}

void setup_dht11() {
  dht1.begin();
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

  dht2.begin();
  sensor_t sensor;
  dht2.temperature().getSensor(&sensor);
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
  dht2.humidity().getSensor(&sensor);
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
void daily_setup() {

}
void time_now() {
  Alarm.delay(10);

  // Time Now
  TimeNOWhour = (hour()) * 3600000L;
  TimeNOWminute = (minute()) * 60000L;
  TimeNOWsec = (second()) * 1000L;
  TimeNOW = (TimeNOWhour + TimeNOWminute + TimeNOWsec);
  Alarm.delay(10);

}
void aqua_timestate() {
  Alarm.delay(10);

  if ((AquaSunrise <= TimeNOW && TimeNOW < AuqaMorning)) {
    aqua_TimeState = 1;
    Serial.println("Aqua: Sunrise");
  }
  if ( AuqaMorning <= TimeNOW && TimeNOW < AquaAfternoon) {
    aqua_TimeState = 2;
    Serial.println("Aqua: Morning");
  }
  if (AquaAfternoon >= TimeNOW && TimeNOW < AquaEvening ) {
    aqua_TimeState = 3;
    Serial.println("Aqua: Afternoon");
  }
  if (AquaEvening >= TimeNOW && TimeNOW < AquaNight ) {
    aqua_TimeState = 4;
    Serial.println("Aqua: Evening");
  }
  if (AquaNight <= TimeNOW || TimeNOW < AquaSunrise) {
    aqua_TimeState = 5;
    Serial.println("Aqua: Night");
    Alarm.delay(10);

  }
}

void grow_timestate() {
  Alarm.delay(10);

  if ((GrowMorning <= TimeNOW && TimeNOW < GrowAfternoon)) {
    grow_TimeState = 1;
    Serial.println("Grow: Sunrise");
  }
  if ( GrowAfternoon <= TimeNOW && TimeNOW < GrowEvening) {
    grow_TimeState = 2;
    Serial.println("Grow: Morning");
  }
  if (GrowEvening >= TimeNOW && TimeNOW < GrowNight ) {
    grow_TimeState = 3;
    Serial.println("Grow: Afternoon");
  }

  if (GrowNight <= TimeNOW || TimeNOW < GrowMorning) {
    grow_TimeState = 4;
    Serial.println("Grow: Night");
    Alarm.delay(10);
  }
}

void aqua_timestate_switch() {
  Alarm.delay(10);

  switch (TimeState) {
    case 1:
      aqua_sunrise_setup();
      break;
    case 2:
      aqua_morning_setup();
      break;
    case 3:
      aqua_afternoon_setup();
      break;
    case 4:
      aqua_evening_setup();
      break;
    case 5:
      aqua_night_setup();
      break;
    default:
      aqua_default_setup();
      break;
      Alarm.delay(10);

  }
}

void grow_timestate_switch() {
  Alarm.delay(10);

  switch (TimeState) {
    case 1:
      grow_morning_setup();
      break;
    case 2:
      grow_afternoon_setup();
      break;
    case 3:
      grow_evening_setup();
      break;
    case 4:
      grow_night_setup();
      break;
    default:
      grow_default_setup();
      break;
      Alarm.delay(10);

  }
}

void aqua_morning_setup() {
  Alarm.delay(10);


  // set Relays to standard startup

  Serial.println("initializing morning_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: ON");

  digitalWrite(LichtA1_Relais2, HIGH);
  Serial.println("setting LichtA1_Relais2: ON");

  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: OFF");

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais5: OFF");

  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais4: ON");

  digitalWrite(LichtGrow1_Relais6, HIGH);
  Serial.println("setting LichtGrow1_Relais6: HIGH");

  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: OFF");

  digitalWrite(GrowAir1_Relais8, LOW);
  Serial.println("setting GrowAir1_Relais8: OFF");

  digitalWrite(GrowAir2_Relais9, LOW);
  Serial.println("setting GrowAir1_Relais9: OFF");

  digitalWrite(TechnoAir1_Relais10, HIGH);
  Serial.println("setting TechnoAir1_Relais10: ON");

  digitalWrite(TechnoAir2_Relais11, LOW);
  Serial.println("setting TechnoAir2_Relais11: OFF");

  digitalWrite(AquaAir1_Relais12, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  digitalWrite(AquaAir2_Relais13, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  t5.setId(50);
  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well
  t5.enable();
  r.addTask(t5);
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
void aqua_afternoon_setup() {
  Alarm.delay(10);

  // set Relays to standard startup

  Serial.println("initializing afternoon_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: ON");

  digitalWrite(LichtA1_Relais2, LOW);
  Serial.println("setting LichtA1_Relais2: OFF");

  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: OFF");

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais5: OFF");

  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais4: ON");

  digitalWrite(LichtGrow1_Relais6, HIGH);
  Serial.println("setting LichtGrow1_Relais6: ON");

  digitalWrite(LichtGrow2_Relais7, HIGH);
  Serial.println("setting LichtGrow2_Relais7: ON");

  digitalWrite(GrowAir1_Relais8, HIGH);
  Serial.println("setting GrowAir1_Relais8: ON");

  digitalWrite(GrowAir2_Relais9, HIGH);
  Serial.println("setting GrowAir1_Relais9: ON");

  digitalWrite(TechnoAir1_Relais10, HIGH);
  Serial.println("setting TechnoAir1_Relais10: ON");

  digitalWrite(TechnoAir2_Relais11, HIGH);
  Serial.println("setting TechnoAir2_Relais11: ON");

  digitalWrite(AquaAir1_Relais12, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  digitalWrite(AquaAir2_Relais13, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  t5.setId(50);
  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well
  t5.enable();
  r.addTask(t5);
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
void aqua_evening_setup() {

  Alarm.delay(10);

  // set Relays to standard startup
  Serial.println("initializing evening_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: ON");

  digitalWrite(LichtA1_Relais2, HIGH);
  Serial.println("setting LichtA1_Relais2: ON");

  digitalWrite(LichtA2_Relais3, HIGH);
  Serial.println("setting LichtA2_Relais3: ON");

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais5: OFF");

  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais4: ON");

  digitalWrite(LichtGrow1_Relais6, HIGH);
  Serial.println("setting LichtGrow1_Relais6: ON");

  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: LOW");

  digitalWrite(GrowAir1_Relais8, LOW);
  Serial.println("setting GrowAir1_Relais8: OFF");

  digitalWrite(GrowAir2_Relais9, LOW);
  Serial.println("setting GrowAir1_Relais9: OFF");

  digitalWrite(TechnoAir1_Relais10, HIGH);
  Serial.println("setting TechnoAir1_Relais10: ON");

  digitalWrite(TechnoAir2_Relais11, LOW);
  Serial.println("setting TechnoAir2_Relais11: OFF");

  digitalWrite(AquaAir1_Relais12, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  digitalWrite(AquaAir2_Relais13, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");


  t5.setId(50);
  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well
  t5.enable();
  r.addTask(t5);
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
void aqua_night_setup() {
  Alarm.delay(10);

  // set Relays to standard startup
  Serial.println("initializing night_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: ON");

  digitalWrite(LichtA1_Relais2, LOW);
  Serial.println("setting LichtA1_Relais2: OFF");

  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: OFF");

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais5: OFF");

  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais4: ON");

  digitalWrite(LichtGrow1_Relais6, LOW);
  Serial.println("setting LichtGrow1_Relais6: OFF");

  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: OFF");

  digitalWrite(GrowAir1_Relais8, LOW);
  Serial.println("setting GrowAir1_Relais8: OFF");

  digitalWrite(GrowAir2_Relais9, LOW);
  Serial.println("setting GrowAir1_Relais9: OFF");

  digitalWrite(TechnoAir1_Relais10, HIGH);
  Serial.println("setting TechnoAir1_Relais10: ON");

  digitalWrite(TechnoAir2_Relais11, LOW);
  Serial.println("setting TechnoAir2_Relais11: OFF");

  digitalWrite(AquaAir1_Relais12, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  digitalWrite(AquaAir2_Relais13, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");


  t5.setId(50);
  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well
  t5.enable();
  r.addTask(t5);
  t4.disable();
  r.deleteTask(t4);
  t1.enable();
  r.addTask(t1);
  t2.enable();
  r.addTask(t2);
  t3.enable();
  r.addTask(t3);
  Serial.println("WATERING IS OFF");

}
void aqua_default_setup() {
  Alarm.delay(10);

  // set Relays to standard startup
  Serial.println("initializing default_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: ON");

  digitalWrite(LichtA1_Relais2, LOW);
  Serial.println("setting LichtA1_Relais2: OFF");

  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: OFF");

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais5: OFF");

  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais4: ON");

  digitalWrite(LichtGrow1_Relais6, LOW);
  Serial.println("setting LichtGrow1_Relais6: OFF");

  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: OFF");

  digitalWrite(GrowAir1_Relais8, LOW);
  Serial.println("setting GrowAir1_Relais8: OFF");

  digitalWrite(GrowAir2_Relais9, LOW);
  Serial.println("setting GrowAir1_Relais9: OFF");

  digitalWrite(TechnoAir1_Relais10, HIGH);
  Serial.println("setting TechnoAir1_Relais10: ON");

  digitalWrite(TechnoAir2_Relais11, LOW);
  Serial.println("setting TechnoAir2_Relais11: OFF");

  digitalWrite(AquaAir1_Relais12, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  digitalWrite(AquaAir2_Relais13, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");


  t5.setId(50);
  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well
  t5.enable();
  r.addTask(t5);
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
void grow_morning_setup() {
  Alarm.delay(10);


  // set Relays to standard startup

  Serial.println("initializing morning_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: ON");

  digitalWrite(LichtA1_Relais2, HIGH);
  Serial.println("setting LichtA1_Relais2: ON");

  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: OFF");

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais5: OFF");

  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais4: ON");

  digitalWrite(LichtGrow1_Relais6, HIGH);
  Serial.println("setting LichtGrow1_Relais6: HIGH");

  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: OFF");

  digitalWrite(GrowAir1_Relais8, LOW);
  Serial.println("setting GrowAir1_Relais8: OFF");

  digitalWrite(GrowAir2_Relais9, LOW);
  Serial.println("setting GrowAir1_Relais9: OFF");

  digitalWrite(TechnoAir1_Relais10, HIGH);
  Serial.println("setting TechnoAir1_Relais10: ON");

  digitalWrite(TechnoAir2_Relais11, LOW);
  Serial.println("setting TechnoAir2_Relais11: OFF");

  digitalWrite(AquaAir1_Relais12, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  digitalWrite(AquaAir2_Relais13, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  t5.setId(50);
  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well
  t5.enable();
  r.addTask(t5);
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
void grow_afternoon_setup() {
  Alarm.delay(10);

  // set Relays to standard startup

  Serial.println("initializing afternoon_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: ON");

  digitalWrite(LichtA1_Relais2, LOW);
  Serial.println("setting LichtA1_Relais2: OFF");

  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: OFF");

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais5: OFF");

  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais4: ON");

  digitalWrite(LichtGrow1_Relais6, HIGH);
  Serial.println("setting LichtGrow1_Relais6: ON");

  digitalWrite(LichtGrow2_Relais7, HIGH);
  Serial.println("setting LichtGrow2_Relais7: ON");

  digitalWrite(GrowAir1_Relais8, HIGH);
  Serial.println("setting GrowAir1_Relais8: ON");

  digitalWrite(GrowAir2_Relais9, HIGH);
  Serial.println("setting GrowAir1_Relais9: ON");

  digitalWrite(TechnoAir1_Relais10, HIGH);
  Serial.println("setting TechnoAir1_Relais10: ON");

  digitalWrite(TechnoAir2_Relais11, HIGH);
  Serial.println("setting TechnoAir2_Relais11: ON");

  digitalWrite(AquaAir1_Relais12, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  digitalWrite(AquaAir2_Relais13, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  t5.setId(50);
  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well
  t5.enable();
  r.addTask(t5);
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
void grow_evening_setup() {

  Alarm.delay(10);

  // set Relays to standard startup
  Serial.println("initializing evening_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: ON");

  digitalWrite(LichtA1_Relais2, HIGH);
  Serial.println("setting LichtA1_Relais2: ON");

  digitalWrite(LichtA2_Relais3, HIGH);
  Serial.println("setting LichtA2_Relais3: ON");

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais5: OFF");

  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais4: ON");

  digitalWrite(LichtGrow1_Relais6, HIGH);
  Serial.println("setting LichtGrow1_Relais6: ON");

  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: LOW");

  digitalWrite(GrowAir1_Relais8, LOW);
  Serial.println("setting GrowAir1_Relais8: OFF");

  digitalWrite(GrowAir2_Relais9, LOW);
  Serial.println("setting GrowAir1_Relais9: OFF");

  digitalWrite(TechnoAir1_Relais10, HIGH);
  Serial.println("setting TechnoAir1_Relais10: ON");

  digitalWrite(TechnoAir2_Relais11, LOW);
  Serial.println("setting TechnoAir2_Relais11: OFF");

  digitalWrite(AquaAir1_Relais12, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  digitalWrite(AquaAir2_Relais13, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");


  t5.setId(50);
  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well
  t5.enable();
  r.addTask(t5);
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
void grow_night_setup() {
  Alarm.delay(10);

  // set Relays to standard startup
  Serial.println("initializing night_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: ON");

  digitalWrite(LichtA1_Relais2, LOW);
  Serial.println("setting LichtA1_Relais2: OFF");

  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: OFF");

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais5: OFF");

  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais4: ON");

  digitalWrite(LichtGrow1_Relais6, LOW);
  Serial.println("setting LichtGrow1_Relais6: OFF");

  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: OFF");

  digitalWrite(GrowAir1_Relais8, LOW);
  Serial.println("setting GrowAir1_Relais8: OFF");

  digitalWrite(GrowAir2_Relais9, LOW);
  Serial.println("setting GrowAir1_Relais9: OFF");

  digitalWrite(TechnoAir1_Relais10, HIGH);
  Serial.println("setting TechnoAir1_Relais10: ON");

  digitalWrite(TechnoAir2_Relais11, LOW);
  Serial.println("setting TechnoAir2_Relais11: OFF");

  digitalWrite(AquaAir1_Relais12, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  digitalWrite(AquaAir2_Relais13, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");


  t5.setId(50);
  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well
  t5.enable();
  r.addTask(t5);
  t4.disable();
  r.deleteTask(t4);
  t1.enable();
  r.addTask(t1);
  t2.enable();
  r.addTask(t2);
  t3.enable();
  r.addTask(t3);
  Serial.println("WATERING IS OFF");

}
void grow_default_setup() {
  Alarm.delay(10);

  // set Relays to standard startup
  Serial.println("initializing default_setup");
  Serial.println(" ");
  digitalWrite(PumpeA_Relais1, LOW);
  Serial.println("setting PumpeA_Relais1: ON");

  digitalWrite(LichtA1_Relais2, LOW);
  Serial.println("setting LichtA1_Relais2: OFF");

  digitalWrite(LichtA2_Relais3, LOW);
  Serial.println("setting LichtA2_Relais3: OFF");

  digitalWrite(PumpeK_Relais4, LOW);
  Serial.println("setting PumpeK_Relais5: OFF");

  digitalWrite(HeizstabA_Relais5, HIGH);
  Serial.println("setting HeizstabA_Relais4: ON");

  digitalWrite(LichtGrow1_Relais6, LOW);
  Serial.println("setting LichtGrow1_Relais6: OFF");

  digitalWrite(LichtGrow2_Relais7, LOW);
  Serial.println("setting LichtGrow2_Relais7: OFF");

  digitalWrite(GrowAir1_Relais8, LOW);
  Serial.println("setting GrowAir1_Relais8: OFF");

  digitalWrite(GrowAir2_Relais9, LOW);
  Serial.println("setting GrowAir1_Relais9: OFF");

  digitalWrite(TechnoAir1_Relais10, HIGH);
  Serial.println("setting TechnoAir1_Relais10: ON");

  digitalWrite(TechnoAir2_Relais11, LOW);
  Serial.println("setting TechnoAir2_Relais11: OFF");

  digitalWrite(AquaAir1_Relais12, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");

  digitalWrite(AquaAir2_Relais13, LOW);
  Serial.println("setting AquaAir1_Relais12: OFF");


  t5.setId(50);
  t4.setId(40);
  r.setHighPriorityScheduler(&hpr);
  r.enableAll(true); // this will recursively enable the higher priority tasks as well
  t5.enable();
  r.addTask(t5);
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
  Alarm.delay(101);

  Serial.println("Initializing Setup:");
  Serial.println("loading ... ");
  Serial.println(" ");
  Alarm.delay(101);
  method_prototypes();
  tasks();

  Wire.begin();
  Alarm.delay(101);

  logo();
  setup_relays();
  Alarm.delay(101);

  Serial.println("Initializing Sensors");
  setup_dht11();
  setup_thermistor();
  setup_ec();
  setup_usrs();
  TSL2561.init();
  Alarm.delay(101);

  setup_ethernet();
  setup_alarms();
  Alarm.delay(101);

  Serial.println("Check Time State");
  time_now();
  aqua_timestate();
  aqua_timestate_switch();
  grow_timestate();
  grow_timestate_switch();
  Alarm.delay(101);

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

  Alarm.delay(101);
}


//  END OF MAIN LOOP
// SUBROUTINES
// EVENT ROUTINE
void abc () {
  Alarm.delay(10);

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
  uint32_t x = 0;
  do {
    Alarm.delay(10);

    unsigned long currentMillisWatering = millis();
    if ((unsigned long)(currentMillisWatering - previousMillisWatering) >= intervallUpdate) {

      digitalClockDisplay();
      Alarm.delay(10);

      UltraSonic();
      Alarm.delay(10);

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
      Alarm.delay(10);

      digitalClockDisplay();
      Alarm.delay(10);

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
  digitalWrite(PumpeA_Relais1, LOW);
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
  Alarm.delay(100);

  UltraSonic();
  Alarm.delay(100);

  dht11();
  Alarm.delay(100);

  LightSensor();
  Alarm.delay(100);

  GetWaterTEMP();
  Alarm.delay(100);

  GetAirTEMP();
  Alarm.delay(100);

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
    Alarm.delay(100);

  }
  else {
    Serial.print(WaterLevel);
    Serial.print(" cm Waterlevel (29cm MAX) // ");

    Serial.print(distance);

    Serial.print(WaterLitres);
    Serial.print(" l Liter Wasser (");

    Serial.print(WaterVolume);
    Serial.println(" cm^2 (60900 cm^2 MAX)");
    Alarm.delay(100);


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
    Alarm.delay(100);


  }
}

// DHT11
void dht11() {
  Alarm.delay(10);

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
    Alarm.delay(10);

  }
  else {
    Serial.print(event.temperature);
    Serial.println(" °C Air Temp (DHT11)");
    float dht11temperature = event.temperature;
    temp_str = String(dht11temperature); //converting ftemp (the float variable above) to a string
    temp_str.toCharArray(dht11temp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa...
    client.publish("Aquaponics/Sensors/DHT11_Temp", dht11temp);
    Alarm.delay(10);

  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("DHT11 Error reading humidity!");
    Alarm.delay(10);

  }
  else {
    Serial.print(event.relative_humidity);
    Serial.println(" %  Humidity (DHT11)");
    float dht11humidity = event.relative_humidity;
    hum_str = String(dht11humidity); //converting Humidity (the float variable above) to a string
    hum_str.toCharArray(dht11hum, hum_str.length() + 1); //packaging up the data to publish to mqtt whoa...
    client.publish("Aquaponics/Sensors/DHT11_Hum", dht11hum);
    Alarm.delay(10);

  }
}

// TSL2561 Light Sensor
void LightSensor() {
  Alarm.delay(10);
  /*
    // Light Sensor
    Serial.print(TSL2561.readVisibleLux());
    Serial.println(" Lux (Light Value TSL2561)");
    float LV = TSL2561.readVisibleLux();
    char buffer[10];
    dtostrf(LV, 0, 0, buffer);
    client.publish("Aquaponics/Sensors/TSL2561", buffer);
    Alarm.delay(10);
  */

}

//Thermistor Water Temp
void GetWaterTEMP() {
  Alarm.delay(10);

  VRT1 = analogRead(A0);              //Acquisition analog value of VRT
  VRT1 = (5.00 / 1023.00) * VRT1;      //Conversion to voltage
  VR1 = VCC - VRT1;
  RT1 = VRT1 / (VR1 / R);               //Resistance of RT

  ln = log(RT1 / RT0);
  TX1 = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor

  TX1 = TX1 - 273.15;                 //Conversion to Celsius
  float WaterTemperature = TX1 - 21.5; // Calibration
  uint32_t WaTemp = TX1 - 0;
  Serial.print(WaterTemperature);
  Serial.println(" °C (Thermistor Water Temperature) ");
  Alarm.delay(10);

  water_str = String(WaterTemperature); //converting ftemp (the float variable above) to a string
  water_str.toCharArray(watertemp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa...
  client.publish("Aquaponics/Sensors/Thermistor_Water", watertemp);
  Alarm.delay(10);


};

//Thermistor Air Temp
void GetAirTEMP() {
  Alarm.delay(10);

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
  Alarm.delay(10);

  air_str = String(AirTemperature); //converting ftemp (the float variable above) to a string
  air_str.toCharArray(airtemp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa...
  client.publish("Aquaponics/Sensors/Thermistor_Air", airtemp);
  Alarm.delay(10);
}

//EC
void GetEC() {
  Alarm.delay(10);


  // Estimates Resistance of Liquid
  digitalWrite(ECPower, HIGH);
  raw = analogRead(ECPin);
  raw = analogRead(ECPin); // This is not a mistake, First reading will be low beause if charged a capacitor
  digitalWrite(ECPower, LOW);
  Alarm.delay(10);

  // Converts to EC
  Vdrop = (Vin * raw) / 1024.0;
  Rc = (Vdrop * R1) / (Vin - Vdrop);
  Rc = Rc - Ra; //acounting for Digital Pin Resitance
  EC = 1000 / (Rc * K);

  // Compensating For Temperaure
  EC25  =  EC / (1 + TemperatureCoef * (WaterTemperature - 25.0));
  ppm = (EC25) * (PPMconversion * 1000);
  Alarm.delay(10);

  rcstr = String(Rc); //converting ftemp (the float variable above) to a string
  rcstr.toCharArray(mqttRc, rcstr.length() + 1); //packaging up the data to publish to mqtt whoa...
  client.publish("Aquaponics/Sensors/EC1_RC", mqttRc);;

  ec25str = String(EC25); //converting ftemp (the float variable above) to a string
  ec25str.toCharArray(mqttEC, ec25str.length() + 1); //packaging up the data to publish to mqtt whoa...
  client.publish("Aquaponics/Sensors/EC1_EC25", mqttEC);;

  char buffer[10];
  dtostrf(ppm, 0, 0, buffer);
  client.publish("Aquaponics/Sensors/EC1_ppm", buffer);
  Alarm.delay(10);


}
void PrintECReadings() {
  Alarm.delay(10);

  GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water

  Alarm.delay(10);
  Serial.print("Rc: ");
  Serial.print(Rc);
  Serial.print(" EC: ");
  Serial.print(EC25);
  Serial.print(" Siemens (SI)  ");
  Serial.print("ppm: ");
  Serial.print(ppm);
  Serial.print(" at ");
  GetWaterTEMP();
  Alarm.delay(10);



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
  Alarm.delay(1);

}
void printDigits(uint32_t digits) {
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
  Alarm.delay(1);

}

// Ethernet
void ethernet_setup() {
  // Trying to connect the Ethernet:
  Serial.println("connecting to the Internet ...");
  Serial.println(" ");
  Ethernet.begin(mac);
  if (Ethernet.begin(mac) == 1) {
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
  else {
    reconnect();
    Serial.println("Failed to configure Ethernet using DHCP");
  }

}


void reconnect() {
  Alarm.delay(10);

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
  Alarm.delay(10);

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
  Alarm.delay(10);

}


// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{ Alarm.delay(10);

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
  Alarm.delay(10);

}


