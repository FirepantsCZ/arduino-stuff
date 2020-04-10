#include <Wire.h>
#include <Rtc_Pcf8563.h>

#include <Arduino.h>
#include <LiquidCrystal.h>

const int pin_RS = 8;
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;
const int pin_BL = 10;

Rtc_Pcf8563 rtc;
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

int x;
int num = 0;
int mode = 0;
int modenum = 0;
char btn = "null";
uint8_t on = 0b01000000;

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Press Key:");
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

  x = analogRead (0);
  if (x < 60) {
    btn = "right";
  }
  else if (x < 200) {
    btn = "up";
  }
  else if (x < 400) {
    btn = "down";
  }
  else if (x < 600) {
    btn = "left";
  }
  else if (x < 800) {
    btn = ("Select");
    modenum++;
  }

  if ( (Second % 2) == 0) {
    on = 0b01000000;
  }
  else {
    on = 0;
  }
  while (Second == rtc.getSecond()) {
    lcd.clear();
    if ( (modenum % 2) == 0) {
      mode = 0;
    }
    else {
      mode = 1;
    }
    lcd.setCursor(2, 0);
    lcd.print(":");
    if (mode == 0) {
      lcd.setCursor(0, 0);
      if (Hour < 10) {
        lcd.print(0);
        lcd.setCursor(1, 0);
        lcd.print(Hour);
      }
      else {
        lcd.print(Hour);
      }
      lcd.setCursor(3, 0);
      if (Minute < 10) {
        lcd.print(0);
        lcd.setCursor(4, 0);
        lcd.print(Minute);
      }
      else {
        lcd.print(Minute);
      }
    }
    else {
      lcd.setCursor(3, 0);
      if (Second < 10) {
        lcd.print(0);
        lcd.setCursor(4, 0);
        lcd.print(Second);
      }
      else {
        lcd.print(Second);
      }
      lcd.setCursor(0, 0);
      if (Minute < 10) {
        lcd.print(0);
        lcd.setCursor(1, 0);
        lcd.print(Minute);
      }
      else {
        lcd.print(Minute);
      }
    }
    delay(50);
  }
}
