/* <<<<<<<<<<<< Aquaponics 1.06 by Marco Heinzen >>>>>>>>>>>>>>>>>>>
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

/**
 * This is a test of TaskScheduler layered priority funtionality
 * 
 * Current test employs two priority layers:
 * Base scheduler runs tasks t1, t2 and t3
 * High priority scheduler runs tasks t4 and t5
 * 
 * Sequence of task scheduling (not execution!) is:
 * 4, 5, 1, 4, 5, 2, 4, 5, 3 = one base scheduler pass
 * 
 * Scheduling overhead (at 20 micros per one pass) is: (B + B * H) * T = (3 + 3 * 2) * 18 = 162 micros
 * where
 *  B - number of tasks in the base scheduler's chain
 *  H - number of tasks in the high priority scheduler's chain
 *  T - scheduling overhead for 1 pass (~15-18 microseconds) 
 *  
 *  Actual task execution order:

Scheduler Priority Test
Task: 40:  0 Start delay = 0
Task: 50: 10  Start delay = 10
Task: 1:  21  Start delay = 21
Task: 2:  31  Start delay = 31
Task: 3:  41  Start delay = 41

Task: 40: 500 Start delay = 0
Task: 40: 1000  Start delay = 0
Task: 50: 1010  Start delay = 10
Task: 1:  1021  Start delay = 20
Task: 40: 1500  Start delay = 0
Task: 40: 2000  Start delay = 0
Task: 50: 2011  Start delay = 11
Task: 1:  2022  Start delay = 21
Task: 2:  2032  Start delay = 32
Task: 40: 2500  Start delay = 0
Task: 40: 3000  Start delay = 0
Task: 50: 3010  Start delay = 10
Task: 1:  3021  Start delay = 20
Task: 3:  3032  Start delay = 32

Task: 40: 3500  Start delay = 0
Task: 40: 4000  Start delay = 0
Task: 50: 4011  Start delay = 11
Task: 1:  4022  Start delay = 21
Task: 2:  4032  Start delay = 32
Task: 40: 4500  Start delay = 0
Task: 40: 5000  Start delay = 0
Task: 50: 5010  Start delay = 10
Task: 1:  5021  Start delay = 20
Task: 40: 5500  Start delay = 0
Task: 40: 6000  Start delay = 0
Task: 50: 6010  Start delay = 10
Task: 1:  6022  Start delay = 21
Task: 2:  6032  Start delay = 32
Task: 3:  6043  Start delay = 42

 */

// ++++++++++++++ Libraries used: +++++++++++++++++

#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <SPI.h>

#include <Wire.h>
#include <Digital_Light_TSL2561.h>

#include <Time.h>
#include <TimeLib.h>
// #include <TimeAlarms.h>
#include <TaskScheduler.h>


#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>



const int DHT11_1PIN = 2;         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 

// DHT11 Sensor 1 Growbox
DHT_Unified dht(DHT11_1PIN, DHTTYPE);
uint32_t delayMS;
/*
// ALARM // TIMER
AlarmId WACY;
AlarmId NTPUD;
*/
//************************* User Defined Variables ********************************************************//
/* 
const int LightsMorningONhour = 10; const int LightsMorningONminute = 0; const int LightsMorningONsec = 0;
const int LightsMorningOFFhour = 15; const int LightsMorningOFFminute = 0; const int LightsMorningOFFsec = 0;

const int LightsEveningONhour = 16; const int LightsEveningONminute = 0; const int LightsEveningONsec = 0;
const int LightsEveningOFFhour = 22; const int LightsEveningOFFminute = 0; const int LightsEveningOFFsec = 0;

const int WateringONhour = 10; const int WateringONminute = 0; const int WateringONsec = 0; 
const int WateringOFFhour = 22; const int WateringOFFminute = 0; const int WateringOFFsec =0;
*/
// ++++++++++++++++++ CYCLE TIMES ++++++++++++++++++++
const unsigned long WateringTime = 23023L;
const unsigned long FlowbackTime = 76019L;
const unsigned long CycleTime = 150000L;
const unsigned long NTPupdateTime = 180000L;
const unsigned long SensorCycle = 10013;

//  intervals for updating Display
const int intervallUpdate = 1000;



// each "event" gets its own tracking variable
unsigned long previousMillisUpdate1=0;
/*unsigned long previousMillisUpdate2=0;
unsigned long previousMillisUpdate3=0;
unsigned long previousMillisUpdate4=0;
unsigned long previousMillisWatering=0;
unsigned long previousMillisafterWatering=0;

 */

