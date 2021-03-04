Global:
//Inside thermistor on Arduino ADC pin 0, Outside on pin 1.
#define THERMISTORPinInside A0
#define THERMISTORPinOutside A1
//One temperature monitoring thermistor for Inside and Outside.
Thermistor4 ThermistorInside, ThermistorOutside;
unsigned long ThermistorLastMillis; //last time something was run.
//If using protothreads...
static struct pt ptv_ThermistorReport; //these hold the states of the PT's.

void setup(){
//My 2 salvaged thermistors are about 30k at room temperature.
ThermistorInside.Pin = THERMISTORPinInside; //Set the pin number.
ThermistorOutside.Pin = THERMISTORPinOutside;
ThermistorInside.SetUp(); //Sets up the analog read pin for internal AVR.
ThermistorOutside.SetUp(); //If using an external ADC, write your own setup here.
//pow() is used elsewhere so might as well be used here.
ThermistorInside.BitResolution=pow(2, 10)-1; //ATmega's have a 10bit ADC (2^10-1).
ThermistorOutside.BitResolution=pow(2, 10)-1; //An external ADC may be lower or higher than 10bits.
ThermistorInside.VoltageSupply=5; //My USB powers my ATmega325 chip at 4.95v. Meter this for accuracy.
ThermistorOutside.VoltageSupply=5; //An external ADC may have different voltages. Meter this for accuracy.
ThermistorInside.ResistanceFixed=27200; //Fixed resistor in the divider. Measured in ohms. Meter this for accuracy.
ThermistorOutside.ResistanceFixed=27100; //The resistor divider should be calculated to maximize desired range.
ThermistorInside.Offset=0.5; //adjust temperature in Kelvin up or down a little to account for unforseen variations.
ThermistorOutside.Offset=0.5; //This will be by trial and error during final manual calibration.
//These numbers were generated from thermistor.sf.net and aren't quite right unless using the full equation.
ThermistorInside.SteinhartA1=1.560442157476244e-003;  //First Steinhart-Hart coefficient.
ThermistorInside.SteinhartA2=-1.298742723052728e-005; //Second Steinhart-Hart coefficient.
ThermistorInside.SteinhartA3=2.500508035979886e-005;  //Third Steinhart-Hart coefficient.
ThermistorInside.SteinhartA4=-7.698170259653937e-007; //Fourth Steinhart-Hart coefficient.
ThermistorOutside.SteinhartA1=2.975623989921602e-003;  //First Steinhart-Hart coefficient.
ThermistorOutside.SteinhartA2=-4.448067018378571e-004; //Second Steinhart-Hart coefficient.
ThermistorOutside.SteinhartA3=6.848365975770642e-005;  //Third Steinhart-Hart coefficient.
ThermistorOutside.SteinhartA4=-2.217688558250577e-006; //Fourth Steinhart-Hart coefficient.
//If using proto-threads, PT's need initializing before use.
PT_INIT(&ptv_ThermistorReport);
}
void loop(){
//PROTOTHREAD VERSION:
//loop forever and let the PT scheduler decide what needs running.
//The PT's will not run until their functions are called.
//If the functions exit, being in the loop will restart the thread.
//Each thread gets its own "pt" state variable.
//Since arguments and local function variables can't be used, use the class object's.
ptf_ThermistorReport(&ptv_ThermistorReport);
//This function exists somewhere else and acts on the thermistor values.

//NORMAL VERSION:
ThermistorInside.ReadCalculate(3);
Serial.print("Inside Temp: "); Serial.println(ThermistorInside.GetFarenheit(), 2);
ThermistorOutside.ReadCalculate(3);
Serial.print("Outside Temp: "); Serial.println(ThermistorOutside.GetFarenheit(), 2);

//Example CSV output that can be extracted from the other information and plotted.
//unix/linux: grep "^CSV," > outfile.csv
ThermistorInside.ReadCalculate(3);
Serial.print("CSV,"); Serial.print(ThermistorInside.GetFarenheit(), 2);
Serial.print(","); Serial.print(ThermistorInside.GetCentigrade(), 2);
ThermistorOutside.ReadCalculate(3);
Serial.print(","); Serial.print(ThermistorOutside.GetFarenheit(), 2);
Serial.print(","); Serial.println(ThermistorOutside.GetCentigrade(), 2);

*/

#ifndef THERMISTOR4_H
#define THERMISTOR4_H

//Bring in the Arduino stuff since it gets called.
#include <WProgram.h>
//for log() (natural logarithm, not log10).
#include <math.h>

//Set to 1 to include debug code, set to 0 to exclude.
//Extra prints add about 1k to the binary.
//This is convenient for calibrating, but usually unnecessary for a final project.
#define THERMISTORDEBUG 1

// Thermistor class to consolidate various variables and functions.
class Thermistor4 {

public:
//note: floats and doubles are both 4 bytes in avr-gcc.
unsigned char Pin; // analog pin number on the Arduino board the thermistor circuit is connected to.
unsigned int BitResolution; // such as an 8bit, 10bit, or 12bit ADC. Most newer Arduino boards are 10bit.
unsigned int ADCReading; // last reading from ADC.
double VoltageSupply; // supply voltage of the thermistor-divider. Manually meter this for accuracy.
double VoltageReading; // current voltage in the middle point of the thermistor-divider.
unsigned long ResistanceFixed; // fixed resistor between thermistor and ground, measured in ohms. Meter this for accuracy.
unsigned long ResistanceThermistor; // last calculated resistance of the thermistor, measured in ohms.
double SteinhartA1; // Calibrate/recalibrate the thermistor from: http://thermistor.sourceforge.net
double SteinhartA2; // SF and real SH coefficients are somehow different.
double SteinhartA3; // These 4 Steinhart-Hart coefficients are different for every thermistor...
double SteinhartA4; // ...and will help make sure the thermistor calculations are accurate over a wide range.
double Temperature; // calculated temperature in Kelvin.
double Offset; // adjust temperature in Kelvin up or down a little to account for unforseen variations.

//Thermistor4(); //Constructor, removed for size.
void SetUp();
void ReadADC(unsigned int);
void CalculateTemperature(unsigned char);
void ReadCalculate(unsigned char);
double GetCentigrade();
double GetFarenheit();
#if THERMISTORDEBUG
void Thermistor4SerialPrint();
#endif
};

#endif
}
//end of Thermistor4.h
