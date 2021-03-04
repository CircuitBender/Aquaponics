


//*********************************Setup - runs Once and sets pins etc ******************************************************//
void setup()
{
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  T0 = 25 + 273.15;                 //Temperature T0 from datasheet, conversion from Celsius to kelvin
  VRT1 = analogRead(A0);              //Acquisition analog value of VRT
  VRT2 = analogRead(A1);              //Acquisition analog value of VRT



  delay(1000);// gives sensor time to settle

  //** Adding Digital Pin Resistance to [25 ohm] to the static Resistor *********//
  // Consule Read-Me for Why, or just accept it as true

  Serial.println("Make sure Probe and Temp Sensor are in Solution and solution is well mixed");
  Serial.println("Measurments at 5's Second intervals [Dont read Ec morre than once every 5 seconds]:");
  Serial.println("");
  Serial.println("Setup: Water Temp A0 Air Temp A1 EC Meter A2, A3, A4");
  Serial.println("    ");
  Serial.println("Starting Measurment:");

}
//******************************************* End of Setup **********************************************************************//




//************************************* Main Loop - Runs Forever ***************************************************************//
//Moved Heavy Work To subroutines so you can call them from main loop without cluttering the main loop

  void loop() {
  GetWaterTEMP();
  GetAirTEMP();
  DisplayTEMP(); // Code Calls 10x with 2 sec delay for Air and Water Temperature
  delay(2000);

}
//************************************** End Of Main Loop **********************************************************************//




//************ This Loop Is called From Main Loop************************//
//*********Reading Temperature Of Solution *******************//
void GetWaterTEMP() {
  VRT1 = analogRead(A0);              //Acquisition analog value of VRT
  VRT1 = (5.00 / 1023.00) * VRT1;      //Conversion to voltage
  VR1 = VCC - VRT1;
  RT1 = VRT1 / (VR1 / R);               //Resistance of RT

  ln = log(RT1 / RT0);
  TX1 = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor

  TX1 = TX1 - 273.15;                 //Conversion to Celsius
  WaterTemperature = TX1;
  Serial.print("Water Temperature: ");
  Serial.print(WaterTemperature);
  Serial.print(" °C     ");
};
//*********Reading Air Temperature  *******************//
void GetAirTEMP() {
  VRT2 = analogRead(A1);              //Acquisition analog value of VRT
  VRT2 = (5.00 / 1023.00) * VRT2;      //Conversion to voltage
  VR2 = VCC - VRT2;
  RT2 = VRT2 / (VR2 / R);               //Resistance of RT

  ln = log(RT2 / RT0);
  TX2 = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor

  TX2 = TX2 - 273.15;                 //Conversion to Celsius
  AirTemperature = TX2;
  Serial.print("Air Temperature: ");
  Serial.print(AirTemperature);
  Serial.println(" °C ");
};
//*********Looping Temperatures  *******************//
void DisplayTEMP() {
  GetWaterTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  GetAirTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  delay(2000);
  GetWaterTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  GetAirTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  delay(2000);
  GetWaterTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  GetAirTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  delay(2000);
  GetWaterTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  GetAirTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  delay(2000);
  GetWaterTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  GetAirTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  delay(2000);
  GetWaterTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  GetAirTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  delay(2000);
  GetWaterTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  GetAirTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  delay(2000);
  GetWaterTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  GetAirTEMP(); // Calls Code to get Temperature reading from Thermistor on Pin A0
  delay(2000);


}

//*********Reading EC Of Solution *******************//


