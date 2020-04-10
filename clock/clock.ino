#include <TM1637.h>
#include <Rtc_Pcf8563.h>
// Instantiation and pins configurations
// Pin 8 - > DIO
// Pin 9 - > CLK
TM1637 tm1637(9, 8);
Rtc_Pcf8563 rtc;

void setup()
{
  Serial.begin(9600);
  tm1637.init();
  tm1637.setBrightness(5);
}

void loop()
{


  tm1637.dispNumber(0000);
  tm1637.dispNumber(rtc.getSecond());
  delay(1000);
  tm1637.switchColon();

}