// ************ Water Sensor************ 


// +++++++++++++++++ Network Setup +++++++++++++++++
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xF7
};
IPAddress ip(192, 168, 50, 109);
IPAddress myDns(192, 168, 50, 1);
IPAddress gateway(192, 168, 50, 1);
IPAddress subnet(255, 255, 255, 0);
// ++++++++++++++++ Time +++++++++++++++++++++
// NTP Servers:
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov
/*ETH: 130.60.128.3
  130.60.64.51
  130.60.204.10 */

EthernetUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

// Timezones
const int timeZone = 2;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)


// Ultra Sonic Range Sensor
#define ECHOPIN 42        // Pin to receive echo pulse 
#define TRIGPIN 41        // Pin to send trigger pulse
 

// +++++++++++++++++++ RELAYS +++++++++++++++++++++
#define PumpeA_Relais1 22    // Pumpe Aquarium auf Relais 1
#define PumpeK_Relais2 23   // Pumpe Kreislauf auf Relais 2
#define LichtA_Relais3 24    // Licht Aquarium auf Relais 3
#define HeizstabA_Relais4 25  // Heizstab Aquarium auf Relais 4

// set pin numbers:
#define buttonKP 30     // the number of the pushbutton pin
// variables will change:
int buttonStateKP = 0;         // variable for reading the pushbutton status


//************ Temp Probe Related *********************************************//

//************ Thermometer with thermistor************ 

/*thermistor parameters:
 * RT0: 10 000 Ω
 * B: 3950 K +- 1.00%
 * T0:  25 C
 * +- 2%
 */

//These values are in the datasheet
const unsigned long RT0 = 10000L;   // Ω
const int B = 3950;      // K
//--------------------------------------


const int VCC = 5;    //Supply voltage
const int R = 10000;  //R=10KΩ

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
 
//##################################################################################
//-----------  Do not Replace R1 with a resistor lower than 300 ohms    ------------
//##################################################################################
// Setup EC Meter 
 
int R1= 1000;
int Ra=25; //Resistance of powering Pins

int ECPin= A2;
int ECPower =A3;
int ECGround=A4;

int EC1Pin= A5;
int EC1Ground=A7;
int EC1Power =A6;
 
//*********** Converting to ppm [Learn to use EC it is much better**************//
// Hana      [USA]        PPMconverion:  0.5
// Eutech    [EU]          PPMconversion:  0.64
//Tranchen  [Australia]  PPMconversion:  0.7
// Why didnt anyone standardise this?
 
float PPMconversion=0.64;
 
//*************Compensating for temperature ************************************//
//The value below will change depending on what chemical solution we are measuring
//0.019 is generaly considered the standard for plant nutrients [google "Temperature compensation EC" for more info
float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring
 
//********************** Cell Constant For Ec Measurements *********************//
//Mine was around 2.9 with plugs being a standard size they should all be around the same
//But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
float K=2.88;
 
//***************************** END Of Recomended User Inputs *****************************************************************//
 
 
float AirTemperature=0;
float WaterTemperature=0;
float EC=0;
float EC25 =0;
int ppm =0;
 
 
float raw= 0;
float Vin= 5;
float Vdrop= 0;
float Rc= 0;
float buffer=0; 
// +++++++++++++++++++++++++++ END OF VARIABLES ++++++++++++++++++++++++
// voids for Task Scheduler
// ++++++++++++++++++++TIMER EVENTS++++++++++++++++++++++++++++


#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_PRIORITY
#define _TASK_WDT_IDS
#define _TASK_TIMECRITICAL
void Sensors(){
/*   Scheduler &s = Scheduler::currentScheduler();
  Task &t = s.currentTask();
  
  Serial.print("Task: "); Serial.print(t.getId());Serial.print(":\t");
  Serial.print(millis()); Serial.print("\tStart delay = "); Serial.println(t.getStartDelay());
  delay(10);

  if (t.getId() == 3) Serial.println();*/
  readSensors();
}
void Watering(){
 /*  Scheduler &s = Scheduler::currentScheduler();
  Task &t = s.currentTask();
  
  Serial.print("Task: "); Serial.print(t.getId());Serial.print(":\t");
  Serial.print(millis()); Serial.print("\tStart delay = "); Serial.println(t.getStartDelay());
  delay(10);

  if (t.getId() == 3) Serial.println();*/
  abc();
}
void getTime(){
  /*Scheduler &s = Scheduler::currentScheduler();
  Task &t = s.currentTask();
  
  Serial.print("Task: "); Serial.print(t.getId());Serial.print(":\t");
  Serial.print(millis()); Serial.print("\tStart delay = "); Serial.println(t.getStartDelay());
  delay(10);

  if (t.getId() == 3) Serial.println();*/
  getNtpTime();
}
Scheduler r, hpr;
Task t1(SensorCycle, TASK_FOREVER, &Sensors, &r);
Task t2(CycleTime, TASK_FOREVER, &Watering, &hpr);
Task t3(NTPupdateTime, TASK_FOREVER, &getTime, &r);


