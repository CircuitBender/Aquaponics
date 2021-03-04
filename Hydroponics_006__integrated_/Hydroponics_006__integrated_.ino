// ************ Water Sensor************ 

int WaterSensor;
int Plant_WaterTank_Full;
int Plant_WaterTank_Low;
int Plant_Water_Status;
//************ Temp Probe Related *********************************************//

//************ Thermometer with thermistor************ 

/*thermistor parameters:
 * RT0: 10 000 Ω
 * B: 3950 K +- 1.00%
 * T0:  25 C
 * +- 2%
 */

//These values are in the datasheet
#define RT0 10000   // Ω
#define B 3950      // K
//--------------------------------------


#define VCC 5    //Supply voltage
#define R 10000  //R=10KΩ

//Variables ------------------
float RT1; 
float RT2; 
float VR1; 
float VR2; 
float ln; 
float TX; 
float TX1;
float TX2;
float T0;
float VRT1; 
float VRT2;
 
//##################################################################################
//-----------  Do not Replace R1 with a resistor lower than 300 ohms    ------------
//##################################################################################
// Setup EC Meter 
 
int R1= 1000;
int Ra=25; //Resistance of powering Pins
int ECPin= A2;
int ECGround=A4;
int ECPower =A3;
 
 
//*********** Converting to ppm [Learn to use EC it is much better**************//
// Hana      [USA]        PPMconverion:  0.5
// Eutech    [EU]          PPMconversion:  0.64
//Tranchen  [Australia]  PPMconversion:  0.7
// Why didnt anyone standardise this?
 
 
float PPMconversion=0.64;
 
 
//*************Compensating for temperature ************************************//
//The value below will change depending on what chemical solution we are measuring
//0.019 is generaly considered the standard for plant nutrients [google "Temperature compensation EC" for more info
float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring
 
 
 
 
//********************** Cell Constant For Ec Measurements *********************//
//Mine was around 2.9 with plugs being a standard size they should all be around the same
//But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
float K=2.88;
 
 
 
 

 
//***************************** END Of Recomended User Inputs *****************************************************************//
 
 
float AirTemperature=10;
float WaterTemperature=10;
float EC=0;
float EC25 =0;
int ppm =0;
 
 
float raw= 0;
float Vin= 5;
float Vdrop= 0;
float Rc= 0;
float buffer=0;
 
 
 
 
//*********************************Setup - runs Once and sets pins etc ******************************************************//
void setup()
{
// Initiate Serial Communication
  Serial.begin(9600);


// Water Sensor Module
WaterSensor = analogRead(A5); //Acquisition analog value of Water Level Sensor (Resistance)


// Temperature Module 
T0 = 25 + 273.15;                 //Temperature T0 from datasheet, conversion from Celsius to kelvin
VRT1 = analogRead(A0);              //Acquisition analog value of VRT
                                             // Data wire For Temp Probe is plugged into pin A0 on the Arduino
VRT2 = analogRead(A1);              //Acquisition analog value of VRT
                                             // Data wire For Temp Probe is plugged into pin A0 on the Arduino
// EC Module
 
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);//Setting pin for sourcing current
  pinMode(ECGround,OUTPUT);//setting pin for sinking current
  digitalWrite(ECGround,LOW);//We can leave the ground connected permanantly
 
  delay(1000);// gives sensor time to settle
  //** Adding Digital Pin Resistance to [25 ohm] to the static Resistor *********//
  // Consule Read-Me for Why, or just accept it as true
  R1=(R1+Ra);// Taking into acount Powering Pin Resitance

 // Setup Text
 Serial.println("Make sure Probe and Temp Sensor are in Solution and solution is well mixed");
 Serial.println("Measurments at 5's Second intervals [Dont read Ec morre than once every 5 seconds]:");
 Serial.println("");
 Serial.println("Setup: Water Temp A0 Air Temp A1 EC Meter A2, A3, A4");
 Serial.println("    ");
 Serial.println("Starting Measurment:");

};
//******************************************* End of Setup **********************************************************************//
 
