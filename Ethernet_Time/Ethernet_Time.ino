#include <Time.h>
#include <TimeLib.h>

//sample code originated at http://www.openreefs.com/ntpServer
//modified by Steve Spence, http://arduinotronics.blogspot.com

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

/* ******** Ethernet Card Settings ******** */
// Set this to your Ethernet Card Mac Address
byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xb3, 0xF7 };

/* ******** NTP Server Settings ******** */
/* us.pool.ntp.org NTP server
   (Set to your time server of choice) */
IPAddress timeServer(216, 23, 247, 62);

/* Set this to the offset (in seconds) to your local time
   This example is GMT - 4 */
const long timeZoneOffset = -14400L; 

/* Syncs to NTP server every 15 seconds for testing,
   set to 1 hour or more to be reasonable */
unsigned int ntpSyncTime = 3600;       


/* ALTER THESE VARIABLES AT YOUR OWN RISK */
// local port to listen for UDP packets
unsigned int localPort = 8888;
// NTP time stamp is in the first 48 bytes of the message
const int NTP_PACKET_SIZE= 48;     
// Buffer to hold incoming and outgoing packets
byte packetBuffer[NTP_PACKET_SIZE]; 
// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;                   
// Keeps track of how long ago we updated the NTP server
unsigned long ntpLastUpdate = 0;   
// Check last time clock displayed (Not in Production)
time_t prevDisplay = 0;           

void setup() {
   Serial.begin(9600);
  
   // Ethernet shield and NTP setup
   int i = 0;
   int DHCP = 0;
   DHCP = Ethernet.begin(mac);
   //Try to get dhcp settings 30 times before giving up
   while( DHCP == 0 && i < 30){
     delay(1000);
     DHCP = Ethernet.begin(mac);
     i++;
   }
   if(!DHCP){
    Serial.println("DHCP FAILED");
     for(;;); //Infinite loop because DHCP Failed
   }
   Serial.println("DHCP Success");
  
   //Try to get the date and time
   int trys=0;
   while(!getTimeAndDate() && trys<10) {
     trys++;
   }
}

// Do not alter this function, it is used by the system
int getTimeAndDate() {
   int flag=0;
   Udp.begin(localPort);
   sendNTPpacket(timeServer);
   delay(1000);
   if (Udp.parsePacket()){
     Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer
     unsigned long highWord, lowWord, epoch;
     highWord = word(packetBuffer[40], packetBuffer[41]);
     lowWord = word(packetBuffer[42], packetBuffer[43]); 
     epoch = highWord << 16 | lowWord;
     epoch = epoch - 2208988800 + timeZoneOffset;
     flag=1;
     setTime(epoch);
     ntpLastUpdate = now();
   }
   return flag;
}

// Do not alter this function, it is used by the system
unsigned long sendNTPpacket(IPAddress& address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;                 
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket();
}

// Clock display of the time and date (Basic)
void clockDisplay(){
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

// Utility function for clock display: prints preceding colon and leading 0
void printDigits(int digits){
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

// This is where all the magic happens...
void loop() {
    // Update the time via NTP server as often as the time you set at the top
    if(now()-ntpLastUpdate > ntpSyncTime) {
      int trys=0;
      while(!getTimeAndDate() && trys<10){
        trys++;
      }
      if(trys<10){
        Serial.println("ntp server update success");
      }
      else{
        Serial.println("ntp server update failed");
      }
    }
  
    // Display the time if it has changed by more than a second.
    if( now() != prevDisplay){
      prevDisplay = now();
      clockDisplay(); 
    }
}
