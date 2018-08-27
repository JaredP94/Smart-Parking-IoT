#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "credentials.h"

#define NUM_PARKINGS 1
#define NUM_SENSORS 1
#define WAIT_TIME 10

char ssid[] = WIFI_SSID; //  Change this to your network SSID (name).
char pass[] = WIFI_PASSWORD;  // Change this your network password
char mqttUserName[] = "GatewayNode";  // Can be any name.
char mqttPass[] = MQTT_PASSWORD;  // Change this your MQTT API Key from Account > MyProfile.
char writeAPIKey[] = WRITE_API_KEY;    // Change to your channel Write API Key.
long channelID = CHANNEL_ID;
char* topic = PUBLISH_TOPIC;
char* server = "mqtt.thingspeak.com";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

unsigned long current_time;
unsigned long upload_duration;

// pin setup
const int TRIGGER = 16;
const int ECHO = 5;
const int MOSFET = 0;

const float wait = 10;
float timeoutDist;
float timeout;

void setup() { 
  Serial.begin(9600); 
  Serial.println(F("MultiDemoNode - Node0")); 

  timeoutDist = 100;
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(MOSFET, OUTPUT);

  WiFi.begin(ssid, pass);
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

  String clientName="ESP-Thingspeak";
  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);

  client.setServer(server, 1883);   // Set the MQTT broker details.
  if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic");
    Serial.println(topic);
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will need to attempt reconnection");
  }
} 

void loop() { 
  if (!client.connected()) 
  {
    reconnect();
  }

  char isOccupied = 0;
  float distance = 0;
  float duration;
  
  digitalWrite(MOSFET, HIGH);
  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = duration / 58;

  digitalWrite(MOSFET, LOW);

  isOccupied = checkOccupied(distance);
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  client.loop();   // Call the loop continuously to establish connection to the server.

  String occupied = "";
  occupied += (int)isOccupied;
  
  String payload="field1=";
  payload+=WiFi.RSSI();
  payload+="&field2=";
  payload+=occupied;
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

  client.loop();
  delay(5000);
  client.loop();
  delay(5000);
  client.loop();
  delay(250);
  client.loop();

  Serial.flush();
}

void reconnect()
{
  char clientID[] = "00000000"; // null terminated 8 chars long
  
  // Loop until reconnected.
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Generate ClientID
    for (int i = 0; i < 8; i++) {
    clientID[i] = random(65, 91); // use only letters A to Z
    }
  
    // Connect to the MQTT broker
    if (client.connect(clientID,mqttUserName,mqttPass)){
      Serial.print("Connected with Client ID: ");
      Serial.print(String(clientID));
      Serial.print(", Username: ");
      Serial.print(mqttUserName);
      Serial.print(" , Passwword: ");
      Serial.println(mqttPass);
    } 
    else{
      Serial.print("failed, rc=");
      // Print to know why the connection failed.
      // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      Serial.print(client.state());
      Serial.println(" try again in 0.5 seconds");
      delay(500);
    }
  }
}

int checkOccupied(float distance) {
  int occupancyStatus;
  if (distance > 0 && distance <= timeoutDist){
    occupancyStatus = 1;
  } else {
    occupancyStatus = 0;
  }
  return occupancyStatus;
}