//  HAVE FUN :)
 
//************************************* Main Loop - Runs Forever ***************************************************************//
//Moved Heavy Work To subroutines so you can call them from main loop without cluttering the main loop

void loop()
{
WaterLevel();
DisplayTEMP(); // Code Calls 10x with 2 sec delay for Air and Water Temperature
GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
PrintReadings();  // Cals Print routine [below main loop]
delay(2000);
 
}
//************************************** End Of Main Loop **********************************************************************//
 
 
 
 
//************ These Loops are called From Main Loop************************//

void WaterLevel(){

if (WaterSensor >= 400) { Plant_WaterTank_Full = 1;
} 
else if (WaterSensor < 399) {
Plant_WaterTank_Low = 0;}
if (Plant_WaterTank_Full = 1) { Plant_Water_Status =1;
} 
else if (Plant_WaterTank_Low = 0) {
Plant_Water_Status = 0;
};

if (Plant_Water_Status = 0) { Serial.println("Plant Tank Status: No Water in Plant Tank");
delay(100);
} 
else if (Plant_Water_Status = 1) { Serial.println("Plant Tank Status: TANK FULL!");
delay(100);
};
}
//*********Reading Temperature Of Solution *******************//

void GetWaterTEMP(){
VRT1 = analogRead(A0);              //Acquisition analog value of VRT
  VRT1 = (5.00 / 1023.00) * VRT1;      //Conversion to voltage
  VR1 = VCC - VRT1;
  RT1 = VRT1 / (VR1 / R);               //Resistance of RT

  ln = log(RT1 / RT0);
  TX1 = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor

  TX1 = TX1 - 273.15;                 //Conversion to Celsius
  WaterTemperature = TX1;
}; 

//*********Reading Air Temperature  *******************//
void GetAirTEMP(){
VRT2 = analogRead(A1);              //Acquisition analog value of VRT
  VRT2 = (5.00 / 1023.00) * VRT2;      //Conversion to voltage
  VR2 = VCC - VRT2;
  RT2 = VRT2 / (VR2 / R);               //Resistance of RT

  ln = log(RT2 / RT0);
  TX2 = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor

  TX2 = TX2 - 273.15;                 //Conversion to Celsius
  AirTemperature = TX2;

}; 



//*********Reading EC Of Solution *******************//

void GetEC(){

 
//************Estimates Resistance of Liquid ****************//
digitalWrite(ECPower,HIGH);
raw= analogRead(ECPin);
raw= analogRead(ECPin);// This is not a mistake, First reading will be low beause if charged a capacitor
digitalWrite(ECPower,LOW);
 

 
 
//***************** Converts to EC **************************//
Vdrop= (Vin*raw)/1024.0;
Rc=(Vdrop*R1)/(Vin-Vdrop);
Rc=Rc-Ra; //acounting for Digital Pin Resitance
EC = 1000/(Rc*K);
 
 
//*************Compensating For Temperaure********************//
EC25  =  EC/ (1+ TemperatureCoef*(WaterTemperature-25.0));
ppm=(EC25)*(PPMconversion*1000);
 
 
;}
//************************** End OF EC Function ***************************//
 
 
 
 
//***This Loop Is called From Main Loop- Prints to serial usefull info ***//
void PrintReadings(){
Serial.print("Air Temperature: ");
Serial.print(AirTemperature);
Serial.println(" °C ");
Serial.print("Water Temperature: ");
Serial.print(WaterTemperature);
Serial.println(" °C     ");
Serial.print("Rc: ");
Serial.print(Rc);
Serial.print(" EC: ");
Serial.print(EC25);
Serial.print(" Simens  ");
Serial.print(ppm);
Serial.print(" ppm  ");
Serial.print(WaterTemperature);
Serial.println(" *C ");
 
 
/*
//********** Usued for Debugging ************
Serial.print("Vdrop: ");
Serial.println(Vdrop);
Serial.print("Rc: ");
Serial.println(Rc);
Serial.print(EC);
Serial.println("Siemens");
//********** end of Debugging Prints *********
*/
};
