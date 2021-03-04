#define Fishfeeder 31  // 3.3v Fishfeeder Relay
uint32_t previousMillisFeeding = 0;
uint32_t FeedingQuantity = 2000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Frishfeeder Setup");

  pinMode(Fishfeeder, OUTPUT);
  digitalWrite(Fishfeeder, HIGH);

  Serial.println("Fishfeeder OK");
  digitalWrite(Fishfeeder, LOW);


}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t q = 0;

  do {
    unsigned long currentMillisFeeding = millis();
    if ((unsigned long)(currentMillisFeeding - previousMillisFeeding) >= FeedingQuantity) {

      digitalWrite(Fishfeeder, HIGH);
      Serial.println("Fishfeeder ON");

      q++;
      previousMillisFeeding = currentMillisFeeding;
    }
  }
  while (q < (FeedingQuantity / (unsigned)1000));

  digitalWrite(Fishfeeder, LOW);
  Serial.println(" Fishfeeder OFF");

  delay(1000);


}
