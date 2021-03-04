/* Recks Aquarium Computer by Carl (Squiggy) Recktenwald Jr.
Reck's Design and Fabrication
Arduino Mega -with 3 Port Expander(MCP23008)for LCD(s) and For Float Switches
Special Thanks to LadaAda (Adafruit.com)for the TMP36 Code and Port Expander Code
Uploaded On =10-11-14 at 1110 
*/ 
/////////////
//Libraries
////////////
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>
#include <IRremote.h>
#include "Timer.h"
#include "NewTone.h"
#include <SPI.h>
#include <Ethernet.h>
#include "Remote_Codes.h"

/////////////////////////////////////
//Frequent Changed Variable Set up
////////////////////////////////////
//Rev Number
  const int Rev = 12;    //So you only have to change the revision number 1 time and in 1 place 

//Millis
  long previousMillis = 0;        // store last time temp was updated
  long interval = 20000;        //25000   //20-25 Second interval (milliseconds)   

//Water Change Info // Change every 14 days
  const int Water_Change_Month =10;   // Digit of the Month
  const int Water_Change_Day =8;    // Day of the Month 
  
//Turn off skimmer for dosing-2 weeks after water change
  int dose_month = 10;        //Int to turn off skimmer for dosing
  int dose_day = 22;

//Phosban Change Info // Change every 180 days (6 Months)
  const int GFO_Month = 10;     
  const int GFO_Day = 7;      
  
//Carbon Change Info // Change every 30 to 45 days
  const int Carbon_Month = 9; 
  const int Carbon_Day = 27;  
  //1/2 cup
  
//PH 
  #define PHADDRESS 0x4D
  int RoomTempI2CAddress = B1001011;
  float volt7 = 0.7432;     //0.7700;   //0.8021;    //change this
  float volt10= 0.4804;     //0.5127;  //0.5377;    //change this
  float calibrationTempC = 26;     //24c=75f, 25c=77f 26c=80f
  //Probe calibrated on 09-28-14 with 7.0 and 10.0
  float phMax = 0.99;
  float phMin = 9.99; 
  float ph_value; 
  double ph_volts; 
  double ph_temp_comp; 
  double ph_temp_room;  
  double ph_temp_adj;   
       
//Aref 
  #define aref_voltage 3.4   // we tie 3.3V to ARef and measure it with a multimeter for acurate Temp readings

/////////////
//Pins Set Up
/////////////  
//A0-TMP36 Tank and Pin Variables
  const int TanktempPin = A0;        //TMP 36 In 
  int tempReading; 
  float tempMin = 85;           // minimum starting temp value for display
  float tempMax = 65;           // maximum starting temp value for display 

//A1-TMP 36 Sump Temp 
  const int SumptempPin = A1;      //TMP 36 In 
  int sumpReading;
  float tempMin2 = 85;           // minimum starting temp value for display
  float tempMax2 = 65;           // maximum starting temp value for display 

//A2-

//A3-Canopy Temp 
  const int CanopytempPin = A3; 
  int canopyReading; 
  float tempMin3 = 99;           // minimum starting temp value for display
  float tempMax3 = 75;           // maximum starting temp value for display 
  
//A4-
  
//A5-Canopy Fan 
  const int Back_Fan = A5; 
  
//A6-  
  
//A7-Work Light In Sump
  const int Work_Light = A7; 
  boolean Worklight = false;  

//Digital Pins/////////////////////  
//D0-RX

//D1-TX

//D2-GFO Pump    //Normal Closed Relay - High Turns it OFF 
  const int GFO = 2; 
  boolean GFO_Pump = false; 
  
//D3-PH Pump
  const int PH_Pump = 3;
  boolean ph_temp = false; 
  
//D4-CS for SD Card
  const int SS_SD_CARD = 4; 

//D5-Cooling Fan
  const int Cooling_Fan = 5;
  boolean Cool = false; 
  
//D6-Skimmer  //Normal Closed Relay - High Turns it OFF
  const int Skimmer = 6;     
  boolean Skim = false; 
  int dose_H1 = 22;               //Hour to dose (Clock time)  
  int dose_M1 = 00;               // Minuntes to dose (Clock Time) 
  int dose_S1 = 00;               // Seconds to Dose (Clock Time)    
  int dose_off_H1 = 8;            //Hour to turn on after dose (Clock time)  
  int dose_off_M1 = 00;           // Minuntes to turn on after dose (Clock Time) 
  int dose_off_S1 = 00;           // Seconds to turn on after Dose (Clock Time)   

