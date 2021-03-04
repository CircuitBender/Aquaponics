
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
// ++++++++++++++ Libraries used: +++++++++++++++++
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <SPI.h>

#include <Time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include "Timer.h"

// ALARM // TIMER
AlarmId WACY;
AlarmId NTPUD;
Timer t;


const int LightsMorningONhour = 10;
const int LightsMorningONminute = 0;
const int LightsMorningONsec = 0;
const int LightsMorningOFFhour = 15;
const int LightsMorningOFFminute = 0;
const int LightsMorningOFFsec = 0;

const int LightsEveningONhour = 16;
const int LightsEveningONminute = 0;
const int LightsEveningONsec = 0;
const int LightsEveningOFFhour = 22;
const int LightsEveningOFFminute = 0;
const int LightsEveningOFFsec = 0;

const int WateringONhour = 10;
const int WateringONminute = 0;
const int WateringONsec = 0;
const int WateringOFFhour = 22;
const int WateringOFFminute = 0;
const int WateringOFFsec =0;

const long WateringTime = 26000L;
const long FlowbackTime = 74000L;
const long CycleTime = 180000L;
// const long NTPupdateTime = 60L*1000L;

// Timer Variables
const int WateringEvent;
const int afterWateringEvent;
const int NTPrefresh;

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



// +++++++++++++++++++ RELAYS +++++++++++++++++++++
#define PumpeA_Relais1 22    // Pumpe Aquarium auf Relais 1
#define PumpeK_Relais2 23   // Pumpe Kreislauf auf Relais 2
#define LichtA_Relais3 24    // Licht Aquarium auf Relais 3
#define HeizstabA_Relais4 25  // Heizstab Aquarium auf Relais 4

// constants won't change. They're used here to
// set pin numbers:
#define buttonKP 30     // the number of the pushbutton pin

// variables will change:
int buttonStateKP = 0;         // variable for reading the pushbutton status

// +++++++++++++++++++++++++++ END OF VARIABLES ++++++++++++++++++++++++
// ++++++++++++++++++++++++++++ SETUP RUNS ONCE ++++++++++++++++++++++++
void setup()
{
  Serial.begin(9600);

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

   const     int WateringEvent = t.every((CycleTime )*1L, abc);
        Serial.print("Cycle Time: ");
        Serial.print(CycleTime / 1000);
        Serial.println(" Seconds");
        Serial.print(" second Cycle started id=");
        Serial.println(WateringEvent);
//        const long afterwateringtime = (WateringTime+FlowbackTime)*1L;
 const int afterWateringEvent = t.after((WateringTime+FlowbackTime)*1L, xyz);
        Serial.print("Flowback Time: ");
        Serial.print(FlowbackTime / 1000);
        Serial.println(" Seconds");
        Serial.print("After Watering event id=");
        Serial.println(afterWateringEvent); 

   /*     int NTPrefresh = t.every(NTPupdateTime, getNtpTime);
        Serial.print("NTP Update Cycle Time:");
        Serial.print(NTPupdateTime / 1000);
        Serial.println(" Seconds");
        Serial.print("NTP Update/Refresh ID event id=");
        Serial.println(NTPrefresh); 
        Serial.println("");
        Serial.println(""); */
        
//Watering Cycle
Alarm.alarmRepeat(WateringONhour, WateringONminute, WateringONsec, WateringON); // 09:05 every day
        Serial.print("Watering ON at: ");
        Serial.print(WateringONhour);
        Serial.print(":");
        Serial.print(WateringONminute);
        Serial.print(":");
        Serial.println(WateringONsec);
 
Alarm.alarmRepeat(WateringOFFhour, WateringOFFminute, WateringOFFsec, WateringOFF); // 21:30  every day
        Serial.print("Watering OFF at: ");
        Serial.print(WateringOFFhour);
        Serial.print(":");
        Serial.print(WateringOFFminute);
        Serial.print(":");
        Serial.println(WateringOFFsec);
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
  digitalClockDisplay();
  Alarm.delay(1000); // wait one second between clock display
  t.update();
}
// ++++++++++++ END OF MAIN LOOP ++++++++++++

// SUBROUTINES


void checkWateringHour (){
/* if (WateringONhour >= hour() && hour()<= WateringOFFhour) {
WateringON ();
 }
 else {
WateringOFF ();
}; */
}

void NTPupdate () {
  getNtpTime();
}

