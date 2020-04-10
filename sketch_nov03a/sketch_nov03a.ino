#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 9
#define DIO 8
TM1637Display display(CLK, DIO);
const int button = 10;
const int start = 11;
const int led = 12;
int num = 0;

void setup()
{
  pinMode(button, INPUT);
  pinMode(start, INPUT);
  pinMode(led, OUTPUT);
}

void loop()
{
  display.setBrightness(0x0f);
  if (digitalRead(start) == HIGH) {
    for (int i; num >= 1; num--) {
      display.showNumberDec(num);
      delay(1000);
    }
  }
  if (num == 0) {
    digitalWrite(led, HIGH);
  }
  else {
    digitalWrite(led, LOW);
  }
  if (digitalRead(button) == HIGH) {
    num = num + 1;
    display.showNumberDec(num);
  }
  else {
    display.showNumberDec(num);
  }
  delay(250);
}
