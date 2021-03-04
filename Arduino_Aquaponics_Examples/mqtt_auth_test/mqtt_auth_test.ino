#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet2.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp2.h>
#include <Twitter.h>
#include <util.h>

/*
 Basic MQTT example with Authentication

  - connects to an MQTT server, providing username
    and password
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/

#include <SPI.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xF7
};
IPAddress ip(192, 168, 50, 109);
IPAddress myDns(192, 168, 50, 1);
IPAddress gateway(192, 168, 50, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress server(192, 168, 50, 104);

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

void setup()
{
  Serial.begin(9600);
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
  
  // Note - the default maximum packet size is 128 bytes. If the
  // combined length of clientId, username and password exceed this,
  // you will need to increase the value of MQTT_MAX_PACKET_SIZE in
  // PubSubClient.h
      Serial.print("Ethernet Started");

  if (client.connect("arduinoClient", "marcoheinzen", "8506mz")) {
    Serial.print("Connected");
    client.publish("Sensors","8 °C");
        Serial.print("Client Published");

    client.subscribe("ExCom");
  }
  else {        Serial.print("MQTT Connection failed");

}
}
void loop()
{
if (client.connect("arduinoClient", "marcoheinzen", "8506mz")) {
    Serial.print("Connected");
    client.publish("Sensors","8 °C");
        Serial.print("Client Published");

    client.subscribe("ExCom");
  }
  else {        Serial.print("MQTT Connection failed");

} delay(10000);
}