// ++++++++++++++++++++++++++++ SETUP RUNS ONCE ++++++++++++++++++++++++

void setup(){  
  Wire.begin();
  Serial.begin(9600);
  TSL2561.init();

Serial.println("Aquaponics 1.06");
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
        Serial.println("Initializing Setup:");
        Serial.println("loading ... ");
        Serial.println(" ");
        delay(1000);

 // ++++++++++++++++ DIGITAL BUTTON ++++++++++++++++
  // initialize the pushbutton pin as an input:
        Serial.println("initializing Watering Button");
  pinMode(buttonKP, INPUT);

  // initialize Relais 1-4 as Output
        Serial.println(" ");
        Serial.println("initializing RELAYS");
        Serial.println("defining Outputs...");

  pinMode(PumpeA_Relais1, OUTPUT);
  pinMode(PumpeK_Relais2, OUTPUT);
  pinMode(LichtA_Relais3, OUTPUT);
  pinMode(HeizstabA_Relais4, OUTPUT);
  
        Serial.println(" ");
        Serial.println("setting all RELAYS: OFF");
  digitalWrite(PumpeA_Relais1, LOW);
        Serial.println("setting PumpeA_Relais1: LOW");
  digitalWrite(PumpeK_Relais2, LOW);
        Serial.println("setting PumpeK_Relais2: LOW");
  digitalWrite(LichtA_Relais3, LOW);
        Serial.println("setting LichtA_Relais3: LOW");
  digitalWrite(HeizstabA_Relais4, LOW);
        Serial.println("setting HeizstabA_Relais4: LOW");
        Serial.println("");
         delay(1000);        // delay in between reads for stability

  // set Relays to standard startup

        Serial.println("initializing RELAYS standard Setup");
        Serial.println(" ");
  digitalWrite(PumpeA_Relais1, HIGH);
        Serial.println("setting PumpeA_Relais1: HIGH");
  digitalWrite(PumpeK_Relais2, LOW);
        Serial.println("setting PumpeK_Relais2: LOW");
  digitalWrite(LichtA_Relais3, HIGH);
        Serial.println("setting LichtA_Relais3: HIGH");
  digitalWrite(HeizstabA_Relais4, HIGH);
        Serial.println("setting HeizstabA_Relais4: HIGH");
        Serial.println(" ");
        delay(1000);        // delay in between reads for stability
 
// Trying to connect the Ethernet:
        Serial.println("connecting to the Internet ...");
        Serial.println(" ");
 if (Ethernet.begin(mac) == 0) {
    // no point in carrying on, so do nothing forevermore:
    while (1) {
        Serial.println("Failed to configure Ethernet using DHCP");
        delay(10000);
    }
  }
        Serial.print("IP number assigned by DHCP is ");
        Serial.println(Ethernet.localIP());
        Serial.println(" ");
  Udp.begin(localPort);
        Serial.println("waiting for sync");
        setSyncProvider(getNtpTime);
        Serial.println(" ");
        Serial.println("successfully connected to the Internet !");
        Serial.println(" ");
  
  time_t prevDisplay = 0; // when the digital clock was displayed
/*
// ALARM TIMER RELAIS
// LIGHTS
Alarm.alarmRepeat(LightsMorningONhour, LightsMorningONminute, LightsMorningONsec, LightsMorningON); // Morning Aquarium Lights ON every day
Alarm.alarmRepeat(LightsMorningOFFhour, LightsMorningOFFminute, LightsMorningOFFsec, LightsMorningOFF); // Midday Aquarium Lights OFF every day
Alarm.alarmRepeat(LightsEveningONhour, LightsEveningONminute, LightsEveningONsec, LightsEveningON); // Afternoon Aquarium Lights ON every day
Alarm.alarmRepeat(LightsEveningOFFhour, LightsEveningONminute, LightsEveningOFFsec, LightsEveningOFF); // Evening Aquarium Lights OFF every day

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


        Serial.print("Take Sensor Reading every");
        Serial.print(SensorCycle / 1000);
        Serial.print(" seconds");
        Serial.println("");

         Serial.print("Update NTP Time every");
        Serial.print(NTPupdateTime / 1000);
        Serial.print(" seconds");
        Serial.println("");
*/
 
   t2.setId(40);
   r.setHighPriorityScheduler(&hpr); 
  r.enableAll(true); // this will recursively enable the higher priority tasks as well


  
// END OF ALARMS AND TIMERS
// INITIALIZING SENSORS:
        Serial.println("Initializing Sensors");
         
          dht.begin();
  Serial.println("DHT11 loading");
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
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

// Water Sensor Module

// Temperature Module 
  T0 = 25 + 273.15;                 //Temperature T0 from datasheet, conversion from Celsius to kelvin
  VRT1 = analogRead(A0);              //Acquisition analog value of VRT
  VRT2 = analogRead(A1);              //Acquisition analog value of VRT

 // EC Modules
 
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);//Setting pin for sourcing current
  pinMode(ECGround,OUTPUT);//setting pin for sinking current
  digitalWrite(ECGround,LOW);//We can leave the ground connected permanantly
 
  Alarm.delay(1000);// gives sensor time to settle

  pinMode(EC1Pin,INPUT);
  pinMode(EC1Power,OUTPUT);//Setting pin for sourcing current
  pinMode(EC1Ground,OUTPUT);//setting pin for sinking current
  digitalWrite(EC1Ground,LOW);//We can leave the ground connected permanantly
 
  Alarm.delay(1000);// gives sensor time to settle
  
  //** Adding Digital Pin Resistance to [25 ohm] to the static Resistor *********//
  // Consule Read-Me for Why, or just accept it as true
  R1=(R1+Ra);// Taking into acount Powering Pin Resitance

 // Setup Text
  Serial.println("");
  Serial.println("Make sure EC-Probes and Water Temp Sensor are in Solution and solution is well mixed");
  Serial.println("all Measurments at 10's Second intervals [Dont read Ec morre than once every 5 seconds]:");
  Serial.println("");

