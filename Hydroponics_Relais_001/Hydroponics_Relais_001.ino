#define HeizstabAquarium_Relais1 2 // Heizstab Aquarium auf Relais 1
#define PumpeAquarium_Relais2 3 // Pumpe Aquarium auf Relais 2
#define LichtAquarium_Relais3 4 // Licht Aquarium auf Relais 3
#define LichtGrow_Relais4 5 // Licht Grow auf Relais 4




void setup() {
  // put your setup code here, to run once:

  pinMode(HeizstabAquarium_Relais1, OUTPUT);
  pinMode(PumpeAquarium_Relais2, OUTPUT);
  pinMode(LichtAquarium_Relais3, OUTPUT);
  pinMode(LichtGrow_Relais4, OUTPUT);
  

}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(HeizstabAquarium_Relais1,LOW);
digitalWrite(PumpeAquarium_Relais2,LOW);
digitalWrite(LichtAquarium_Relais3,LOW);
digitalWrite(LichtGrow_Relais4,LOW);
delay (1000);
digitalWrite(HeizstabAquarium_Relais1,HIGH);
digitalWrite(PumpeAquarium_Relais2,LOW);
digitalWrite(LichtAquarium_Relais3,HIGH);
digitalWrite(LichtGrow_Relais4,LOW);
delay (1000);
digitalWrite(HeizstabAquarium_Relais1,LOW);
digitalWrite(PumpeAquarium_Relais2,HIGH);
digitalWrite(LichtAquarium_Relais3,LOW);
digitalWrite(LichtGrow_Relais4,HIGH);
delay (2000);

}

    
