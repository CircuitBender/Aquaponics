// demo of Grove - High Temperature Sensor
// Thmc -> A3
// RoomTemp -> A2
// Thmc -> A5
// RoomTemp -> A4


#include "High_Temp.h"

HighTemp ht1(A2, A3);
HighTemp ht2(A4, A5);


void setup()
{
    Serial.begin(9600);
    Serial.println("grove - high temperature sensor  1");
        Serial.println("");

    ht1.begin();
        Serial.println("grove - high temperature sensor  2");

        ht2.begin();
   Serial.println("");
}

void loop()
{
      Serial.println("grove - high temperature sensor  1:");

    Serial.println(ht1.getThmc());
    Serial.println("grove - high temperature sensor  2:");

        Serial.println(ht2.getThmc());

   Serial.println("");
       delay(1000);
}
