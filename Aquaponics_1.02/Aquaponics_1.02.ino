/* <<<<<<<<<<<< Aquaponics 1.0 by Marco Heinzen >>>>>>>>>>>>>>>>>>>
 *    _ _
 *   _{ ' }_
 *  { `.!.` }
 *  ',_/Y\_,'
 *    {_,_}
 *      |
 *    (\|  /)
 *     \| //
 *      |//
 *   \\ |/  //
 *^^^^^^^^^^^^^^^
 *
 *   |\   \\\\__     o
|*   |\_/    o \o    o 
>*   |_   (( <==> oo  
|*   |/ \__+___/      
|*   /     |/
 *------------------------------------------------------
 */


// Libraries used:
#include <Dhcp.h>
#include <Dns.h> 
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <SPI.h>

#include <Time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>

// Network Setup
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xF7
};
IPAddress ip(192, 168, 50, 109);
IPAddress myDns(192,168,50, 1);
IPAddress gateway(192, 168, 50, 1);
IPAddress subnet(255, 255, 255, 0);// NTP Servers:

// ++++++++++++++++ Time +++++++++++++++++++++
// NTP
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov
/*ETH: 130.60.128.3 
130.60.64.51  
130.60.204.10 */

const int timeZone = 2;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)


EthernetUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets
// ++++++++++ RELAYS ++++++++++++
#define PumpeA_Relais1 22    // Pumpe Aquarium auf Relais 1
#define PumpeK_Relais2 23   // Pumpe Kreislauf auf Relais 2
#define LichtA_Relais3 24    // Licht Aquarium auf Relais 3
#define HeizstabA_Relais4 25  // Heizstab Aquarium auf Relais 4

// constants won't change. They're used here to
// set pin numbers:
#define buttonKP 30     // the number of the pushbutton pin

// variables will change:
int buttonStateKP = 0;         // variable for reading the pushbutton status
const long WateringTime = 26000L;
const long FlowbackTime = 74000L;
const long CycleTime = 720000L;

// ALARM // TIMER 
AlarmId Aid;

void setup() 
{
  Serial.begin(9600);
  
  Serial.println("aquaponics 1.01");
  Serial.println("loading ... ");

  // ++++++++++++++++ DIGITAL BUTTON ++++++++++++++++
// initialize the pushbutton pin as an input:
  Serial.println("initializing Watering Button");

  pinMode(buttonKP, INPUT);

// initialize Relais 1-4 as Output
  Serial.println("initializing RELAYS");
  Serial.println("setting all RELAYS: OFF");

  pinMode(PumpeA_Relais1, OUTPUT);
  pinMode(PumpeK_Relais2, OUTPUT);
  pinMode(LichtA_Relais3, OUTPUT);
  pinMode(HeizstabA_Relais4, OUTPUT);
  delay(500);        // delay in between reads for stability

// set Relays to standard startup
  
  Serial.println("initializing RELAYS standard Setup");
  
 
  digitalWrite(PumpeA_Relais1,HIGH);
  Serial.println("setting PumpeA_Relais1: HIGH");
  digitalWrite(PumpeK_Relais2,LOW);
  Serial.println("setting PumpeK_Relais2: LOW");
  digitalWrite(LichtA_Relais3,HIGH);
  Serial.println("setting LichtA_Relais3: HIGH");
  digitalWrite(HeizstabA_Relais4,HIGH);
  Serial.println("setting HeizstabA_Relais4: HIGH");
  delay(1000);        // delay in between reads for stability

// Trying to (re)connect the Ethernet:
  Serial.println("connecting to the Internet ...");

  if (Ethernet.begin(mac) == 0) {
    // no point in carrying on, so do nothing forevermore:
    while (1) {
      Serial.println("Failed to configure Ethernet using DHCP");
      delay(10000);
    }
  }
  Serial.print("IP number assigned by DHCP is ");
  Serial.println(Ethernet.localIP());
  Udp.begin(localPort);
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  Serial.println("successfully connected to the Internet !");
  Serial.println("End of Setup !");
  Serial.println("Starting Main Loop:");



}

