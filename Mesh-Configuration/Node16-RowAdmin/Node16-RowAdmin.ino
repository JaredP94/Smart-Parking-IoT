#include <avr/power.h>
#include <SPI.h> 
#include "RF24.h" 
#include "RF24Network.h"
#include "LowPower.h"

#define NUM_PARKINGS 24
#define NUM_SENSORS 4
#define TIMEOUT_TIME 3000
#define WAIT_TIME 10

RF24 myRadio (A0, 10);
RF24Network network(myRadio);

// Octal Mapping
const uint16_t this_node = 03;
const uint16_t gateway_node = 00;
const uint16_t node1 = 013;
const uint16_t node2 = 023;
const uint16_t node3 = 033;
const uint16_t node5 = 043;
const uint16_t node6 = 053;

bool status_received;
bool returned_results;
bool timer_running;
bool ack1;
bool ack2;
bool ack3;
bool ack5;
bool ack6;

unsigned long current_time;
unsigned long receive_duration;

// pin setup
const int trigger[NUM_SENSORS] = {2, 3, 4, 5};
const int echo[NUM_SENSORS] = {6, 7, 8, 9};

const float wait = 10;
float timeoutDist;
float timeout;

char dataTransmitted[NUM_PARKINGS];
char parkingsTracked[NUM_SENSORS] = {-1, -1, -1, -1};

void setup() {
  clock_prescale_set(clock_div_16);
  Serial.begin(9600);
  Serial.println(F("Row Admin / Sensor Node - Node16")); 
  
  SPI.begin();
  myRadio.begin();
  network.begin(90, this_node); //(channel, node address)
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_MIN); 

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(trigger[i], OUTPUT);
    pinMode(echo[i], INPUT);
  }

  pinMode(A5, OUTPUT);

  receive_duration = 0;
  timeoutDist = 200;            //100cm
  timeout = timeoutDist * 58;   //corresponding time inmicroSeconds
}

