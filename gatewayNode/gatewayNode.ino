#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <SPI.h>
#include <PubSubClient.h>
#include "credentials.h"

SimpleMap<String, String>* myMap;

char ssid[] = WIFI_SSID; //  Change this to your network SSID (name).
char pass[] = WIFI_PASSWORD;  // Change this your network password
char mqttUserName[] = "GatewayNode";  // Can be any name.
char mqttPass[] = "YYC34E16RCBXMVW4";  // Change this your MQTT API Key from Account > MyProfile.
char writeAPIKey[] = "Y7F65B7CI35LQNJ3";    // Change to your channel Write API Key.
long channelID = 542645;
char* topic ="channels/542645/publish/Y7F65B7CI35LQNJ3";
char* server = "mqtt.thingspeak.com";

WiFiClient wifiClient;
PubSubClient client(server, 1883, wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void setup() {
  Serial.begin(9600);

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Connection strength: ");
  Serial.println(WiFi.RSSI());

  bool ret = Ping.ping("www.google.com");
  Serial.println("Pinging Google:");
  Serial.println(ret);

String clientName="ESP-Thingspeak";
  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);
  
  if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic");
    Serial.println(topic);
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
}

void loop() {
  static int counter = 0;
  String payload="field1=";
  payload+=WiFi.RSSI();
  payload+="&status=MQTTPUBLISH";
  
  if (client.connected()){
    Serial.print("Sending payload: ");
    Serial.println(payload);
    
    if (client.publish(topic, (char*) payload.c_str())) {
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
    }
  }
  ++counter;
  delay(15000);
}
