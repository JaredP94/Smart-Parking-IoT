#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>  
#include "RF24.h" 
#include "RF24Network.h"
#include "credentials.h"

#define NUM_PARKINGS 24
#define NUM_SENSORS 4
#define NUM_ROWS 3
#define SLEEP_TIME 32e6

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

RF24 myRadio (2, 15);
RF24Network network(myRadio);

unsigned long current_time;
unsigned long upload_duration;
unsigned long total_on_time;
unsigned long total_wifi_on_time;
unsigned long total_wifi_transmit_time;

bool received_results;
bool incoming;
int row_number;
char dataReceived[NUM_PARKINGS];
char parkingBayData[NUM_ROWS][NUM_PARKINGS];

// Octal Mapping
const uint16_t this_node = 04;
const uint16_t gateway_node = 00;

bool row1 = false;
bool row2 = false;
bool row3 = false;


void setup() { 
  Serial.begin(9600); 
  Serial.println(F("Transmitter - Node19")); 

  total_on_time = micros();
  
  for (int i = 0; i < NUM_PARKINGS; i++) {
    dataReceived[i] = -1;
  }

  for (int i = 0; i < NUM_ROWS; i++) {
    for (int j = 0; j < NUM_PARKINGS; j++){
      parkingBayData[i][j] = 0;
    }
  }
  
  SPI.begin();
  myRadio.begin(); 
  network.begin(90, this_node);
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_HIGH); 

  total_wifi_on_time = micros();
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
  incoming = false;
  received_results = false;
  row1 = false;
  row2 = false;
  row3 = false;

  if (!client.connected()) 
  {
    reconnect();
  }

  client.loop();   // Call the loop continuously to establish connection to the server.

  while(!received_results){
    network.update();

    if (network.available() == true || network.available() == false){
      incoming = network.available();
    }
    
    if (incoming){
      Serial.println("Recv");
      RF24NetworkHeader header;
      network.read(header, &dataReceived, sizeof(dataReceived)); // Read the incoming data
      Serial.print("Data received from admin: ");
      
      if (header.from_node == gateway_node && !row1){
        memcpy(parkingBayData[0], dataReceived, NUM_PARKINGS * sizeof(char));
        
        Serial.println("Row 1 from gateway: ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print((int)parkingBayData[0][k]);
          Serial.print(" ");
        }
        Serial.println("");
        
        row1 = true; 
      }

      else if (header.from_node == gateway_node && !row2){
        memcpy(parkingBayData[1], dataReceived, NUM_PARKINGS * sizeof(char));
        
        Serial.println("Row 2 from gateway ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print((int)parkingBayData[1][k]);
          Serial.print(" ");
        }
        Serial.println("");
        
        row2 = true;
      }

      else if (header.from_node == gateway_node && !row3){
        memcpy(parkingBayData[2], dataReceived, NUM_PARKINGS * sizeof(char));
        
        Serial.println("Row 3 from gateway ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print((int)parkingBayData[2][k]);
          Serial.print(" ");
        }
        Serial.println("");
        
        row3 = true;
      }

      if (row1 && row2 && row3){
        received_results = true;
      }
    }
    
    delay(0);
    client.loop();
  }

  current_time = micros();

  String row1_string;
  String row2_string;
  String row3_string;
  
  for (int i = 0; i < NUM_PARKINGS; i++){
    row1_string += (int)parkingBayData[0][i];
  }

  for (int i = 0; i < NUM_PARKINGS; i++){
    row2_string += (int)parkingBayData[1][i];
  }

  for (int i = 0; i < NUM_PARKINGS; i++){
    row3_string += (int)parkingBayData[2][i];
  }
  
  Serial.println(F("Converted:"));
  Serial.println(row1_string);
  Serial.println(row2_string);
  Serial.println(row3_string);

  String payload="field1=";
  payload+=WiFi.RSSI();
  payload+="&field2=";
  payload+=row1_string;
  payload+="&field3=";
  payload+=row2_string;
  payload+="&field4=";
  payload+=row3_string;
  payload+="&status=MQTTPUBLISH";
  
  if (client.connected()){
    Serial.print("Sending payload: ");
    Serial.println(payload);
    
    total_wifi_transmit_time = micros();
    
    if (client.publish(topic, (char*) payload.c_str())) {
      Serial.println("Publish ok");
      total_wifi_transmit_time = micros() - total_wifi_transmit_time;
    }
    else {
      Serial.println("Publish failed");
    }
  }

  upload_duration = micros() - current_time;
  total_on_time = micros() - total_on_time;
  total_wifi_on_time = micros() - total_wifi_on_time;

  Serial.print("Total wifi transmit time: ");
  Serial.println(total_wifi_transmit_time);
  Serial.print("Total wifi time: ");
  Serial.println(total_wifi_on_time);
  Serial.print("Total on time: ");
  Serial.println(total_on_time);

  Serial.flush();

  client.disconnect();

  ESP.deepSleep(SLEEP_TIME - upload_duration);
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
