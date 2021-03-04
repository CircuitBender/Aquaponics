int in1 = 2;

void setup() {
  pinMode(in1, OUTPUT);
  digitalWrite(in1, LOW);
}

void loop() {
  digitalWrite(in1, HIGH);
  delay(3000);
  digitalWrite(in1, LOW);
  delay(3000);
}
