#include <avr/power.h>
#include <SPI.h>  
#include "RF24.h" 
#include "RF24Network.h"
#include "LowPower.h"

#define NUM_PARKINGS 24
#define NUM_SENSORS 4
#define NUM_ROWS 3
#define WAIT_TIME 10

RF24 myRadio (A0, 10);
RF24Network network(myRadio);
int status; 
bool received_results;
char dataReceived[NUM_PARKINGS];
char parkingBayData[NUM_ROWS][NUM_PARKINGS];

// Octal Mapping
const uint16_t this_node = 00;
const uint16_t node4 = 01;
const uint16_t node9 = 02;
const uint16_t node16 = 03;
const uint16_t transmitter = 04;

bool ack4 = false;
bool ack9 = false;
bool ack16 = false;

bool row1 = false;
bool row2 = false;
bool row3 = false;

// pin setup
const int trigger[NUM_SENSORS] = {2, 4, 6, 8};
const int echo[NUM_SENSORS] = {3, 5, 7, 9};

const float wait = 10;
float timeoutDist;
float timeout;

unsigned long total_on_time;
unsigned long temp_rf_transmit_time1;
unsigned long temp_rf_transmit_time2;
unsigned long total_rf_transmit_time;

void setup() { 
  clock_prescale_set(clock_div_16);
  Serial.begin(9600); 
  Serial.println(F("Gateway - Node10")); 
  
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
  myRadio.setPALevel(RF24_PA_MIN); 

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(trigger[i], OUTPUT);
    pinMode(echo[i], INPUT);
  }

  pinMode(A5, OUTPUT);

  timeoutDist = 200;            //100cm
  timeout = timeoutDist * 58;   //corresponding time inmicroSeconds
} 

void loop() { 
  for(int i = 0; i < 3; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);

  total_on_time = micros();
  
  status = 1;
  received_results = false;
  row1 = false;
  row2 = false;
  row3 = false;
  
  network.update();

  temp_rf_transmit_time1 = micros();
  
  //===== Sending =====//
  while(!ack4){
    RF24NetworkHeader header2(node4);     // (Address where the data is going)
    ack4 = network.write(header2, &status, sizeof(status)); // Send the data
    Serial.print("Data transmitted to Node4: ");
    Serial.println(status);
  }

  while(!ack9){
    RF24NetworkHeader header3(node9);     // (Address where the data is going)
    ack9 = network.write(header3, &status, sizeof(status)); // Send the data
    Serial.print("Data transmitted to Node9: ");
    Serial.println(status);
  }

  while(!ack16){
    RF24NetworkHeader header4(node16);     // (Address where the data is going)
    ack16 = network.write(header4, &status, sizeof(status)); // Send the data
    Serial.print("Data transmitted to Node16: ");
    Serial.println(status);
  }

  temp_rf_transmit_time1 = micros() - temp_rf_transmit_time1;

  ack4 = false;
  ack9 = false;
  ack16 = false;

  while(!received_results){
    network.update();

    while ( network.available() ) {     // Is there any incoming data?'
      
      temp_rf_transmit_time2 = micros();
      
      Serial.println("Recv");
      RF24NetworkHeader header;
      network.read(header, &dataReceived, sizeof(dataReceived)); // Read the incoming data
      Serial.print("Data received from admin: ");
      
      if (header.from_node == node4 && !row1){
        memcpy(parkingBayData[0], dataReceived, NUM_PARKINGS * sizeof(char));
        
        Serial.println("Array received from RowAdmin4: ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print((int)parkingBayData[0][k]);
          Serial.print(" ");
        }
        Serial.println("");
        
        row1 = true; 
      }

      else if (header.from_node == node9 && !row2){
        memcpy(parkingBayData[1], dataReceived, NUM_PARKINGS * sizeof(char));
        
        Serial.println("Array received from RowAdmin9: ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print((int)parkingBayData[1][k]);
          Serial.print(" ");
        }
        Serial.println("");
        
        row2 = true;
      }

      else if (header.from_node == node16 && !row3){
        memcpy(parkingBayData[2], dataReceived, NUM_PARKINGS * sizeof(char));
        
        Serial.println("Array received from RowAdmin16: ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print((int)parkingBayData[2][k]);
          Serial.print(" ");
        }
        Serial.println("");
        
        row3 = true;
      }

      if (row1 && row2 && row3){
        received_results = true;
        temp_rf_transmit_time2 = micros() - temp_rf_transmit_time2;
      }
    }
  }

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
    parkingBayData[1][i+12] = isOccupied[i];
  }

  Serial.println("Final row2 values: ");
  for (int k = 0; k < NUM_PARKINGS; k++) {
    Serial.print((int)parkingBayData[1][k]);
    Serial.print(" ");
  }

  RF24NetworkHeader header5(transmitter);
  total_rf_transmit_time = micros();
  for (int i = 0; i < NUM_ROWS; i++){
    bool ok = network.write(header5, &parkingBayData[i], sizeof(parkingBayData[i])); // Send the data
    Serial.print("Row Data transmitted to Transmitter: ");
    Serial.println(i);
  }

  total_rf_transmit_time = (micros() - total_rf_transmit_time) + temp_rf_transmit_time1 + temp_rf_transmit_time2;
  total_on_time = micros() - total_on_time;

  Serial.print("Total rf transmit time: ");
  Serial.println(total_rf_transmit_time);
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
