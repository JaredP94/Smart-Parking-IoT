#include <avr/power.h>
#include <SPI.h> 
#include "RF24.h" 
#include "RF24Network.h"
#include "LowPower.h"

#define NUM_PARKINGS 24
#define NUM_SENSORS 4
#define TIMEOUT_TIME 3000

RF24 myRadio (A0, 10);
RF24Network network(myRadio);
//int dataTransmitted;
bool status_received;
bool returned_results;
bool returned_results_node1;
bool returned_results_node2;
bool returned_results_node3;

unsigned long current_time;
unsigned long receive_duration;

// pin setup
const int trigger[NUM_SENSORS] = {2, 3, 4, 5};
const int echo[NUM_SENSORS] = {6, 7, 8, 9};

const float wait = 10;
float timeoutDist;
float timeout;

signed char dataTransmitted[NUM_PARKINGS];
signed char parkingsTracked[NUM_SENSORS] = {-1, -1, -1, -1};
int isOccupied[NUM_SENSORS] = {0, 0, 0, 0};

// Octal Mapping
const uint16_t this_node = 01;
const uint16_t gateway_node = 00;
const uint16_t node1 = 011;
const uint16_t node2 = 021;
const uint16_t node3 = 031;
const uint16_t node5 = 041;
const uint16_t node6 = 051;

void setup() {
  clock_prescale_set(clock_div_16);
  Serial.begin(9600);
  Serial.println(F("Row Admin / Sensor Node - Node4")); 

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(trigger[i], OUTPUT);
    pinMode(echo[i], INPUT);
  }

  pinMode(A5, OUTPUT);

  receive_duration = 0;
  timeoutDist = 200;            //100cm
  timeout = timeoutDist * 58;   //corresponding time inmicroSeconds
  
  SPI.begin();
  myRadio.begin();
  network.begin(90, this_node); //(channel, node address)
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_MIN); 
  //dataTransmitted = 10; 
}

void loop() {
  for(int i = 0; i < 4; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }

  status_received = false;
  returned_results = false;
  returned_results_node1 = false;
  returned_results_node2 = false;
  returned_results_node3 = false;

  // reset array 
  for (int j = 0; j < NUM_PARKINGS; j++) {
    dataTransmitted[j] = -1;
  }

  while(!returned_results){
    network.update();
    //===== Receiving =====//
    while ( network.available() && !status_received) {     // Is there any incoming data?
      RF24NetworkHeader header;
      unsigned long incomingData = 0;
      network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
      Serial.print("Data received: ");
      Serial.println(incomingData);
      if (header.from_node == 00){
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
        
        status_received = true;
        current_time = millis();
      }
    }
    while ( network.available() ) {     // Is there any incoming data?
      RF24NetworkHeader header2;
      unsigned long incomingData = 0;
      network.read(header2, &parkingsTracked, sizeof(parkingsTracked)); // Read the incoming data
      if (header2.from_node == 011) {    // If data comes from Node 1
        Serial.println(F("Data received from Node1"));
        for (int i = 0; i < NUM_SENSORS; i++){
          dataTransmitted[i] = parkingsTracked[i];
        }
        returned_results_node1 = true;
      }
      if (header2.from_node == 021) {    // If data comes from Node 1
        Serial.println(F("Data received from Node2"));
        for (int i = 0; i < NUM_SENSORS; i++){
          dataTransmitted[i+4] = parkingsTracked[i];
        }
        returned_results_node2 = true;
      }
      if (header2.from_node == 031) {    // If data comes from Node 1
        Serial.println(F("Data received from Node3"));
        for (int i = 0; i < NUM_SENSORS; i++){
          dataTransmitted[i+8] = parkingsTracked[i];
        }
        returned_results_node3 = true;
      }
      if (returned_results_node1 && returned_results_node2 && returned_results_node3){
        RF24NetworkHeader header3(gateway_node);     // (Address where the data is going)
         bool ok = network.write(header3, &dataTransmitted, sizeof(dataTransmitted)); // Send the data
        Serial.print("Data transmitted to gateway ");
        returned_results = true;
      }
      else {
        receive_duration = millis() - current_time;
        if ( receive_duration > TIMEOUT_TIME ){
          RF24NetworkHeader header3(gateway_node);     // (Address where the data is going)
          bool ok = network.write(header3, &dataTransmitted, sizeof(dataTransmitted)); // Send the data
          Serial.print("Data transmitted to gateway ");
          returned_results = true;
        }
      }
    }
  }
}

