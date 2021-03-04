

void feed(){
  
      delay(500); 
      digitalWrite(Return_Pump, HIGH);      //Turns off the pump
      digitalWrite(Heater, LOW);     //Turns Heater OFF 
      delay(1000); 
      digitalWrite(Skimmer, HIGH);      //turns off skimmer
      lcd.setBacklight(HIGH);         //digitalWrite(LcdLight, HIGH);
      lcd2.setBacklight(HIGH);        //digitalWrite(Lcd2Light, HIGH);
      lcd2.setCursor(12,3);
      lcd2.print("Feed ON "); 
      delay(120000);  
      lcd2.setCursor(12,3);
      lcd2.print("Feed OFF");
      digitalWrite(Return_Pump, LOW);  
      delay(1000); 
      digitalWrite(Skimmer, LOW); 
      lcd.setBacklight(LOW);  //digitalWrite(LcdLight, LOW);
      lcd2.setBacklight(LOW);  //digitalWrite(Lcd2Light, LOW); 
  }

