
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <SPI.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <Digital_Light_TSL2561.h>

#include <Time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// +++++++++++++++++ Network Setup +++++++++++++++++
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xF7
};
IPAddress ip(192, 168, 50, 109);
IPAddress myDns(192, 168, 50, 1);
IPAddress gateway(192, 168, 50, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress mqtt_server(192, 168, 50, 104);

EthernetClient ethClient;
void callback();
void getNtpTime();
void printDigits();

PubSubClient client(mqtt_server, 1883, callback, ethClient);

char message_buff[100]; // this buffers our incoming messages so we can do something on certain commands
long lastMsg = 0;
int value = 0;


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



//DHT11 Sensor

const int DHT11_1PIN = 2;         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 

// DHT11 Sensor 1 Growbox
DHT_Unified dht(DHT11_1PIN, DHTTYPE);
uint32_t delayMS;

void dht11_setup(){
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

// Variables for printing temp and humidity

String temp_str; //see last code block below use these to convert the float that you get back from DHT to a string =str
String hum_str;
char dht11temp[50];
char dht11hum[50];

//setting up Ethernet 
void setup_ethernet() {
reconnect();
}



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
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

void reconnect() {
  unsigned long previousMillisreconnect = 0;
  const long interval_reconnect = 5000;         // Wait 5 seconds before retrying
  unsigned long currentMillisreconnect = millis();
  int connection_state = LOW;

  if (currentMillisreconnect - previousMillisreconnect >= interval_reconnect) {
    // save the last time you blinked the LED
    previousMillisreconnect = currentMillisreconnect;
    // Internet and NTP
    if (Ethernet.begin(mac) == 0) {
      // no point in carrying on, so do nothing forevermore:
      while (1) {
        Serial.println("Failed to configure Ethernet using DHCP");
        Alarm.delay(10000);
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

    // MQTT Reconnection
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    if (client.connect("arduinoClient", "marcoheinzen", "8506mz")) {
      Serial.println("MQTT Connected");
      client.publish("Aquaponics/MQTT/Response", "MQTT Connected");
    }

    else {
      Serial.println("MQTT Not Connected");
    }

    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect("arduinoClient", "marcoheinzen", "8506mz")) {
        Serial.println("MQTT connected");
        // Once connected, publish an announcement...
        client.publish("outTopic", "hello world");
        client.publish("Aquaponics/MQTT/MQTTARDUINO_OUT", "MQTT Connected");

        // ... and resubscribe
        client.subscribe("inTopic");
        client.subscribe("Aquaponics/MQTT/MQTTARDUINO_IN");

      }
      else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
    }
  }
}

void setup()
{
  Serial.begin(9600);
  setup_ethernet();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

    time_t prevDisplay = 0; // when the digital clock was displayed

//DHT11 Setup
  // Call out your data pin for your DHT sensor here
  dht11_setup();

}

//--(end setup )---


void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  dht11();
  client.loop();

 

}


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
   

   //this is where you get the data from the sensor

  float humiditydht11 = event.relative_humidity;
  float temperaturedht11 = event.temperature;


  //counter for the messages, see if I am missing any on the Mqtt broker

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;

    //Preparing for mqtt send

    temp_str = String(temperaturedht11); //converting ftemp (the float variable above) to a string
    temp_str.toCharArray(dht11temp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa...

    hum_str = String(humiditydht11); //converting Humidity (the float variable above) to a string
    hum_str.toCharArray(dht11hum, hum_str.length() + 1); //packaging up the data to publish to mqtt whoa...

    Serial.print("Publish message: ");//all of these Serial prints are to help with debuging

    client.publish("Aquaponics/Sensors/DHT11_Temp", dht11temp); //money shot
    client.publish("Aquaponics/Sensors/DHT11_Hum", dht11hum);
  }
}
/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime(){


  if (client.connect("arduinoClient", "marcoheinzen", "8506mz")) {
    Serial.println("MQTT Connected");
    client.publish("Aquaponics/MQTT/Response", "MQTT Connected");

  }
  else {
    Serial.println("MQTT Not Connected");
  }
  client.publish("Aquaponics/MQTT/Response", "MQTT Not Connected");

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  client.publish("Aquaponics/NTP/Response", "Transmit NTP Request");

  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      client.publish("Aquaponics/NTP/Response", "Received NTP Response");

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

  client.publish("Aquaponics/NTP/Response", "No NTP Response!");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  Scheduler &s = Scheduler::currentScheduler();
  Task &t = s.currentTask();
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



