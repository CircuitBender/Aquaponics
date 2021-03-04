
  void water_change(){      //Turns off pump and other stuff so I can complete a Water Change
    
    digitalWrite(Heater, LOW); 
    digitalWrite(Return_Pump, HIGH); 
    delay(500); 
    digitalWrite(Skimmer, HIGH); 
    delay(500); 
    digitalWrite(PH_Pump, LOW); 
    delay(250); 
    digitalWrite(Worklight, HIGH);
    delay(250); 
    digitalWrite(Back_Fan, HIGH); 
    delay(250);
    digitalWrite(SumpLight, HIGH);
    delay(250);
    
    while (digitalRead(Return_Pump) == HIGH){
      
      //Do Nothing so I can clean the water
      //Have to manual reset the arduino with the press of reset button
  }   
  }