// Ultra Sonic Range Sensor 
  Serial.println("initializing Ultra Sonic Range Sensor");
  pinMode(ECHOPIN, INPUT); 
  pinMode(TRIGPIN, OUTPUT); 
  Serial.println("");

  Serial.println("SUCCESS !");
  Serial.println("End of Setup !");
  Serial.println("Starting Main Loop:");
  Serial.println("");
  Serial.println("");
}

// END OF SETUP

// ++++++++++++ MAIN LOOP ++++++++++++

void loop(){
 r.execute();

 // runner.execute();
   unsigned long currentMillis1 = millis();
 /*  unsigned long currentMillis2 = millis();
   unsigned long currentMillis3 = millis();
   unsigned long currentMillis4 = millis();
 */
   // time to update ?
   if ((unsigned long)(currentMillis1 - previousMillisUpdate1) >= intervallUpdate) {
      
    digitalClockDisplay();
    previousMillisUpdate1 = currentMillis1;
   }
 /*  if ((unsigned long)(currentMillis2 - previousMillisUpdate2) >= SensorCycle) {
    digitalClockDisplay();
    readSensors();
    previousMillisUpdate2 = currentMillis2;
   }
 if ((unsigned long)(currentMillis3 - previousMillisUpdate3) >= CycleTime) {
    digitalClockDisplay();
    abc();
    previousMillisUpdate3 = currentMillis3;
   }
 if ((unsigned long)(currentMillis4 - previousMillisUpdate4) >= NTPupdateTime) {
    digitalClockDisplay();
    getNtpTime();
    previousMillisUpdate4 = currentMillis4;
   }*/
 
}

// ++++++++++++ END OF MAIN LOOP ++++++++++++
// +++++++++++++++SUBROUTINES+++++++++++++++++
// ALARM ROUTINES:

void WateringOFF (){
}

void WateringON (){
     
}

