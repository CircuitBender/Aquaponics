

void dates(){
    lcd.clear(); 
    lcd2.clear();
    delay (250); 
    
    lcd.setCursor(0,0); 
    lcd.print("Cleaned On");
    lcd.setCursor(0,1);
    lcd.print(Water_Change_Month); 
    lcd.print("-");
    lcd.print(Water_Change_Day); 
        
    lcd.setCursor(0,2); 
    lcd.print("Carbon");
    lcd.setCursor(0,3);
    lcd.print(Carbon_Month);
    lcd.print("-");
    lcd.print(Carbon_Day); 
    
    lcd2.setCursor(0,0); 
    lcd2.print("GFO");
    lcd2.setCursor(0,1);
    lcd2.print(GFO_Month );
    lcd2.print("-");
    lcd2.print(GFO_Day); 
    
    lcd2.setCursor(0,2); 
    lcd2.print("Skimmer Dosing Off");
    lcd2.setCursor(0,3); 
    lcd2.print(dose_month); 
    lcd2.print("/");   
    lcd2.print(dose_day); 
    
    delay(7500); 
    lcd.clear(); 
    lcd2.clear(); 
}



