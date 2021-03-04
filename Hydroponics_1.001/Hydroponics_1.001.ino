// ************HYDROPONICS 1.0 *********************
// author: Marco Heinzen
//licencing: prototype, for private use only !

// constants won't change. They're used here to
// set relays numbers:
#define PumpeA_Relais1 2    // Pumpe Aquarium auf Relais 1
#define PumpeK_Relais2 3    // Pumpe Kreislauf auf Relais 2
#define LichtA_Relais3  4    // Licht Aquarium auf Relais 3
#define Heizstab_Relais4 5  // Heizstab Aquarium auf Relais 4
#define LichtB_Relais5  6    // Licht Aquarium auf Relais 3
#define AirIN_Relais6  7    // Licht Aquarium auf Relais 3
#define AirOUT_Relais7  8    // Licht Aquarium auf Relais 3
#define Leer_Relais8  9    // Licht Aquarium auf Relais 3

// set pin numbers of button:
#define  buttonKP 11   // the number of the pushbutton pin

// set data pin of DHT11
#define DHT11 12

// set Thermistor 1 for Water
#define Thermistor1 A0
// set Thermistor 2 for Air
#define Thermistor2 A1

// set EC Probe 1
// set EC Probe 2
// set Water Level Probe 1
// set Water Level Probe 2

// set RTC ports
#define Thermistor2 A1


// constant variables
// Watering time:
const int WateringTime = 17000;
const long FlowbackTime = 110000L;

// variables will change:
int buttonStateKP = 0;         // variable for reading the pushbutton status

// SETUP, RUNS ONCE

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // initialize the pushbutton pin as an input:
  pinMode(buttonKP, INPUT);

  // initialize Relais 1-4 as Output

  pinMode(PumpeA_Relais1, OUTPUT);
  pinMode(PumpeK_Relais2, OUTPUT);
  pinMode(LichtA_Relais3, OUTPUT);
  pinMode(Heizstab_Relais4, OUTPUT);
  delay(500);        // delay in between reads for stability

  // set Relays to 0
  digitalWrite(PumpeA_Relais1, LOW);
  digitalWrite(PumpeK_Relais2, LOW);
  digitalWrite(LichtA_Relais3, LOW);
  digitalWrite(Heizstab_Relais4, LOW);
  delay(500);        // delay in between reads for stability

  // set Relays to standard
  digitalWrite(PumpeA_Relais1, HIGH);
  digitalWrite(PumpeK_Relais2, LOW);
  digitalWrite(LichtA_Relais3, HIGH);
  digitalWrite(Heizstab_Relais4, LOW);

  delay(500);        // delay in between reads for stability
}

// MAIN LOOP
void loop() {
  // put your main code here, to run repeatedly:

  WateringPlants();
  delay(100);
};

// SUB LOOPS

// WATERING PLANTS
void WateringPlants() {
  buttonStateKP = digitalRead(buttonKP);
  if (buttonStateKP == HIGH ) {
    // turn PumpeK_Relais2 ON, Watering Plants
    digitalWrite(PumpeA_Relais1, LOW);
    digitalWrite(PumpeK_Relais2, HIGH);
    delay(WateringTime);        // Time Watering
    digitalWrite(PumpeK_Relais2, LOW);
    delay(FlowbackTime);        // delay in between reads for stability
    digitalWrite(PumpeA_Relais1, HIGH);

  }
  else {
    digitalWrite(PumpeK_Relais2, LOW);
    digitalWrite(PumpeA_Relais1, HIGH);

  };

};

