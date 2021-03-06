/*#include <OneWire.h>
  #include <DallasTemperature.h>*/
#include <DHT.h>
#include <Encoder.h>
#include <Arduino.h>
#include <TM1637Display.h>
#include <IRremote.h>

#define DHTTYPE DHT11
#define DHTPIN 12
//#define tempin 12
#define infr_pin 5
#define led 11
#define CLK 9
#define DIO 8
#define clksw 4
#define pinB 3
#define pinA 2
#define rele 10
int intemp;
int encoderPosCount = 0;
int aLast;
float goalTemp = 20.00;
int pinAstateCurrent = LOW;                // Current state of Pin A
int pinAStateLast = pinAstateCurrent;
boolean bCW;
boolean setupMode = false;
int aVal;
int switchState;
float temp2;
float temp;
long newPosition;
long oldPosition  = -999;
long diff;
float intGoal;
long steps;
unsigned long previousMillis = 0;
const long interval = 3000;
unsigned long currentMillis;
int command;
boolean manOverrideOn;
boolean manOverrideOff;

DHT dht(DHTPIN, DHTTYPE);
Encoder myEnc(pinA, pinB);
/*OneWire oneWire(tempin);
  DallasTemperature sensors(&oneWire);*/
TM1637Display display(CLK, DIO);
IRrecv irrecv(infr_pin);
decode_results results;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(rele, OUTPUT);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode (clksw, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(pinB), rotup, CHANGE);
  Serial.begin(9600);
  Serial.println("Start měření");
  //sensors.begin();
  dht.begin();
  irrecv.enableIRIn();
}

void loop() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    checkTemp();
  }

  if (irrecv.decode()) {
    irrecv.printIRResultShort(&Serial);
    command = irrecv.decodedIRData.command;
    Serial.println(command);
    switch (command) {
      case 1:
        if (setupMode) {
          setupMode = false;
          Serial.println("Konec nastavovacího režimu");
        }
        else {
          setupMode = true;
          Serial.println("Začátek nastavovacího režimu");
        }
        break;
      case 2:
        if (setupMode) {
          goalTemp = goalTemp + 0.5;
        }
        break;
      case 3:
        if (setupMode) {
          goalTemp = goalTemp - 0.5;
        }
        break;
      case 4:
        manOverrideOn = true;
        manOverrideOff = false;
        digitalWrite(led, HIGH);
        digitalWrite(rele, HIGH);
        break;
      case 5:
        manOverrideOn = false;
        manOverrideOff = true;
        digitalWrite(led, LOW);
        digitalWrite(rele, LOW);
        break;
      case 6:
        manOverrideOn = false;
        manOverrideOff = false;
        break;
    }
    irrecv.resume();
  }

  switchState = digitalRead(clksw);
  if (switchState == LOW) {
    delay(250);
    if (setupMode) {
      setupMode = false;
      Serial.println("Konec nastavovacího režimu");
    }
    else {
      setupMode = true;
      Serial.println("Začátek nastavovacího režimu");
    }
  }

  if (setupMode) {
    newPosition = myEnc.read();
    delay(100);
    if (newPosition != oldPosition) {
      diff = newPosition - oldPosition;
      if (diff > 0) {
        steps = 1;
      }
      else if (diff < 0) {
        steps = -1;
      }
      /*if ((diff % 4) == 0) {
        steps = diff / 4;
        }
        else {
        steps = 0;
        }*/
      goalTemp = goalTemp + (steps * 0.5);
      oldPosition = newPosition;
      Serial.print("Stepped by: ");
      Serial.println(steps);
      Serial.print("Difference: ");
      Serial.println(diff);
      Serial.print("Current position: ");
      Serial.println(newPosition);
      Serial.print("Cílová teplota: ");
      Serial.println(goalTemp);
    }
  }
  display.setBrightness(0x08);
  if (setupMode) {
    intGoal = (int) (goalTemp * 100);
    display.showNumberDecEx(intGoal, 0b01000000, false);
  }
  else {
    display.showNumberDecEx(intemp, 0b01000000, false);
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void checkTemp() {
  digitalWrite(LED_BUILTIN, LOW);
  //sensors.requestTemperatures();

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Error while reading the sensor");
    return;
  }

  Serial.println("-----------------------------------------------------------");
  Serial.print("Vlhkost: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Teplota: ");
  Serial.print(t);
  Serial.println("°C");
  Serial.println("-----------------------------------------------------------");

  //temp = sensors.getTempCByIndex(0);
  temp2 = t * 100;
  intemp = (int) temp2;
  if (manOverrideOn) {
    digitalWrite(led, HIGH);
    digitalWrite(rele, HIGH);
  }
  else if (manOverrideOff) {
    digitalWrite(led, LOW);
    digitalWrite(rele, LOW);
  }
  else {
    if (t >= goalTemp) {
      digitalWrite(led, LOW);
      digitalWrite(rele, LOW);
    }
    else {
      digitalWrite(led, HIGH);
      digitalWrite(rele, HIGH);
    }
  }
}

/* void rotup() {
  delay(50);
  if (setupMode) {
    switchState = digitalRead(clksw);
    aVal = digitalRead(pinA);

    if ((aLast == LOW) && (aVal == HIGH)) {
      if (digitalRead(pinB) == HIGH) {
        Serial.println("Right");
        goalTemp = goalTemp - 0.5;
        Serial.println(goalTemp);
      } else {
        Serial.println("Left");
        goalTemp = goalTemp + 0.5;
        Serial.println(goalTemp);
      }
    }
    aLast = aVal;
    if (switchState == LOW) {
      setupMode = false;
      Serial.print("Cílová teplota nastavena: ");
      Serial.println(goalTemp);
      rotup();
    }
  }
  }*/