//D7-Return Pump  //Normal Closed Relay - High Turns it OFF
  const int Return_Pump = 7;       
  boolean Pump = false;
  
//D8-IR Sensor 
  const int RECV_PIN = 8;
  IRrecv irrecv(RECV_PIN);
  decode_results results;

//D9-ATO
  const int ATO = 9;    
  int ato_ontime = 20;           //Number of seconds to run ato
  int ato_hour =0;             //default so ato runs once an hour
  int pump_count =0;           //Number of times pump has run
  int ato_last =0;             //Variable for pump running once an hour
  Timer t;
  
//D10-SS for Etherent Shield
    const int SS_ETHERNET = 10;
      
//D11-Sump Light SSR
  const int SumpLight = 11;         //Crydom SSR of
  const int Sump_on = 1930;        //Sump Light On Time
  const int Sump_off = 730;       // Sump Light Off Time 
  boolean Sump = false;     // Var to Have manual control of light

//D12-Heater
  const int Heater = 12;       //Crydom SSR
  boolean Heat = false; 
  
//D13-Buzzzer
  //#define TONE_PIN =13;           //Piezo Buzzer

//D24-Led Tank Light-(N/O Relay)
  const int LED_TankLight = 24;
  const int LED_on = 1230;        //LED Light On Time
  const int  LED_off = 1530;       // LED Light Off Time 
  boolean LED = false;  

//D34-TankLight-(N/C Relay)//Normal Closed Relay - High Turns it OFF
  const int TankLight = 34;
  boolean Tank = false; 

//D26-Wave Pumps-(N/C Relay)//Normal Closed Relay - High Turns it OFF
  const int Wave_Pumps = 26; 
  boolean Wave = false; 
  
//D32-Dawn and Dusk Lights (N/O Relay)
  const int Dawn_Dusk = 32; 
  const int Dawn_on = 715;         //Dawn_Dusk Light On Time
  const int  Dawn_off = 810;       // Dawn_Dusk Light Off Time 
  const int Dusk_on = 1850;         //Dawn_Dusk Light On Time
  const int  Dusk_off = 1915;       // Dawn_Dusk Light Off Time 
  boolean Dawn_Dusk_Light = false; 
 
//SPI BUS-50, 51 and 52 - 53 Also need to be an output
//50
//51
//52
//53

//Xively
  #define APIKEY         "eYH2miNz2QyYaDYNXgtGinLrTN7VOg3xU2fcQxyQDD7eGPxY" // replace your xively api key here
  #define FEEDID         1899079766 // replace your feed ID
  #define USERAGENT      "90 Gallon Aquarium" // user agent is the project name
  byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  IPAddress ip(10,0,0,27);
  EthernetClient client;
  IPAddress server(216,52,233,121);      // numeric IP for api.xively.com 
     
//Real Time Clock 
  RTC_DS1307 RTC;
  int mil_time = 0;           //Military Time 
  int time;                   //for ATO Hour

//MCP InPuts-//Float Switches 
  Adafruit_MCP23008 mcp;  //Address 6 
//0
  const int Fresh_Low =0;            //USB Balck and White Black (Ground) Wire- ATO Input from float switch from fresh water tank
  int Fresh;
//1
  const int Too_High =1;           //-USB Black-Green Wire-Ato Input #2 from float Switch -2
  int Salt_High; 
//2
  const int Sump_Low =2;            //-USB Black-White Wire- ATO Input from float switch -1 
  int Salt_Low;  
//3  
  const int Cabinet_Pin = 3;       //USB White-Green Wire
  int Cabinet; 
//4   
  const int Low_Pump = 4;         //-USB-White-White Wire
  int Water_Low;
  
//LCD and LCD 2
  LiquidCrystal lcd (1);
  LiquidCrystal lcd2(2); 
  boolean lcdlight = true; 
  const int Lcd_off = 2130; 
  
