#define PumpeA_Relais1 3    // Pumpe Aquarium auf Relais 1
#define PumpeK_Relais2 5    // Pumpe Kreislauf auf Relais 2
#define LichtA_Relais3  6    // Licht Aquarium auf Relais 3
#define HeizstabA_Relais4 7  // Heizstab Aquarium auf Relais 4

// constants won't change. They're used here to
// set pin numbers:
#define buttonKP 8     // the number of the pushbutton pin

// variables will change:
int buttonStateKP = 0;         // variable for reading the pushbutton status
const long WateringTime = 26000L;
const long FlowbackTime = 70000L;
const long CycleTime = 180000L;

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
 
// ++++++++++++++++ DIGITAL BUTTON ++++++++++++++++
// initialize the pushbutton pin as an input:
  pinMode(buttonKP, INPUT);

// initialize Relais 1-4 as Output

  pinMode(PumpeA_Relais1, OUTPUT);
  pinMode(PumpeK_Relais2, OUTPUT);
  pinMode(LichtA_Relais3, OUTPUT);
  pinMode(HeizstabA_Relais4, OUTPUT);
  delay(500);        // delay in between reads for stability

// set Relays to 0
  digitalWrite(PumpeA_Relais1,HIGH);
  digitalWrite(PumpeK_Relais2,LOW);
  digitalWrite(LichtA_Relais3,HIGH);
  digitalWrite(HeizstabA_Relais4,HIGH);


  delay(1000);        // delay in between reads for stability
}

void loop() {

WateringPlants();

}

void WateringPlants  (){
  pushbutton();
  if (buttonStateKP == HIGH) {
    delay(CycleTime); 
  }
  else {
  delay(50);        // delay in between reads for stability
  };
}
void pushbutton (){  
// read the state of the pushbutton value:

  buttonStateKP = digitalRead(buttonKP);

 
 
    // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonStateKP == HIGH) {
    // turn LED on:
    digitalWrite(PumpeA_Relais1, LOW);
    digitalWrite(PumpeK_Relais2, HIGH);
    delay(WateringTime);        // delay in between reads for stability
    digitalWrite(PumpeK_Relais2, LOW);
    delay(FlowbackTime);        // delay in between reads for stability
    digitalWrite(PumpeA_Relais1, HIGH);
  } else {
    digitalWrite(PumpeK_Relais2, LOW);
    digitalWrite(PumpeA_Relais1, HIGH);

  };
 if (buttonStateKP == LOW) {
    // turn LED on:
     digitalWrite(PumpeK_Relais2, LOW);
    digitalWrite(PumpeA_Relais1, HIGH);
    
  } else {
    digitalWrite(PumpeK_Relais2, LOW);

  }; 
};
  

