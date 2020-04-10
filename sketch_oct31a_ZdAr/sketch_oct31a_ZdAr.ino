#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 9
#define DIO 8

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   2000

const uint8_t ZdAr[] = {
  SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,
  SEG_B | SEG_G | SEG_E | SEG_D | SEG_C,
  SEG_E | SEG_F | SEG_A | SEG_B | SEG_C | SEG_G,
  SEG_E | SEG_G
};

TM1637Display display(CLK, DIO);

void setup()
{
}

void loop()
{
  display.setBrightness(0x08);
  display.setSegments(ZdAr);
  delay(1000);
  display.setBrightness(0x0f);
  display.setSegments(ZdAr);
  delay(1000);
}