/////////////////////////
//SETUP
//////////////////////// 
      void setup(void) {
// TMP 36- If you want to set the aref to something other than 5v
  analogReference(EXTERNAL);
  
//A0
  pinMode(TanktempPin, INPUT);   //Tank Temp Don't Have to Declare as Input

//A1
  pinMode(SumptempPin, INPUT);  //Sump Temp Don't Have to Declare as Input

//A2
  
//A3 
  pinMode(CanopytempPin, INPUT); 
  
//A4-
  
//A5-Canopy Fan on Back Mosfet Driven
  pinMode(Back_Fan, OUTPUT); 
  digitalWrite(Back_Fan, LOW);
 
//A6  
  
//A7-Work Light Mosfet Driven
  pinMode(Work_Light, OUTPUT);  
  digitalWrite(Work_Light, LOW); 

//Digital Pins ////////////////   
//D0-RX

//D1-TX 

//D2-GFO-N/C Relay 
  pinMode(GFO, OUTPUT); 
  
//D3-PH Pump
  pinMode(PH_Pump, OUTPUT); 

//D4-CS for SD Card
  pinMode(SS_SD_CARD, OUTPUT);
  digitalWrite(SS_SD_CARD, HIGH);    // SD Card not active  

//D5-Cooling Fan
  pinMode(Cooling_Fan, OUTPUT); 

//D6-Skimmer 
  pinMode(Skimmer, OUTPUT);   
  
//D7-Return Pump 
  pinMode(Return_Pump, OUTPUT);

//D8-IR Remote
  irrecv.enableIRIn(); // Start the receiver

//D9-ATO
  pinMode(ATO, OUTPUT); 

//D10-SS for Etherent Shield
  pinMode(SS_ETHERNET, OUTPUT);
  digitalWrite(SS_ETHERNET, HIGH);    // Ethernet not active-turns it OFF

//D11-Sump Light 
  pinMode(SumpLight, OUTPUT); 
  
//D12-Heater 
  pinMode(Heater, OUTPUT); 

//D13-Buzzer
//Buzzer Output (Don't Need to Declare, it is Here for Reference Only)

//D-Led Tank Light-(N/O Relay)
  pinMode(LED_TankLight, OUTPUT); 
 
//D-TankLight-(N/C Relay)
  pinMode(TankLight, OUTPUT);
  
//D26-Wave Pumps-(N/C Relay)
  pinMode(Wave_Pumps, OUTPUT); 
  
//D32-Canopy Dawn / Dusk Lightsd (N/O Relay) 
  pinMode(Dawn_Dusk, OUTPUT); 
  
//SPI BUS-50, 51 and 52 and 53 also need to be an output
//50
//51
//52
//53  
  pinMode(53, OUTPUT); //On the Mega, the hardware SS pin, 53, is not used 
                       //to select either the W5100 or the SD card, but it 
                      //must be kept as an output or the SPI interface won't work. 

//Ethernet-Static IP  
  Ethernet.begin(mac, ip);

//I2C-
    Wire.begin();
    RTC.begin();
    
//MCP InPuts
  mcp.begin(6);
  mcp.pinMode(Fresh_Low, INPUT); //Fresh
  mcp.pinMode(Too_High, INPUT); //Salt High
  mcp.pinMode(Sump_Low, INPUT); //Salt Low
  mcp.pinMode(Cabinet_Pin, INPUT); //Cabinet Pin
  mcp.pinMode(Low_Pump, INPUT); //Low Water - Pump OFF

//LCD OutPut
  lcd.begin(20,4);            //LCD Output
  
//LCD 2 Output
  lcd2.begin(20,4);           //LCD 2 Output
  
//Boot Message
  Boot_Message();   //Go to the boot up message tab
}
/////////////////////////
//LOOP
////////////////////////
      void loop(void) {
//Timer Update 
  t.update(); 
               
//Get RTC Data
  DateTime now = RTC.now();    //Get the date and time from RTC
  mil_time = now.hour() *100 + now.minute();    // Mlitary time setup 
  time = now.hour();   //variable for getting just the hour

//Print to LCD    
  lcd.setCursor (11,1);
  lcd.print ("Time:");
  if(mil_time < 1000){  //Print a 0 if less than 1000 (10 am)
      lcd.print("0");}
  lcd.print (mil_time);
  
//LED TankLight (Full Day Sun) On and Off Time - With Manual Override
  if ((LED_on <= mil_time) && (LED_off > mil_time)){
        digitalWrite(LED_TankLight, HIGH);} 
  
  else if (LED == true){
         digitalWrite(LED_TankLight, HIGH);}   
     
  else {digitalWrite(LED_TankLight, LOW); }    

//Dawn and Dusk Lights With Manual Override
  if ((Dawn_on == mil_time) || (Dusk_on == mil_time)){
        Dawn_Dusk_Light  = true;} 
  
  else if ((Dawn_off == mil_time) || (Dusk_off == mil_time)){
        Dawn_Dusk_Light  = false;}
            
  if (Dawn_Dusk_Light  == true){ 
        digitalWrite(Dawn_Dusk, HIGH);}
     
  else {digitalWrite(Dawn_Dusk, LOW); }

//Sump Light On and Off Time - With Manual Override
  if ((Sump_on <= mil_time) || (Sump_off > mil_time) || (Sump==true)){
      digitalWrite(SumpLight, HIGH);}  
     
  else {digitalWrite(SumpLight, LOW); }
     
//Read Sump Light Status
    if (digitalRead(SumpLight) == HIGH){
        lcd2.setCursor(11,0);
        lcd2.print("Sump ON "); }
                  
    else if (digitalRead(SumpLight) == LOW) {
        lcd2.setCursor(11,0);     
        lcd2.print("Sump OFF");}
  
//Low Water Float
    Water_Low = mcp.digitalRead(Low_Pump);
    
//If All is Well then Print ALL Good 
    if (Water_Low == LOW){
     lcd.setCursor (12,3); 
     lcd.print("All Good");}
    
    else if(Return_Pump == HIGH){
     lcd.setCursor (12,3); 
     lcd.print("CHECK TK");}      

//Turn off return pump if water is low     
    else if (Water_Low == HIGH){
        lcd.setCursor (12,3); 
        lcd.print("H20 Pump");
        digitalWrite(Return_Pump, HIGH);  
        digitalWrite(Skimmer, HIGH); 
        NewTone(13, 75, 5000);               //pin #, Frequency, Duration -sound the buzzer
        lcd.setBacklight(HIGH);    //Turn on LCD BackLight to Tell Me 
        lcd2.setBacklight(HIGH); }    
    
//Digital Read Fresh RO Float for ATO 
   Fresh = mcp.digitalRead(Fresh_Low); 

//ATO Float Status  
     if (Fresh ==LOW){ 
     lcd2.setCursor(0,2);
     lcd2.print ("Fresh OK "); }
     
     else if (Fresh ==HIGH){ 
     lcd2.setCursor(0,2);
     lcd2.print ("Fresh LOW"); }
     
//Digital Read Salt Water Floats for ATO     
   Salt_High = mcp.digitalRead(Too_High);
   Salt_Low = mcp.digitalRead(Sump_Low); 
   
    if ((Salt_Low==LOW) && (Salt_High == LOW)){
     lcd2.setCursor(10,2);
     lcd2.print("Sump Level");  }
    
    else if (Salt_Low==HIGH){
     lcd2.setCursor(10,2);
     lcd2.print("Sump LOW  ");  } 
     
    else if ((Salt_High==HIGH) && (Salt_Low == LOW)){
     lcd2.setCursor(10,2);
     lcd2.print("Sump HIGH ");  }

//Skimmer
    if (Salt_High == HIGH){   //Water is high in the sump so turn off the skimmer
        digitalWrite(Skimmer, HIGH);   
         NewTone(13, 75, 1000); }  

    else if (digitalRead(Return_Pump) == HIGH){   
             digitalWrite(Skimmer, HIGH); }      //Water is high in the sump so turn off the skimmer
             //NewTone(13, 75, 1000); }
           
//Skimmer off and on for Dosing period
    if((dose_H1==now.hour())&&(dose_M1==now.minute())&&(dose_S1)==now.second()&&(dose_month == now.month())&&(dose_day == now.day())){  
           digitalWrite(Skimmer, HIGH);
           Skim = true;   }
           
    else if (( Skim==true)&&(dose_off_H1==now.hour())&&(dose_off_M1==now.minute())&&(dose_off_S1)==now.second()){
           digitalWrite(Skimmer, LOW); 
           Skim = false;  }      
   
//Cabinet Door Pin 
  Cabinet = mcp.digitalRead(Cabinet_Pin); 

//Work Light
  if ((Worklight == true) || (Cabinet == HIGH)){
    digitalWrite(Work_Light, HIGH); }
    
  else {digitalWrite(Work_Light, LOW);  } 
  
//LCD Light

  if (mil_time == Lcd_off){
      lcdlight = false;   } 
         
  else if (lcdlight == true){
       lcd.setBacklight(HIGH);  
       lcd2.setBacklight(HIGH);}
    
  else if (lcdlight == false){
       lcd.setBacklight(LOW);  
       lcd2.setBacklight(LOW);  }    
                      
//ATO
   if((Salt_Low == HIGH) && (Salt_High == LOW) && (Fresh == LOW) && (time != ato_hour)){   //If water low
         delay(500);                //Check to ensure water level is still low - in case of splashing in sump 
            if((Salt_Low == HIGH) && (Cabinet ==LOW)){   //Check level is still Low and that Sump Door is closed
                 ato(); }   }        //Go to ATO Tab 
                 
//ATO Status
    if (digitalRead(ATO) == HIGH){
     lcd2.setCursor(1,1);
     lcd2.print("ATO ON"); }
   
    else if (digitalRead(ATO) == LOW){
    lcd2.setCursor(1,1);
    lcd2.print(ato_last); 
    lcd2.print("  "); }
    //lcd2.print(" #");
    //lcd2.print(pump_count); } 

///////////////////////
//IR - Buttons Do this
///////////////////////
  if(irrecv.decode(&results)){ //this checks to see if a code has been received
      if(results.value == Power){
        lcdlight = !lcdlight; 
        Worklight = !Worklight; 
        lcd2.setCursor(0,0);
        lcd2.print("  LIGHT   ");    
   } 
    else if(results.value == one){ 
        lcdlight = !lcdlight; 
        lcd2.setCursor(0,0);
        lcd2.print("   LCD    ");     //Do This if one
   }
    else if(results.value == two){ 
        lcd2.setCursor(0,0);
        lcd2.print("H2O Change");    //Do this if two
        water_change();    
 }
    else if(results.value == three){ 
        Sump = !Sump;   
        lcd2.setCursor(0,0);
        lcd2.print("   Sump   ");    //Do This if three
  } 
    else if(results.value == four){ 
        Tank = !Tank;  
        lcd2.setCursor(0,0);
        lcd2.print("   Tank   ");    //Do This if four
  }
    else if(results.value == five){ 
        LED = !LED; 
        lcd2.setCursor(0,0);
        lcd2.print("   LED    ");    //Do This if five
  }
    else if(results.value == six){ 
        Worklight = !Worklight; 
        lcd2.setCursor(0,0);
        lcd2.print("Work Light");       //Do This if six
  }
    else if(results.value == seven){ 
        digitalWrite(Skimmer, HIGH);
        lcd2.setCursor(0,0);
        lcd2.print(" Skim OFF ");        //Do This if seven
  } 
    else if(results.value == eight){ 
        digitalWrite(Skimmer, LOW); 
        lcd2.setCursor(0,0);
        lcd2.print(" Skim ON  ");    //Do This if eight       
  } 
    else if(results.value == nine){ 
        lcd2.setCursor(0,0);
        lcd2.print("   ATO    "); 
        ato(); }    // Do this if Nine
        
    else if(results.value == zero){ 
        lcd2.setCursor(0,0);
        lcd2.print("GFO Pump");
        GFO_Pump =! GFO_Pump;            //Do This if zero
  }
    else if(results.value == onehundred){ 
        digitalWrite(Return_Pump, HIGH); 
        lcd2.setCursor(0,0);
        lcd2.print(" Pump OFF ");       //Do This if 100+
  } 
    else if(results.value == Menu){ 
        digitalWrite(Return_Pump, LOW);     //Resets The Pump(turns back on) 
        delay(1000); 
        digitalWrite(Skimmer, LOW); 
        lcd2.setCursor(0,0);
        lcd2.print("Reset Pumps");    //go to the loop of LCD 
  }    
    else if(results.value == Channel_up){ 
        lcd2.setCursor(0,0);
        lcd2.print("  Dates   "); 
        dates();  
  }  
     else if(results.value == Channel_down){ 
        lcd2.setCursor(0,0);
        lcd2.print(" FAN Test ");
        digitalWrite(Cooling_Fan, HIGH);       //Do This 
        delay(2500); 
        digitalWrite(Cooling_Fan, LOW); 
  }       
    else if(results.value == Volume_up){ 
        dose_month++;
        lcd2.setCursor(0,0);
        lcd2.print(dose_month);      //Do This if volume down 
        delay(30);  
  }
    else if(results.value == Volume_down){
        lcd2.setCursor(2,0);
        lcd2.print("/"); 
        dose_day++;
        lcd2.setCursor(3,0);
        lcd2.print(dose_day);  
        delay(30);   
  }            
    else if(results.value == Display){ 
        lcd2.setCursor(0,0);
        lcd2.print(" Display  ");      //Do This if display
        LCD();                  
  } 
    else if(results.value == Picture){ 
        lcd2.setCursor(0,0);
        lcd2.print("Dawn_Dusk");      //Do This if 
        Dawn_Dusk_Light =! Dawn_Dusk_Light; 
   }     
    else if(results.value == Mute){ 
        lcd2.setCursor(0,0);
        lcd2.print(" Feeding "); 
        feed();  
  }      
 irrecv.resume(); //receive the next value
}   
   
//Date Out of Bounse Checking
//Roles over the date if going past 12 or 31
    if (dose_month >= 13){
        dose_month = 00; } 
     
    if (dose_day >= 32){
        dose_day = 00;   }    

//PH- Mostly a copy from the tindle website for the i2c ph board
  SetRoomTemperataureResolutionBits(12);//12 bits room temp resolution in celcius
  int sampleSize = 500;
  double avgMeasuredPH = 0;
  double avgRoomTempC = 0;
  double avgPHVolts = 0;
  double avgRoomTemperatureCompensatedMeasuredPH = 0;
  double tempAdjusted10;
  int x;
  for(x=0;x< sampleSize;x++)
    {
  double phVolt = getPHVolts();    
  tempAdjusted10 = adjustPHBasedOnTemp(10,calibrationTempC);
  double voltsPerPH = abs((volt10-volt7) / (tempAdjusted10-7));
  double realPHVolt = (volt7 - phVolt);
  double phUnits = realPHVolt / voltsPerPH;
  double measuredPH = 7 + phUnits;
  double roomTempC =  getRoomTemperatureC(); 
  double roomTempCompensatedMeasuredPH = adjustPHBasedOnTemp(measuredPH,roomTempC);
  avgMeasuredPH+=measuredPH;
  avgRoomTemperatureCompensatedMeasuredPH+=roomTempCompensatedMeasuredPH;
  avgRoomTempC+=roomTempC;
  avgPHVolts += phVolt;  
    }
  avgMeasuredPH/=sampleSize;
  avgRoomTemperatureCompensatedMeasuredPH/=sampleSize;
  avgRoomTempC/=sampleSize;
  avgPHVolts/=sampleSize;
 
//Print to LCD
    lcd.setCursor(12,0); 
    lcd.print("PH:");  
    lcd.print(avgMeasuredPH,2);  //Prints to 2 decimal places(avgMeasuredPH,2) or,3 or,4
       
//For Calibration Only Prints Volt to LCD
    lcd2.setCursor(0,0);
    lcd2.print("V=");  
    lcd2.print(avgPHVolts,4); //Prints to 4 decimal places(

//For xively 
    ph_value = avgMeasuredPH,3;
    ph_volts = avgPHVolts,4; 
    ph_temp_comp = avgRoomTemperatureCompensatedMeasuredPH,4; 
    ph_temp_room = avgRoomTempC,3; 

//Max and Min PH on LCD
// record the maximum PH
  if (avgMeasuredPH > phMax) phMax = avgMeasuredPH;{
  //lcd.setCursor (0,2);
  //lcd.print("PH=");
  //lcd.print("Max:");
  //lcd.print(phMax);
  }
     
// record the minimum PH 
  if (avgMeasuredPH < phMin) phMin = avgMeasuredPH;{
  //lcd.setCursor(12,2);
  //lcd.print("Min:");
  //lcd.print(phMin); 
  }  
  
//Ph Pump
  if ((avgMeasuredPH <= 8.10) && (ph_temp ==false)) {      //if ph is below turn on pump
    digitalWrite(PH_Pump, HIGH);
    lcd.setCursor(0,3); 
    lcd.print("PH Pump ON "); 
  }

  else if ((avgMeasuredPH >= 8.20) || (ph_temp == true)){      //if above turn off
    digitalWrite(PH_Pump, LOW);
    lcd.setCursor(0,3); 
    lcd.print("PH Pump OFF");}
    
//Wave Pumps
    //No code Yet 
    
//GFO Pump 
  if (GFO_Pump == true){
    digitalWrite(GFO, HIGH);} 
  
  else { digitalWrite(GFO, LOW); }   
  
///////////////////////////////////////////////////
//Millis is below
///////////////////////////////////////////////////

  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
// save the last time
   previousMillis = currentMillis;      
      
//TMP36 Tank Temp    
   tempReading = analogRead(TanktempPin); 
   float voltage = tempReading * aref_voltage;
   voltage /= 1024.0; 
   float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree w/ 500 mV offset to degrees ((volatge - 500mV) times 100)
   float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;  // now convert to Fahrenheight
   lcd.setCursor(0,0); 
   lcd.print ("Tank=");
   lcd.print(temperatureF); 
   lcd.print((char)223);      //Print the Degree Symbol
   
//TMP36 Sump Temp
  sumpReading = analogRead(SumptempPin); 
  lcd.setCursor(0,1);
  float voltage2 = sumpReading * aref_voltage;
  voltage2 /= 1024.0; 
  float temperatureC2 = (voltage2 - 0.5) * 100 ; 
  float temperatureF2 = (temperatureC2 * 9.0 / 5.0) + 32.0;
  lcd.print ("Sump=");
  lcd.print(temperatureF2); 
  lcd.print((char)223);    
  
//Heater on or off
      if ((temperatureF < 76.0)  && (temperatureF2 < 78.0) && (Heat == true)){  //Turns Heater on if below 77.and sump below 80
       digitalWrite(Heater, HIGH); 
       lcd2.setCursor (10,1);       
       lcd2.print("Heater ON ");  }
        
     else if ((temperatureF > 77.0) || (temperatureF2 > 80.0) && (Heat == true)){            //turn heater off
      digitalWrite(Heater, LOW);
      Heat = false; 
      lcd2.setCursor (10,1);
      lcd2.print("Heater OFF"); }
      
      else {
      digitalWrite(Heater, LOW);
      Heat = false; 
      lcd2.setCursor (10,1);
      lcd2.print("Heater OFF"); }
            
//Set the Heater Flag to elimate false Heater on Events
      if ((temperatureF < 76.0)  && (temperatureF2 < 78.0)){ 
        Heat = true;
        lcd2.setCursor(7,1);
        lcd2.print(" T");  }
        
      else { Heat = false;
             lcd2.setCursor(7,1);
             lcd2.print(" F"); }           
          
//Max and Min Temp on LCD
// record the maximum temp 
  if (temperatureF > tempMax) tempMax = temperatureF;{
  //lcd.setCursor (0,3);
  //lcd.print("Tank=");
  //lcd.print("Max");
  //lcd.print(tempMax);
  //lcd.print((char)223);
  }
     
// record the minimum temp 
  if (temperatureF < tempMin) tempMin = temperatureF;{
  //lcd.setCursor (6,3);
  //lcd.print("Min");
  //lcd.print(tempMin);
  //lcd.print((char)223);
  }     
  
//Sump Min and Max Temps
  if (temperatureF2 > tempMax2) tempMax2 = temperatureF2;{
  }
  
  if (temperatureF2 < tempMin2) tempMin2 = temperatureF2;{
  }  

//TMP36 Canopy Temp    
   canopyReading = analogRead(CanopytempPin); 
   float voltage3 = canopyReading * aref_voltage;
   voltage3 /= 1024.0; 
   float temperatureC3 = (voltage3 - 0.5) * 100 ;  //converting from 10 mv per degree w/ 500 mV offset to degrees ((volatge - 500mV) times 100)
   float temperatureF3 = (temperatureC3 * 9.0 / 5.0) + 32.0;  // now convert to Fahrenheight
   lcd.setCursor(0,2); 
   lcd.print ("Canopy=");
   lcd.print(temperatureF3); 
   lcd.print((char)223);      //Print the Degree Symbol

//Canopy Min and Max Temps
  if (temperatureF3 > tempMax3) tempMax3 = temperatureF3;{
  }
  
  if (temperatureF3 < tempMin3) tempMin3 = temperatureF3;{
  }   
  
//Tank Cooling Fan 
  if ((temperatureF > 80) && (temperatureF2 > 79)){
      digitalWrite(Cooling_Fan, HIGH);
      lcd2.setCursor (0,3);
      lcd2.print("FAN ON "); }    //Turn on 12VDC Fan 
    
  if ((temperatureF < 79) || (temperatureF2 < 78)){
       digitalWrite(Cooling_Fan, LOW);
       lcd2.setCursor (0,3);
       lcd2.print("FAN OFF"); }      //Turn Off 12VDC Fan

//Tank Light Off when water way too hot
  if ((Cool ==true) && (temperatureF > 84) && (temperatureF2 > 84)){
      digitalWrite(TankLight, HIGH);     //Turn Off Tanklight
      digitalWrite(Cooling_Fan, HIGH);
      digitalWrite(Back_Fan, HIGH);
      ph_temp = true;                     //Turn Off PH Pump
      lcd2.setCursor(8,3);
      lcd2.print(" HOT  "); }    
    
  else if (temperatureF < 82){
      digitalWrite(TankLight, LOW);     //Turn tankLight Back On 
      ph_temp = false;        //Turn on PH Pump
      Cool = false; 
      lcd2.setCursor(8,3);
      lcd2.print("TL/OK "); }      
      
//Cooling Flag for turn off tank light
  if ((temperatureF > 83) && (temperatureF2 > 83)){ 
       Cool = true;
       digitalWrite(Cooling_Fan, HIGH);
       digitalWrite(Back_Fan, HIGH);   }
        
  else { Cool = false; }        
      
//Canopy Cooling Fan 
  if ((temperatureF3 > 99) || (temperatureF > 82)){
    digitalWrite(Back_Fan, HIGH);
    lcd.setCursor(13,2); 
    lcd.print("FAN ON "); }
    
  else if ((temperatureF3 < 90) && (temperatureF < 81)){
    digitalWrite(Back_Fan, LOW); 
    lcd.setCursor(13,2); 
    lcd.print("FAN OFF");}   

//Xively Channels   
  digitalWrite(SS_ETHERNET, LOW);   //Turn on Etherent Shield
  
  String dataString = "PH,";
  dataString += ph_value;
  
  dataString += "\nPH_Volts,";
  dataString += ph_volts;
  
  dataString += "\nPH_TempComp,";
  dataString += ph_temp_comp;
  
  dataString += "\nPH_RoomTemp,";
  dataString += ph_temp_room;  
  
  dataString += "\nTank_Temperature,";
  dataString += temperatureF;

  dataString += "\nTank_Temp_Max,";
  dataString += tempMax;

  dataString += "\nTank_Temp_Min,";
  dataString += tempMin;
  
  dataString += "\nSump_Temperature,";
  dataString += temperatureF2;
  
  dataString += "\nSump_Temp_Max,";
  dataString += tempMax2;

  dataString += "\nSump_Temp_Min,";
  dataString += tempMin2;
  
  dataString += "\nCanopy_Temperature,";
  dataString += temperatureF3;
  
  dataString += "\nCanopy_Temp_Max,";
  dataString += tempMax3; 

  dataString += "\nCanopy_Temp_Min,";
  dataString += tempMin3; 
  
  int DawnDusk_Status = digitalRead(Dawn_Dusk); 
  dataString += "\nDawn_Dusk,";
  dataString += DawnDusk_Status;
  
  int WavePumps_Status = digitalRead(Wave_Pumps); 
  dataString += "\nWave_Maker,";
  dataString += WavePumps_Status;
  
  int LEDLight_Status = digitalRead(LED_TankLight); 
  dataString += "\nLED_Light,";
  dataString += LEDLight_Status;
  
  int PH_Pump_Status = digitalRead(PH_Pump); 
  dataString += "\nPH_Pump,";
  dataString += PH_Pump_Status;
    
  int CanopyFan_Status = digitalRead(Back_Fan); 
  dataString += "\nCanopy_Fan,";
  dataString += CanopyFan_Status;

  int TankLight_Status = digitalRead(TankLight);
  dataString += "\nTank_Light,";
  dataString += TankLight_Status;
  
  int Cooling_Status = digitalRead(Cooling_Fan);
  dataString += "\nCooling_Fan,";
  dataString += Cooling_Status;
  
  int Skimmer_Status = digitalRead(Skimmer); 
  dataString += "\nSkimmer,";
  dataString += Skimmer_Status;
  
  dataString += "\nSkimmer_Flag,";
  dataString += Skim;
  
  dataString += "\nCooling_Flag,";
  dataString += Cool;
  
  int Return_Pump_Status = digitalRead(Return_Pump); 
  dataString += "\nReturn_Pump,";
  dataString += Return_Pump_Status;  

  int Work_Light_Status = digitalRead(Work_Light); 
  dataString += "\nWork_Light,";
  dataString += Work_Light_Status;
  
  int SumpLight_Status = digitalRead(SumpLight); 
  dataString += "\nSump_Light,";
  dataString += SumpLight_Status;
  
  int Heater_Status = digitalRead(Heater);
  dataString += "\nHeater,";
  dataString += Heater_Status;
  
  dataString += "\nHeater_Flag,";
  dataString += Heat;
  
  dataString += "\nFresh_Water_Float,";
  dataString += Fresh;
  
  dataString += "\nSalt_High_Float,";
  dataString += Salt_High;
  
  dataString += "\nSalt_Low_Float,";
  dataString += Salt_Low;
  
  dataString += "\nWater_Pump_Float,";
  dataString += Water_Low;
  
  dataString += "\nCabinet_Door,";
  dataString += Cabinet;

  dataString += "\nATO_Count,";
  dataString += pump_count;
  
  dataString += "\nRev,";
  dataString += Rev;

  dataString += "\nWater_Change_Month,";
  dataString += Water_Change_Month;
  
  dataString += "\nDosing_Month,";
  dataString += dose_month;
  
  dataString += "\nDosing_Day,";
  dataString += dose_day;
  
  dataString += "\nWater_Day,";
  dataString += Water_Change_Day;
  
  dataString += "\nATO_Ran,";
  dataString += ato_last;

  dataString += "\nTime,";
  dataString += mil_time;

//Send the Data to Xively
    sendData(dataString);             //take the program to SendData tab

}    //Millis End
}    //Loop End
      
 
