const int red = 10;
const int yellow = 9;
const int green = 8;
const int button = 7;

void setup() {
  // put your setup code here, to run once:
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(button, INPUT);
  digitalWrite(green,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(button) == HIGH) {
    delay(15); //software debounce
    if (digitalRead(button) == HIGH) {
      changeLights();
    }
  }
}

void changeLights() {
  digitalWrite(green,LOW);
  digitalWrite(yellow,HIGH);
  delay(3000); //3 seconds

  digitalWrite(yellow,LOW);
  digitalWrite(red,HIGH);
  delay(5000); //5 seconds

  digitalWrite(yellow, HIGH);
  delay(2000); //2 seconds

  digitalWrite(yellow,LOW);
  digitalWrite(red,LOW);
  digitalWrite(green,HIGH);
  delay(3000); //3 seconds
}
