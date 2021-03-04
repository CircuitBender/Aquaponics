#define FeederPin 13
void setup() {
    Serial.begin(9600);

  pinMode(FeederPin, OUTPUT);
}

void loop() {
    Serial.println("ON");

  analogWrite(FeederPin, 200);
  delay(5000);
  Serial.println("OFF");
  analogWrite(FeederPin, 0);
  delay(5000);

}
