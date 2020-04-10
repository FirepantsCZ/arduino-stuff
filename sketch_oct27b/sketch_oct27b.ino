const int button = 7;
const int buzzer = 6;

void setup() {
  // put your setup code here, to run once:
  pinMode(button, INPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  unsigned char i; // definuj proměnnou
  while (1) {
    // generace frekvence
    for (i = 0; i < 5; i++) {
      digitalWrite(buzzer, HIGH);
      delay(500); // počkat 1ms
      digitalWrite(buzzer, LOW);
      delay(500); // počkat 1ms
    }
  }
}
