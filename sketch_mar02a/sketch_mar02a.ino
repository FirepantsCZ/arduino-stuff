#include <RTClib.h>

void setup() {
Serial.begin(9600);
char tyme[20] = [DateTime(F(__TIME__))]
Serial.print(tyme);
}

void loop() {
  // put your main code here, to run repeatedly:

}
