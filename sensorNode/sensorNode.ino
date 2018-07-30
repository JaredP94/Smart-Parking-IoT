#include <avr/power.h>
#include <SPI.h> 
#include "nRF24L01.h" 
#include "RF24.h" 

#define NUM_PARKINGS 24
#define NUM_SENSORS 4
#define ROW_NUM 1

// pin setup
const int trigger[NUM_SENSORS] = {2, 3, 4, 5};
const int echo[NUM_SENSORS] = {6, 7, 8, 9};

const float wait = 10;
float timeoutDist;
float timeout;

RF24 myRadio (A0, 10); 
byte addresses[][6] = {"1Node"}; 

signed char dataTransmitted[NUM_PARKINGS+1];
const int parkingsTracked[4] = {1, 2, 3, 4};
int isOccupied[NUM_SENSORS] = {0, 0, 0, 0};

void setup() {
  clock_prescale_set(clock_div_16);
  Serial.begin (9600);

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(trigger[i], OUTPUT);
    pinMode(echo[i], INPUT);
  }

  pinMode(A5, OUTPUT);

  // reset array 
  dataTransmitted[0] = ROW_NUM;
  for (int j = 1; j < NUM_PARKINGS+1; j++) {
    dataTransmitted[j] = -1;
  }
  
  timeoutDist = 100;            //100cm
  timeout = timeoutDist * 58;   //corresponding time inmicroSeconds

  Serial.println(F("Parking Data Transmit Test")); 
  myRadio.begin(); 
  myRadio.setChannel(108); 
  myRadio.setPALevel(RF24_PA_MIN); 
  myRadio.openWritingPipe( addresses[0]); 
  
  delay(1000); 
}
 
void loop() {

  float distance[NUM_SENSORS] = {0, 0, 0, 0};
  digitalWrite(A5, HIGH);
  //delay(100);

  for (int i = 0; i < NUM_SENSORS; i++) {
    distance[i] =  getDistance(trigger[i], echo[i]);
    isOccupied[i] = checkOccupied(distance[i]);
    dataTransmitted[parkingsTracked[i]] = isOccupied[i];
    Serial.print("Distance: ");
    Serial.print(distance[i]);
    Serial.print(" cm \t");
    Serial.print("status:");
    Serial.println(isOccupied[i]);
    delay(wait);
  }
  
  digitalWrite(A5, LOW);

  Serial.println("Array for transmission: ");
  for (int k = 0; k < NUM_PARKINGS+1; k++) {
    Serial.print(dataTransmitted[k]);
    Serial.print(" ");
  }
  Serial.print("\n");
  
  myRadio.write(&dataTransmitted, sizeof(dataTransmitted));
  delay(2000);
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



