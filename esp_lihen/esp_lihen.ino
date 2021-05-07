#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Encoder.h>
#include <Regexp.h>
#include <Arduino.h>
#include <TM1637Display.h>

#define DHTTYPE DHT11
#define DHTPIN 5
//#define led 13
#define rele 13
#define CLK 4
#define DIO 0
#define clksw 2
#define pinB 14
#define pinA 12

char* mood = "auto";
int intemp;
int encoderPosCount = 0;
int aLast;
float goalTemp = 20.00;
char* state;
int pinAstateCurrent = LOW;
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
const long interval = 3000; //3000
const long loopinterval = 50;
unsigned long currentMillis;
int command;
boolean manOverrideOn;
boolean manOverrideOff;
String mes = "";
String newmes = "";

const char* ssid = "Kobra_11_v2";
const char* password = "i5LcnjLJ2x";
const char* mqtt_server = "10.0.1.17";

//const char* msgtemplate = "[{\"n\":\"teplota\",\"v\":%d}]";
const char* msgtemplate = "{\"mqtt_dashboard\":{\"temperature\":%d,\"humidity\":%d";

//char msgEvent[80] = "[{\"n\":\"teplota\",\"v\":23}]";
char msgEvent[80] = "";
char msgEvent2[80] = "";
char buf [100];

TM1637Display display(CLK, DIO);
DHT dht(DHTPIN, DHTTYPE);
Encoder myEnc(pinA, pinB);
WiFiClient espClient;
PubSubClient client(espClient);
MatchState ms;

long lastMsg = 0;
char msg[150];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  //mode":.*","
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    mes += (char)payload[i];
  }

  Serial.println();
  Serial.println(mes);
  /*char buffer1[32];r
  char buffer2[32];
  topic.toCharArray(buffer1, 32);
  "topic/tep".toCharArray(buffer2, 32);
  Serial.println(buffer1);
  Serial.println(buffer2);*/
  Serial.println(strcmp(topic, "lihen/tep"));
  /*if (strcmp(topic, "lihen/tep")) {
    Serial.println("THEY EQUAL");
  }*/

  int str_len = mes.length() + 1;
  char mes_arr[str_len];
  mes.toCharArray(mes_arr, str_len);

  /*ms.Target(mes_arr);

    //char result = ms.Match("(?<=\"setpoint\":)(.*)(?=,\"t)");
    char result = ms.Match("nt\":(.*),\"te");

    if (result > 0)
    {
    Serial.print ("Found match at: ");
    Serial.println (ms.MatchStart);
    Serial.print ("Match length: ");
    Serial.println (ms.MatchLength);
    newmes = "";
    for (int i = ms.MatchStart; i < ms.MatchStart + ms.MatchLength; i++) {
      Serial.print(mes_arr[i]);
      newmes += (char)mes_arr[i];
    }
    Serial.println();
    Serial.println(newmes);
    newmes.replace("nt\":", "");
    newmes.replace(",\"te", "");
    Serial.println(newmes);*/

  if (strcmp(topic, "lihen/settep") == 0) {
    Serial.println(mes);

    float floatmes = mes.toFloat();
    if (manOverrideOff == false && manOverrideOn == false) {
      Serial.print("Cílová teplota: ");
      Serial.println(floatmes);
      goalTemp = floatmes;
    }
  }

  /* else {
     Serial.println("No match found for [setpoint]");
    }*/

  //result = ms.Match("mode\":.*\",\"");

  /*if (result > 0)
    {
    Serial.print ("Found match at: ");
    Serial.println (ms.MatchStart);
    Serial.print ("Match length: ");
    Serial.println (ms.MatchLength);
    newmes = "";
    for (int i = ms.MatchStart; i < ms.MatchStart + ms.MatchLength; i++) {
      Serial.print(mes_arr[i]);
      newmes += (char)mes_arr[i];
    }
    Serial.println();
    Serial.println(newmes);
    newmes.replace("mode\":\"", "");
    newmes.replace("\",\"", "");
    Serial.println(newmes);*/

  if (strcmp(topic, "lihen/setmood") == 0) {
    newmes = mes;
    Serial.println("Mood arriveeedddd!!!");
    Serial.println(mes);
    Serial.println(newmes);
    if (newmes == "off" || newmes == "cool") {
      Serial.print("Setting off");
      //client.publish("lihen/tep", "{\"mqtt_dashboard\":{\"setpoint\":15}}");
      manOverrideOff = true;
      manOverrideOn = false;
      mood = "off";
      state = "false";
    }
    else if (newmes == "heat" || newmes == "Zahřívání") {
      Serial.print("Setting heat");
      //client.publish("lihen/tep", "{\"mqtt_dashboard\":{\"setpoint\":15}}");
      manOverrideOff = false;
      manOverrideOn = true;
      mood = "heat";
      state = "true";
    }
    else if (newmes == "auto") {
      Serial.print("Setting auto");
      manOverrideOff = false;
      manOverrideOn = false;
      mood = "auto";
    }
  }


  mes = "";

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      client.subscribe("lihen/settep");
      client.subscribe("lihen/tep");
      client.subscribe("lihen/setmood");
      client.subscribe("lihen/sta");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(led, OUTPUT);
  pinMode(rele, OUTPUT);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode (clksw, INPUT_PULLUP);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("Start měření");
  dht.begin();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();

  /*if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("lihen", msg);
    }*/
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    checkTemp();
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
      //if ((diff % 4) == 0) {
      //steps = diff / 4;
      //}
      //else {
      //steps = 0;
      //}
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
  delay(loopinterval);
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

  snprintf(msgEvent, sizeof(msgEvent), msgtemplate, (int)t, (int)h);
  snprintf(msgEvent2, sizeof(msgEvent2), ",\"mode\":\"%s\"}}", mood);
  String fullmsg = String(msgEvent) + msgEvent2;
  Serial.println(fullmsg);
  //client.publish("lihen/tep", msgEvent);
  //sprintf(msgEvent, msgtemplate, (int)h);
  char full_arr[150];
  fullmsg.toCharArray(full_arr, 150);
  Serial.println(full_arr);

  //client.publish("lihen/tep", full_arr);

  char simplemsg[25];
  String chart = String(t);
  chart.toCharArray(simplemsg, 25);

  char goalmsg[25];
  String gmg = String(goalTemp);
  gmg.toCharArray(goalmsg, 25);

  char hummsg[25];
  String hmh = String(h);
  hmh.toCharArray(hummsg, 25);
  
  client.publish("lihen/tep", simplemsg);
  client.publish("lihen/hum", hummsg);
  client.publish("lihen/mood", mood);
  client.publish("lihen/settep", goalmsg);
  //char tet[6] = "22";
  //client.publish("lihen/tep", tet);


  //client.publish("lihen/vlh", (int) h);

  //temp = sensors.getTempCByIndex(0);
  temp2 = t * 100;
  intemp = (int) temp2;
  if (manOverrideOn) {
    //digitalWrite(led, HIGH);
    digitalWrite(rele, HIGH);
  }
  else if (manOverrideOff) {
    //digitalWrite(led, LOW);
    digitalWrite(rele, LOW);
  }
  else {
    if (t >= goalTemp) {
      //digitalWrite(led, LOW);
      digitalWrite(rele, LOW);
      mood = "auto";
      state = "false";
    }
    else if (t <= goalTemp - 1){
      //digitalWrite(led, HIGH);
      digitalWrite(rele, HIGH);
      mood = "auto";
      state = "true";
    }
  }
  client.publish("lihen/sta", state);
}
