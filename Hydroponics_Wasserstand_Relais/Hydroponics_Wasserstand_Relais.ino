#define PumpeAquarium_Relais1 2 // Heizstab Aquarium auf Relais 1
#define PumpeKreislauf_Relais2 3 // Pumpe Aquarium auf Relais 2
#define LichtAquarium_Relais3  4 // Licht Aquarium auf Relais 3
#define HeizstabAquarium_Relais4 5 // Licht Grow auf Relais 4

 //+++++++++++ WATER LEVEL ++++++++++++++++
 // read the input on analog pin A5:
int WaterLevelSensor = analogRead(A5);

//++++++++++++ digitalbutton Constants & Variables +++++++++
// constants won't change. They're used here to
// set pin numbers:
int buttonKreislaufpumpe = digitalRead(6);     // the number of the pushbutton pin
// variables will change:
int buttonStateKP = 0;         // variable for reading the pushbutton status

void setup() {
// initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
 
// ++++++++++++++++ DIGITAL BUTTON ++++++++++++++++
// initialize the pushbutton pin as an input:
  pinMode(buttonKreislaufpumpe, INPUT);

// initialize Relais 1-4 as Output

  pinMode(PumpeAquarium_Relais1, OUTPUT);
  pinMode(PumpeKreislauf_Relais2, OUTPUT);
  pinMode(LichtAquarium_Relais3, OUTPUT);
  pinMode(HeizstabAquarium_Relais4, OUTPUT);
  delay(500);        // delay in between reads for stability

// set Relays to 0
digitalWrite(PumpeAquarium_Relais1,LOW);
digitalWrite(PumpeKreislauf_Relais2,LOW);
digitalWrite(LichtAquarium_Relais3,LOW);
digitalWrite(HeizstabAquarium_Relais4,LOW);

delay(500);        // delay in between reads for stability
delay(500);        // delay in between reads for stability

};


// the loop routine runs over and over again forever:
void loop() {
  
  if (buttonStateKP == HIGH) {
    // turn LED on:
    digitalWrite(PumpeAquarium_Relais1, LOW);
    delay(500);        // delay in between reads for stability
    delay(500);        // delay in between reads for stability
    Serial.println("Circuit Pump OFF - Aquarium Pump ON");
    delay(500);        // delay in between reads for stability
    } 
    else {
    // turn LED off:
    digitalWrite(PumpeKreislauf_Relais2, LOW);
    delay(500);        // delay in between reads for stability
    digitalWrite(PumpeAquarium_Relais1, HIGH);
    Serial.println("PumpeKreislauf_Relais2 ON Watering Plants");
    delay(500);        // delay in between reads for stability  
  }
    
delay(500);        // delay in between reads for stability

}
