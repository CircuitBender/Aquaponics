// print out the value you read:
  Serial.print(WaterLevelSensor);
  Serial.print(" ");
delay(50);        // delay in between reads for stability
if (WaterLevelSensor >= 480) { 
    digitalWrite(PumpeKreislauf_Relais2,LOW);
    delay(50);        // delay in between reads for stability
    digitalWrite(PumpeAquarium_Relais1,HIGH);
    Serial.println("Grow Tank FULL - Circuit Pump OFF - Aquarium Pump ON");}
else { 
    digitalWrite(PumpeKreislauf_Relais2,HIGH);
    delay(50);        // delay in between reads for stability

    digitalWrite(PumpeAquarium_Relais1,LOW);

    Serial.println("Watering Plants");};