void loop() {
  for(int i = 0; i < 1; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }

  status_received = false;
  returned_results = false;
  timer_running = false;
  ack1 = false;
  ack2 = false;
  ack3 = false;
  ack5 = false;
  ack6 = false;

  // reset array 
  for (int j = 0; j < NUM_PARKINGS; j++) {
    dataTransmitted[j] = 2;
  }

  while(!returned_results){
    network.update();

    while ( network.available() && !status_received) {     // Is there any incoming data?
      RF24NetworkHeader header;
      unsigned long incomingData = 0;
      network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
      Serial.print("Data received: ");
      Serial.println(incomingData);
      
      if (header.from_node == gateway_node){
        char isOccupied[NUM_SENSORS] = {0, 0, 0, 0};
        float distance[NUM_SENSORS] = {0, 0, 0, 0};
        
        digitalWrite(A5, HIGH);
        
        for (int i = 0; i < NUM_SENSORS; i++) {
          distance[i] =  getDistance(trigger[i], echo[i]);
          isOccupied[i] = checkOccupied(distance[i]);
          Serial.print("Distance: ");
          Serial.print(distance[i]);
          Serial.print(" cm \t");
          Serial.print("status:");
          Serial.println(isOccupied[i]);
          delay(WAIT_TIME);
        }

        digitalWrite(A5, LOW);
        
        for (int i = 0; i < NUM_SENSORS; i++){
          dataTransmitted[i+12] = isOccupied[i];
        }
        
        RF24NetworkHeader header4(node1);     // (Address where the data is going)
        bool ok1 = network.write(header4, &incomingData, sizeof(incomingData)); // Send the data
        Serial.print("Data transmitted to Node1: ");
        Serial.println(incomingData);

        RF24NetworkHeader header5(node2);     // (Address where the data is going)
        bool ok2 = network.write(header5, &incomingData, sizeof(incomingData)); // Send the data
        Serial.print("Data transmitted to Node2: ");
        Serial.println(incomingData);

        RF24NetworkHeader header6(node3);     // (Address where the data is going)
        bool ok3 = network.write(header6, &incomingData, sizeof(incomingData)); // Send the data
        Serial.print("Data transmitted to Node3: ");
        Serial.println(incomingData);

        RF24NetworkHeader header7(node5);     // (Address where the data is going)
        bool ok5 = network.write(header7, &incomingData, sizeof(incomingData)); // Send the data
        Serial.print("Data transmitted to Node5: ");
        Serial.println(incomingData);

        RF24NetworkHeader header8(node6);     // (Address where the data is going)
        bool ok6 = network.write(header8, &incomingData, sizeof(incomingData)); // Send the data
        Serial.print("Data transmitted to Node6: ");
        Serial.println(incomingData);

        status_received = true;
        timer_running = true;
        current_time = millis();
      }
    }
    while ( network.available() || timer_running) {     // Is there any incoming data?
      network.update();
      if (network.available()){
        RF24NetworkHeader header2;
        char isOccupied[NUM_SENSORS] = {0, 0, 0, 0};
        network.read(header2, &isOccupied, sizeof(isOccupied)); // Read the incoming data
        
        if (header2.from_node == node1) {    // If data comes from Node 1
          Serial.println(F("Data received from Node1"));
          for (int i = 0; i < NUM_SENSORS; i++){
            dataTransmitted[i] = isOccupied[i];
          }
          ack1 = true;
        }
        
        else if (header2.from_node == node2) {    // If data comes from Node 1
          Serial.println(F("Data received from Node2"));
          for (int i = 0; i < NUM_SENSORS; i++){
            dataTransmitted[i+4] = isOccupied[i];
          }
          ack2 = true;
        }
        
        else if (header2.from_node == node3) {    // If data comes from Node 1
          Serial.println(F("Data received from Node3"));
          for (int i = 0; i < NUM_SENSORS; i++){
            dataTransmitted[i+8] = isOccupied[i];
          }
          ack3 = true;
        }
  
        else if (header2.from_node == node5) {    // If data comes from Node 1
          Serial.println(F("Data received from Node5"));
          for (int i = 0; i < NUM_SENSORS; i++){
            dataTransmitted[i+16] = isOccupied[i];
          }
          ack5 = true;
        }
  
        else if (header2.from_node == node6) {    // If data comes from Node 1
          Serial.println(F("Data received from Node6"));
          for (int i = 0; i < NUM_SENSORS; i++){
            dataTransmitted[i+20] = isOccupied[i];
          }
          ack6 = true;
        }
        
        if (ack1 && ack2 && ack3 && ack5 && ack6){
          RF24NetworkHeader header3(gateway_node);     // (Address where the data is going)
          bool ok = network.write(header3, &dataTransmitted, sizeof(dataTransmitted)); // Send the data
          Serial.print("Data transmitted to gateway ");
          returned_results = true;
        }
      }
      receive_duration = millis() - current_time;
      
      if ( receive_duration > TIMEOUT_TIME ){
        Serial.println(F("Timeout"));
        bool ok = false;
        while(!ok){
          Serial.println(F("Attempt transmission to gateway"));
          RF24NetworkHeader header3(gateway_node);     // (Address where the data is going)
          ok = network.write(header3, &dataTransmitted, sizeof(dataTransmitted)); // Send the data
          delay(10);
        }
        Serial.print("Data transmitted to gateway ");
        returned_results = true;
        timer_running = false;
      }
    }
  }

  Serial.flush();
}

float getDistance(const int trigPin, const int echoPin){
  float distance, duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, timeout);
  distance = duration/58;
  return distance;
}

int checkOccupied(float distance) {
  int occupancyStatus;
  if (distance > 0 && distance <= 100){
    occupancyStatus = 1;
  } else {
    occupancyStatus = 0;
  }
  return occupancyStatus;
}

