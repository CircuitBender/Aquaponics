

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

}

void loop() {
int WaterSensor = analogRead(A5); //Acquisition analog value of Water Level Sensor (Resistance)

switch (WaterSensor) {
    case 1 >= 580:
      {
      int Plant_Water_Status = 0;
      }
      break;
      case 2 < 579:
      { int Plant_Water_Status = 1;
      }
    break;
  }
  int Plant_Water_Status;

Serial.println(" ");

Serial.print(Plant_Water_Status);
Serial.println(" ");

if (Plant_Water_Status = 1) { Serial.println("Plant_Water_Status: FULL !"); }
if (Plant_Water_Status = 0) { Serial.println("Plant_Water_Status: OK !"); }

delay(2000);

}
