

void LCD(){    //Tells me what each button does 
//LCD 1 
    lcd.clear(); 
    lcd2.clear();
    delay (250);  
    lcd.setCursor(0,0); 
    lcd.print("1=LCD, "); 
    lcd.print("2=H2O_Change");   
    lcd.setCursor(0,1); 
    lcd.print("3=Sump, "); 
    lcd.print("4=Tank"); 
    lcd.setCursor(0,2); 
    lcd.print("5=LED, "); 
    lcd.print("6=Work");   
    lcd.setCursor(0,3); 
    lcd.print("7=Skim OFF, "); 
    lcd.print("8=Skim ON"); 
//LCD 2     
    lcd2.setCursor(0,0); 
    lcd2.print("9=ATO, "); 
    lcd2.print("Menu=Rest Pumps,");   
    lcd2.setCursor(0,1);
    lcd2.print("0=GFO, ");
    lcd2.print("100=Pump OFF");  
    lcd2.setCursor(0,2); 
    lcd2.print("C-=FAN, ");     
    lcd2.print("C+=Dates");   
    lcd2.setCursor(0,3);
    lcd2.print("V-=Month, ");
    lcd2.print("V+=Day");
//Delay    
    delay(5000); 
    lcd.clear(); 
    lcd2.clear(); 
  }  

