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
const uint16_t this_node = 013;
const uint16_t row_admin = 03;

// pin setup
const int trigger[NUM_SENSORS] = {2, 4, 6, 8};
const int echo[NUM_SENSORS] = {3, 5, 7, 9};

float timeoutDist;
float timeout;

unsigned long total_on_time;
unsigned long temp_rf_transmit_time;
unsigned long total_transmit_time;

void setup() {
  clock_prescale_set(clock_div_16);
  Serial.begin(9600);
  Serial.println(F("Sensor Node - Node13")); 
  
  SPI.begin();
  myRadio.begin();
  network.begin(90, this_node); //(channel, node address)
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_HIGH); 

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(trigger[i], OUTPUT);
    pinMode(echo[i], INPUT);
  }

  pinMode(A5, OUTPUT);

  timeoutDist = 200;            //200cm
  timeout = timeoutDist * 58;   //corresponding time inmicroSecond
}
void loop() {
  for(int i = 0; i < 3; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);

  Serial.print("on");
  total_on_time = micros();

  returned_results = false;

  while(!returned_results){
    network.update();
    //===== Receiving =====//
    while ( network.available() ) {     // Is there any incoming data?
      temp_rf_transmit_time = micros();
      RF24NetworkHeader header;
      unsigned long incomingData = 0;
      network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
      temp_rf_transmit_time = micros() - temp_rf_transmit_time;
      Serial.print("Data received: ");
      Serial.println(incomingData);
      if (header.from_node == row_admin) {    // If data comes from Node 4
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
        
        bool ok = false;
        total_transmit_time = micros();
        while (!ok){
          Serial.println(F("Attempt transmission to row admin"));
          RF24NetworkHeader header2(row_admin);     // (Address where the data is going)
          ok = network.write(header2, &isOccupied, sizeof(isOccupied)); // Send the data
          delay(10);
        }
        total_transmit_time = (micros() - total_transmit_time) + temp_rf_transmit_time;
        Serial.print("Data transmitted to row admin: ");
        returned_results = true;
      }
    }
  }

  total_on_time = micros() - total_on_time;

  Serial.print("Total transmit time: ");
  Serial.println(total_transmit_time);
  Serial.print("Total on time: ");
  Serial.println(total_on_time);

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
  if (distance > 0 && distance <= timeoutDist){
    occupancyStatus = 1;
  } else {
    occupancyStatus = 0;
  }
  return occupancyStatus;
}


