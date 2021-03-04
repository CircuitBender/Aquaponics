//////////////////////////////////////////////////////////////////
//©2011 bildr
//Released under the MIT License - Please reuse change and share
//Simple code to output a PWM sine wave signal on pin 9
//////////////////////////////////////////////////////////////////

#define Vent_Mosfet_Tech1OUT_Pin1 5
#define Vent_Mosfet_Tech2IN_Pin2 6
#define Vent_Mosfet_Tech3OUT_Pin3 7
#define Vent_Mosfet_Grow1IN_Pin4 8
#define Vent_Mosfet_Grow2OUT_Pin5 9
#define Vent_Mosfet_Aqua1_Pin6 11
#define Vent_Mosfet_Aqua2_Pin7 12

void setup() {
  pinMode(Vent_Mosfet_Tech1OUT_Pin1, OUTPUT);
  pinMode(Vent_Mosfet_Tech2IN_Pin2, OUTPUT);
  pinMode(Vent_Mosfet_Tech3OUT_Pin3, OUTPUT);
  pinMode(Vent_Mosfet_Grow1IN_Pin4, OUTPUT);
  pinMode(Vent_Mosfet_Grow2OUT_Pin5, OUTPUT);
  pinMode(Vent_Mosfet_Aqua1_Pin6, OUTPUT);
  pinMode(Vent_Mosfet_Aqua2_Pin7, OUTPUT);

}

void loop() {

  /* for (int i = 0; i < 360; i++) {
     //convert 0-360 angle to radian (needed for sin function)
     float rad = DEG_TO_RAD * i;

     //calculate sin of angle as number between 0 and 255
     int sinOut = constrain((sin(rad) * 128) + 128, 0, 255);

     //  analogWrite(fadePin, sinOut);
  */
  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 255);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 255);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 255);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 255);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 255);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 255);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 255);

  delay(180000);
  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 0);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 0);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 0);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 0);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 0);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 0);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);


  delay(5000);
  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 200);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 200);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 200);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 200);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 200);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 200);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 200);


  delay(10000);
  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 0);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 0);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 0);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 0);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 0);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 0);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);


  delay(5000);
  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 150);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 150);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 150);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 150);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 150);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 150);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 150);

  delay(10000);
  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 0);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 0);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 0);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 0);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 0);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 0);

  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);


  delay(5000);
  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 100);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 100);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 100);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 100);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 100);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 100);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 100);


  delay(10000);
  analogWrite(Vent_Mosfet_Tech1OUT_Pin1, 0);
  analogWrite(Vent_Mosfet_Tech2IN_Pin2, 0);
  analogWrite(Vent_Mosfet_Tech3OUT_Pin3, 0);
  analogWrite(Vent_Mosfet_Grow1IN_Pin4, 0);
  analogWrite(Vent_Mosfet_Grow2OUT_Pin5, 0);
  analogWrite(Vent_Mosfet_Aqua1_Pin6, 0);
  analogWrite(Vent_Mosfet_Aqua2_Pin7, 0);

}
