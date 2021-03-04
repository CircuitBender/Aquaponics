
  void Boot_Message (){
// Boot Up Mesage
  lcd.setBacklight(HIGH);   //Turn on LCD BackLight 
  lcd2.setBacklight(HIGH); 
  lcd.setCursor(0,0);              //Prints Message 1 Time during the start up 
  lcd.print("ACM");
  lcd.setCursor(0,1);
  lcd.print("By: Carl (SQUIGGY)");
  lcd.setCursor(0,2);
  lcd.print("Recktenwald Jr.");
  lcd.setCursor(0,3);
  lcd.print ("90 Gallon:");
  lcd.setCursor(12,3);
  lcd.print("Booting");
  
//LCD 2 Boot Up
  lcd2.setCursor(0,0);            //Prints Message 1 Time during the start up 
  lcd2.print("Reck's Aquarium:");
  lcd2.setCursor(0,1);
  lcd2.print("Computer- ACM#");
  lcd2.print(Rev); 
  delay(2000);                    //Display the boot up message for 2 seconds
  lcd.clear();                    //clears the lcds for the incoming Aquarium Data
  lcd2.clear();
  NewTone (13, 100, 1000);   //pin #, Frequency, Duration -sound the buzzer 
  //lcd.setBacklight(LOW);   //Turn off LCD BackLight 
  //lcd2.setBacklight(LOW);  
  
//RAC Print
  lcd2.setCursor(14,3);  //lcd.setCursor(12,0);
  lcd2.print ("ACM:");
  lcd2.print (Rev);
    
  } 
