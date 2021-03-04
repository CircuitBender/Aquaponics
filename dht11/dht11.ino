
#include <dht.h>

#define dht_dpin 2 //no ; here. Set equal to channel sensor is on

dht DHT;

void setup(){
  Serial.begin(9600);
  delay(300);//Let system settle
  Serial.println("Humidity and temperature\n\n");
  delay(700);//Wait rest of 1000ms recommended delay before
  //accessing sensor
}//end "setup()"

void loop(){
  //This is the "heart" of the program.
  DHT.read11(dht_dpin);

    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    Serial.println("C  ");
  delay(800);//Don't try to access too frequently... in theory
  //should be once per two seconds, fastest,
  //but seems to work after 0.8 second.
}// end loop()
