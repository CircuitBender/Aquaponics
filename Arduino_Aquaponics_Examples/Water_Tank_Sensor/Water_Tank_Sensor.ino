int WaterDetectorGrow = 0;
int WaterDetectorFloor = 0;
int WaterDetectorGrowPin = A12;
int WaterDetectorFloorPin = A13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(WaterDetectorGrowPin, INPUT);
  pinMode(WaterDetectorFloorPin, INPUT);

}

void loop() {
  WaterDetectorGrow = analogRead(WaterDetectorGrowPin); //Acquisition analog value of Water Level Sensor (Resistance)
WaterDetectorFloor = analogRead(WaterDetectorFloorPin); //Acquisition analog value of Water Level Sensor (Resistance)
Serial.print(WaterDetectorGrow);
Serial.println(" WaterDetectorGrow");
Serial.print(WaterDetectorFloor);
Serial.println(" WaterDetectorFloor");

delay(2000);

}
