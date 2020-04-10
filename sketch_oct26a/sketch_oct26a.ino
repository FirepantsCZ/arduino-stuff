const int keyPin = 10;
const int ledPinInt = 13;

void setup() {
  // put your setup code here, to run once:
  pinMode(keyPin, INPUT);
  pinMode(ledPinInt, OUTPUT);
  digitalWrite(ledPinInt, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
    if (digitalRead(keyPin) == HIGH) {
      digitalWrite(ledPinInt, HIGH);
  }
  else {
    digitalWrite(ledPinInt, LOW);
  }
}