void LightsMorningON () {
  Serial.print("Good Morning ! It is ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println("");
  Serial.print("Turning Lights ON");
  digitalWrite(PumpeA_Relais1, HIGH);
  Serial.println("setting PumpeA_Relais1: HIGH");
  digitalWrite(PumpeK_Relais2, LOW);
  Serial.println("setting PumpeK_Relais2: LOW");
  digitalWrite(LichtA_Relais3, HIGH);
  Serial.println("setting LichtA_Relais3: HIGH");
  digitalWrite(HeizstabA_Relais4, HIGH);
  Serial.println("setting HeizstabA_Relais4: HIGH");

}

void LightsMorningOFF () {
  Serial.print("It is ");
  Serial.println(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print("Turning Lights OFF");
  digitalWrite(LichtA_Relais3, LOW);
  Serial.println("setting LichtA_Relais3: LOW");

}

void LightsEveningON () {
  Serial.print("It is ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println("");
  Serial.println("Turning Lights ON");
  digitalWrite(LichtA_Relais3, HIGH);
  Serial.println("setting LichtA_Relais3: HIGH");
}

void LightsEveningOFF () {
  Serial.print("It is ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println("");
  Serial.print("Turning Lights OFF");
  digitalWrite(LichtA_Relais3, LOW);
  Serial.println("setting LichtA_Relais3: LOW");
}
/*
void t1t3enable(){
    t1.enable();
    t3.enable();
    }
    void t1t3disable(){
        t1.disable();
    t3.disable();
    }
void t1disable(){
  t1.disable();
}
void t1enable(){
  t1.enable();
}
void t2disable(){
  t2.disable();
}
void t2enable(){
  t2.enable();
}  
void t3disable(){
  t3.disable();
}
void t3enable(){
  t3.enable();
}*/

// EVENT ROUTINE
void abc (){
 
    getNtpTime();
    Serial.print("It is ");
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.println("");
    Serial.print("Watering Plants for: ");
    Serial.print (WateringTime / ((unsigned)1000));
    Serial.println ("  Seconds");
    digitalWrite(PumpeA_Relais1, LOW);
    digitalWrite(PumpeK_Relais2, HIGH);
    int x = 0;
do {
   unsigned long currentMillisWatering = millis();
   if ((unsigned long)(currentMillisWatering - previousMillisWatering) >= intervallUpdate) {
   
  digitalClockDisplay();
  UltraSonic();
  x++;    
  previousMillisWatering = currentMillisWatering;
   }
  } 
  while(x < (WateringTime / ((unsigned)1000)));
  
    digitalWrite(PumpeK_Relais2, LOW);
    Serial.println("Watering Plants STOP");
    Serial.print("Watered Plants for: ");
    Serial.print (WateringTime / ((unsigned)1000));
    Serial.println ("  Seconds");
    Serial.print("Waiting: ");
    Serial.print (FlowbackTime / ((unsigned)1000));
    Serial.println ("  Seconds to let Water Flow back");
    int y = 0;
do {
   unsigned long currentMillisafterWatering = millis();
   if ((unsigned long)(currentMillisafterWatering - previousMillisafterWatering) >= intervallUpdate) {
   
  digitalClockDisplay();
  UltraSonic();
  y++;    
  previousMillisafterWatering = currentMillisafterWatering;
   }
  } 
  while(y < (FlowbackTime / (unsigned)1000)); 
    
    Serial.print("Waited: ");
    Serial.print (FlowbackTime / (unsigned)1000);
    Serial.println ("  Seconds to let Water Flow back");
    Serial.println("Turning Aquarium Pump PumpeA_Relais1 ON ");
    digitalWrite(PumpeA_Relais1, HIGH);
    Serial.println("Aquarium Pump PumpeA_Relais1 is ON ");
    Serial.print("Wating for:");
    Serial.print((CycleTime - WateringTime - FlowbackTime)  / (unsigned)1000);
    Serial.println(" Seconds to activate the next Cycle");
    }
    
// AFTER EVENT ROUTINE 

// +++++++++++++++++++++Sensors Sub++++++++++++++++++++++++++++++++++
// read sensors
void readSensors(){ 
    UltraSonic();
    dht11();
    LightSensor();

    GetWaterTEMP();
    GetAirTEMP();
    PrintECReadings();
}


/*-------- NTP code ----------*/

  const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

  time_t getNtpTime()
  {
 
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
  }

  // send an NTP request to the time server at the given address
  void sendNTPpacket(IPAddress &address)
  {
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
  distance= distance/58; 
  float WaterLevel =0;
  WaterLevel = (unsigned)38 -distance;
  float WaterVolume; 
  WaterVolume = WaterLevel *((unsigned) 60 * 35);
  float WaterLitres;
  WaterLitres = WaterVolume / (unsigned)1000;

/* The speed of sound is 340 m/s or 29 us per cm.The Ultrasonic burst travels out & back.So to find the distance of object we divide by 58  */
 
  
  if (distance <= 9) {
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
  Serial.print(" cm Waterlevel (29cm MAX) // ");
  Serial.print(distance); 
  Serial.println(" cm von Decke (9 cm MAX)");
  
  Serial.print(WaterLitres); 
  Serial.print(" l Liter Wasser (");
  Serial.print(WaterVolume); 
  Serial.println(" cm^2 (60900 cm^2 MAX)");
  }
}

// DHT11 SUBROUTINE
void dht11() {

  // Get temperature event and print its value.
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("DHT11 Error reading temperature!");
  }
  else {
    Serial.print(event.temperature);
    Serial.println(" °C Air Temp (DHT11)");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("DHT11 Error reading humidity!");
  }
  else {
    Serial.print(event.relative_humidity);
    Serial.println(" %  Humidity (DHT11)");
  }
  }
  
  void LightSensor(){
  // Light Sensor
    Serial.print(TSL2561.readVisibleLux());
    Serial.println(" Lux (Light Value TSL2561)");
  }
    
  

//*********Reading Temperature Of Solution *******************//

void GetWaterTEMP(){
VRT1 = analogRead(A0);              //Acquisition analog value of VRT
  VRT1 = (5.00 / 1023.00) * VRT1;      //Conversion to voltage
  VR1 = VCC - VRT1;
  RT1 = VRT1 / (VR1 / R);               //Resistance of RT

  ln = log(RT1 / RT0);
  TX1 = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor

  TX1 = TX1 - 273.15;                 //Conversion to Celsius
  WaterTemperature = TX1 -11; // Calibration
Serial.print(WaterTemperature);
Serial.println(" °C (Thermistor Water Temperature) ");


}; 

//*********Reading Air Temperature  *******************//
void GetAirTEMP(){
VRT2 = analogRead(A1);              //Acquisition analog value of VRT
  VRT2 = (5.00 / 1023.00) * VRT2;      //Conversion to voltage
  VR2 = VCC - VRT2;
  RT2 = VRT2 / (VR2 / R);               //Resistance of RT

  ln = log(RT2 / RT0);
  TX2 = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor

  TX2 = TX2 - 273.15;                 //Conversion to Celsius
  AirTemperature = TX2-10;
Serial.print(AirTemperature);
Serial.println(" °C (Thermistor Air Temperature)");


}; 

//*********Reading EC Of Solution *******************//

void GetEC(){

 
//************Estimates Resistance of Liquid ****************//
digitalWrite(ECPower,HIGH);
raw= analogRead(ECPin);
raw= analogRead(ECPin);// This is not a mistake, First reading will be low beause if charged a capacitor
digitalWrite(ECPower,LOW);
 

 
 
//***************** Converts to EC **************************//
Vdrop= (Vin*raw)/1024.0;
Rc=(Vdrop*R1)/(Vin-Vdrop);
Rc=Rc-Ra; //acounting for Digital Pin Resitance
EC = 1000/(Rc*K);
 
 
//*************Compensating For Temperaure********************//
EC25  =  EC/ (1+ TemperatureCoef*(WaterTemperature-25.0));
ppm=(EC25)*(PPMconversion*1000);
 
 
;}
//************************** End OF EC Function ***************************//
//***This Loop Is called From Main Loop- Prints to serial usefull info ***//
void PrintECReadings(){
    GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
//  GetEC1();
Serial.print("Rc: ");
Serial.print(Rc);
Serial.print(" EC: ");
Serial.print(EC25);
Serial.print(" Siemens (SI)  ");
Serial.print("ppm: ");
Serial.print(ppm);
Serial.print(" at ");
Serial.print(WaterTemperature);
Serial.println(" °C Water Temperature");

 
 
/*
//********** Usued for Debugging ************
Serial.print("Vdrop: ");
Serial.println(Vdrop);
Serial.print("Rc: ");
Serial.println(Rc);
Serial.print(EC);
Serial.println("Siemens");
//********** end of Debugging Prints *********
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

void printDigits(int digits) {
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

