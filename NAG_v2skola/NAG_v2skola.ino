//NodeMCU
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <BH1750FVI.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
BH1750FVI LightSensor(BH1750FVI::k_DevModeOneTimeLowRes);
int sensorPin = A0;    // select the input analog sensor pin

    uint16_t voltage;
    uint16_t light;
    int16_t tempHTU;
    uint16_t humHTU;


extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
}

//define SSID, password, mqtt server
static const char* ssid = "spsvos-student";
static const char* username = "iot";
static const char* password = "Heslo123";
/*
const char* mqtt_server = "connect.hexio.cloud";
const char* mqtt_user = "5l6jGB04"; //https://grafana.hexio.cloud/login - sps.zaci passwd:SPSvos123
const char* mqtt_password = "c647S61AK4";

const char* outTopic1 = "HTUteplota";
const char* outTopic2 = "HTUhumidity";
const char* outTopic3 = "BH1750light";

const char* msgtemplate = "[{\"n\":\"teplota\",\"v\":%d}]";

char msgEvent[80] = "[{\"n\":\"teplota\",\"v\":23}]"; // znakový řetězec znaků naměřené hodnoty
*/
// WiFiClient espClient;
//PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.print("username: ");
  Serial.println(username);
  Serial.print("password: ");
  Serial.println(password);  
  // WPA2 Connection starts here
  // Setting ESP into STATION mode only (no AP mode or dual mode)
    wifi_set_opmode(STATION_MODE);
    struct station_config wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config));
    strcpy((char*)wifi_config.ssid, ssid);
    wifi_station_set_config(&wifi_config);
    wifi_station_clear_cert_key();
    wifi_station_clear_enterprise_ca_cert();
    wifi_station_set_wpa2_enterprise_auth(1);
    wifi_station_set_enterprise_identity((uint8*)username, strlen(username));
    wifi_station_set_enterprise_username((uint8*)username, strlen(username));
    wifi_station_set_enterprise_password((uint8*)password, strlen(password));
    wifi_station_connect();
  // WPA2 Connection ends here
  // Wait for connection AND IP address from DHCP
  Serial.println();
  Serial.println("Waiting for connection and IP Address from DHCP");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
/*
void reconnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP8266Client";
    clientId += String(random(0xffff), HEX);
    
   if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
 // if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
*/
void setup() {
  //initialize Serial Monitor
  Serial.begin(74880);
  while (!Serial);
  Serial.println("\WiFi-SPS, NAG-mqtt sender-ZCU \n");

  // setup_wifi();
/*  client.setServer(mqtt_server, 1883);
  delay(100);
Serial.println("\n mqtt_server: ");
Serial.print("mqtt_user: ");
  Serial.println(mqtt_user);
  Serial.print("mqtt_password: ");
  Serial.println(mqtt_password);
  */

//    pinMode(3, FUNCTION_3);
    pinMode(4, OUTPUT); //voltage measuring - GND
    digitalWrite(4, LOW);
    
     Wire.begin(2, 0);           // set I2C pins (SDA = GPIO2, SCL = GPIO0), default clock is 100kHz

        LightSensor.begin();
   light = LightSensor.GetLightIntensity();
   Serial.print("Light: "); 
   Serial.println(light);

  if (!htu.begin())
  { 
   Serial.println("Error! No HTU Sensor Detected!!!");
  }

   tempHTU = 100*htu.readTemperature();
   humHTU = 100*htu.readHumidity();
   Serial.print("tempHTU: "); 
   Serial.println(tempHTU);
   Serial.print("humHTU: "); 
   Serial.println(humHTU);
 
    
    voltage = analogRead(sensorPin);
   Serial.print("Voltage: "); 
   Serial.println(voltage);  
/*
    Serial.print("Publish Topic: ");
    Serial.print(outTopic1);
    Serial.print(", msgEvent:");
    sprintf(msgEvent, msgtemplate, (int)(temp*100));
    Serial.println(msgEvent);
    client.publish(outTopic1, msgEvent);

    Serial.print("Publish Topic: ");
    Serial.print(outTopic2);
    Serial.print(", msgEvent:");
    sprintf(msgEvent, msgtemplate, (int)(temp*100));
    Serial.println(msgEvent);
    client.publish(outTopic2, msgEvent);

        Serial.print("Publish Topic: ");
    Serial.print(outTopic3);
    Serial.print(", msgEvent:");
    sprintf(msgEvent, msgtemplate, (int)(temp*100));
    Serial.println(msgEvent);
    client.publish(outTopic3, msgEvent);
    */
      ESP.deepSleep( 5e6 );
}
void loop() {
   
}