void WateringOFF (){
 // t.stop(WateringEvent);
}

void WateringON (){
/*int WateringEvent = t.every(CycleTime, Watering);
        Serial.print(CycleTime / 1000);
        Serial.print(" second Cycle started id=");
        Serial.println(WateringEvent);
        
int afterWateringEvent = t.after(FlowbackTime, doAfterWatering);
        Serial.print("Flowback Time: ");
        Serial.print(FlowbackTime / 1000);
        Serial.println(" Seconds");
        Serial.print("After Watering event id=");
        Serial.println(afterWateringEvent); 
        */
}


void abc (){
 //   t.stop(afterWateringEvent);
    getNtpTime();
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
    Alarm.delay(WateringTime);        // delay in between reads for stability
    digitalWrite(PumpeK_Relais2, LOW);
    Serial.println("Watering Plants STOP");
    Serial.print("Watered Plants for: ");
    Serial.print (WateringTime / 1000);
    Serial.println ("  Seconds");
    Serial.print("Waiting: ");
    Serial.print (FlowbackTime / 1000);
    Serial.println ("  Seconds to let Water Flow back");
    }
    
void xyz(){   
 //   getNtpTime();
    Serial.print("Waited: ");
    Serial.print (FlowbackTime / 1000);
    Serial.println ("  Seconds to let Water Flow back");
    Serial.println("Turning Aquarium Pump PumpeA_Relais1 ON ");
    digitalWrite(PumpeA_Relais1, HIGH);
    Serial.println("Aquarium Pump PumpeA_Relais1 is ON ");
    Serial.print("Wating for:");
    Serial.print((CycleTime - WateringTime - FlowbackTime)  / 1000);
    Serial.println(" Seconds to the next Cycle");
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
  Alarm.delay(1000);        // delay in between reads for stability

}

void LightsMorningOFF () {
  Serial.print("It is ");
  Serial.println(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print("Turning Lights OFF");
  digitalWrite(LichtA_Relais3, LOW);
  Serial.println("setting LichtA_Relais3: LOW");
  Alarm.delay(1000);        // delay in between reads for stability

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


/*
void WateringPlants  () {
  if (buttonStateKP == HIGH) {
      

    delay(CycleTime);
  }
  else {
    Alarm.delay(50);        // delay in between reads for stability
  };
}

void pushbutton () {
  // read the state of the pushbutton value:

  buttonStateKP = digitalRead(buttonKP);



  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonStateKP == HIGH) {
    // turn LED on:
    Serial.print("Watering Plants for: ");
    Serial.print (WateringTime / 1000);
    Serial.println ("  Seconds");
    digitalWrite(PumpeA_Relais1, LOW);
    digitalWrite(PumpeK_Relais2, HIGH);
    Alarm.delay(WateringTime);        // delay in between reads for stability
    digitalWrite(PumpeK_Relais2, LOW);
    Serial.println("Watering Plants STOP");
    Serial.print("Watered Plants for: ");
    Serial.print (WateringTime / 1000);
    Serial.println ("  Seconds");
    Serial.print("Waiting: ");
    Serial.print (FlowbackTime / 1000);
    Serial.println ("  Seconds to let Water Flow back");
    Alarm.delay(FlowbackTime);        // delay in between reads for stability
    Serial.print("Waited: ");
    Serial.print (FlowbackTime / 1000);
    Serial.println ("  Seconds to let Water Flow back");
    Serial.println("Turning Aquarium Pump PumpeA_Relais1 ON ");
    digitalWrite(PumpeA_Relais1, HIGH);
    Serial.println("Aquarium Pump PumpeA_Relais1 is ON ");
  } else {
    digitalWrite(PumpeK_Relais2, LOW);
    digitalWrite(PumpeA_Relais1, HIGH);

  };
  if (buttonStateKP == LOW) {
    // turn LED on:
    digitalWrite(PumpeK_Relais2, LOW);
    digitalWrite(PumpeA_Relais1, HIGH);

  } else {
    digitalWrite(PumpeK_Relais2, LOW);

  };
};
*/
/*  void ManualWatering (){
  
  if (buttonStateKP == HIGH) {
     Serial.println ("WateringCycle manually activated");
   int WateringEvent = t.every(CycleTime, Watering);
        Serial.print(CycleTime / 1000);
        Serial.print(" second Cycle started id=");
        Serial.println(WateringEvent);
  }
}

*/