time_t prevDisplay = 0; // when the digital clock was displayed

// ALARM TIMER RELAIS
// LIGHTS 
  Alarm.alarmRepeat(9,0,0, LightsMorningON);  // Morning Aquarium Lights ON every day
  Alarm.alarmRepeat(14,0,0, LightsMorningOFF);  // Midday Aquarium Lights OFF every day
  Alarm.alarmRepeat(15,0,0, LightsEveningON);  // Afternoon Aquarium Lights ON every day
  Alarm.alarmRepeat(21,30,0, LightsEveningOFF);  // Evening Aquarium Lights OFF every day
  
// Watering Cycle 
  Alarm.alarmRepeat(9,5,0, WateringON);  // 09:05 every day
  Alarm.alarmRepeat(21,30,0, WateringOFF);  // 21:30  every day
  Aid =  Alarm.timerRepeat(CycleTime, WateringCycle);      // timer for the ebb/ flow cycle




// END OF SETUP

// ++++++++++++ MAIN LOOP ++++++++++++
void loop()
{ 
  digitalClockDisplay();
  Alarm.delay(1000); // wait one second between clock display
  WateringPlants(); 



}

// SUBROUTINES

void LightsMorningON (){
    Serial.print("Good Morning ! It is ");
    Serial.println(now);
    Serial.print("Turning Lights ON");
  digitalWrite(PumpeA_Relais1,HIGH);
    Serial.println("setting PumpeA_Relais1: HIGH");
  digitalWrite(PumpeK_Relais2,LOW);
    Serial.println("setting PumpeK_Relais2: LOW");
  digitalWrite(LichtA_Relais3,HIGH);
    Serial.println("setting LichtA_Relais3: HIGH");
  digitalWrite(HeizstabA_Relais4,HIGH);
    Serial.println("setting HeizstabA_Relais4: HIGH");
    
    delay(1000);        // delay in between reads for stability

}

void LightsMorningOFF (){
    Serial.print("It is ");
    Serial.println(now);
    Serial.print("Turning Lights OFF");
    digitalWrite(LichtA_Relais3,LOW);
    Serial.println("setting LichtA_Relais3: LOW");
}

void LightsEveningON (){
    Serial.print("It is ");
    Serial.println(now);
    Serial.print("Turning Lights ON");
    digitalWrite(LichtA_Relais3,LOW);
    Serial.println("setting LichtA_Relais3: HIGH");
}

void LightsEveningOFF (){
    Serial.print("It is ");
    Serial.println(now);
    Serial.print("Turning Lights OFF");
    digitalWrite(LichtA_Relais3,LOW);
    Serial.println("setting LichtA_Relais3: LOW");
}

void WateringPlants  (){
  pushbutton();
  if (buttonStateKP == HIGH) {
    delay(CycleTime); 
  }
  else {
  delay(50);        // delay in between reads for stability
  };
}
void pushbutton (){  
// read the state of the pushbutton value:

  buttonStateKP = digitalRead(buttonKP);

 
 
    // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonStateKP == HIGH) {
    // turn LED on:
    Serial.println("Watering Plants for: ");
    Serial.print (WateringTime / 1000);
    Serial.println ("  Seconds");
    digitalWrite(PumpeA_Relais1, LOW);
    digitalWrite(PumpeK_Relais2, HIGH);
    delay(WateringTime);        // delay in between reads for stability
    Serial.println("Watering Plants STOP");
    digitalWrite(PumpeK_Relais2, LOW);
    delay(FlowbackTime);        // delay in between reads for stability
    digitalWrite(PumpeA_Relais1, HIGH);
    Serial.println("Watered Plants for: ");
    Serial.print (WateringTime / 1000);
    Serial.println ("  Seconds");
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

// DIGITAL CLOCK AND NTP SUBROUTINES

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print("NTP: Time:");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print("Date: ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
*/
/*-------- NTP code ----------*/
/*
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
*/
