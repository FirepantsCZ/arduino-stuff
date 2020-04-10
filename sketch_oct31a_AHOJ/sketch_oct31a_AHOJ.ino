#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 9
#define DIO 8

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   2000

const uint8_t SEG_DONE[] = {
  SEG_E | SEG_F | SEG_A | SEG_B | SEG_C | SEG_G,
  SEG_E | SEG_F | SEG_B | SEG_C | SEG_G,
  SEG_E | SEG_F | SEG_A | SEG_B | SEG_C | SEG_D,
  SEG_B | SEG_C | SEG_D | SEG_E
};

TM1637Display display(CLK, DIO);

void setup()
{
}

void loop()
{
  display.setBrightness(0x0a);
  display.setSegments(SEG_DONE);
  delay(1000);
  display.setBrightness(0x0f);
  display.setSegments(SEG_DONE);
  delay(1000);
}
