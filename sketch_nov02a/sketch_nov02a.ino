#include <Wire.h>
#include <Rtc_Pcf8563.h>

#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 9
#define DIO 8
Rtc_Pcf8563 rtc;
TM1637Display display(CLK, DIO);
const int button = 10;
int num = 0;
uint8_t on = 0b01000000;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print("Hour:");
  Serial.println(rtc.getHour());
  int Hour = rtc.getHour();
  Serial.print("Minute:");
  Serial.println(rtc.getMinute());
  int  Minute = rtc.getMinute();
  Serial.print("Second:");
  Serial.println(rtc.getSecond());
  int Second = rtc.getSecond();
  Serial.print("Weekday:");
  Serial.println(rtc.getWeekday());
  int Weekday = rtc.getWeekday();
  Serial.print("Day:");
  Serial.println(rtc.getDay());
  int Day = rtc.getDay();
  Serial.print("Month:");
  Serial.println(rtc.getMonth());
  int Month = rtc.getMonth();
  Serial.print("Year:");
  Serial.println(rtc.getYear());
  int Year = rtc.getYear();
  display.setBrightness(0x0f);

  if ( (Second % 2) == 0) {
    on = 0b01000000;
  }
  else {
    on = 0;
  }

  display.showNumberDec(Second, true, 2, 2);
  display.showNumberDecEx(Minute, on, true, 2, 0);

  while (Second == rtc.getSecond()) {
    delay(1);
  }
}
