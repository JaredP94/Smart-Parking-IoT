#include <avr/power.h>
#include <SPI.h> 
#include "RF24.h" 
#include "RF24Network.h"
#include "LowPower.h"

#define NUM_SENSORS 4
#define WAIT_TIME 10

RF24 myRadio (A0, 10); 
RF24Network network(myRadio);
bool returned_results;

// Octal Mapping
const uint16_t this_node = 051;
const uint16_t row_admin = 01;

// pin setup
const int trigger[NUM_SENSORS] = {2, 3, 4, 5};
const int echo[NUM_SENSORS] = {6, 7, 8, 9};

float timeoutDist;
float timeout;

void setup() {
  clock_prescale_set(clock_div_16);
  Serial.begin(9600);
  Serial.println(F("Sensor Node - Node1")); 
  
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

  timeoutDist = 200;            //200cm
  timeout = timeoutDist * 58;   //corresponding time inmicroSecond
}
void loop() {
  for(int i = 0; i < 4; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }

  returned_results = false;

  while(!returned_results){
    network.update();
    //===== Receiving =====//
    while ( network.available() ) {     // Is there any incoming data?
      RF24NetworkHeader header;
      unsigned long incomingData = 0;
      network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
      Serial.print("Data received: ");
      Serial.println(incomingData);
      if (header.from_node == 01) {    // If data comes from Node 4
        signed char isOccupied[NUM_SENSORS] = {0, 0, 0, 0};
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
        
        RF24NetworkHeader header2(row_admin);     // (Address where the data is going)
        bool ok = network.write(header2, &isOccupied, sizeof(isOccupied)); // Send the data
        Serial.print("Data transmitted to row admin: ");
        returned_results = true;
      }
    }
  }
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


