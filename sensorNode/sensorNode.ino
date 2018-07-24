#include <avr/power.h>
#include <SPI.h> 
#include "nRF24L01.h" 
#include "RF24.h" 

const int numSensors = 4;
const int trigger[numSensors] = {2, 3, 4, 5};
const int echo[numSensors] = {6, 7, 8, 9};

const float wait = 100;
float timeoutDist;
float timeout;

String bayInfo;
String delimiter = "&";

RF24 myRadio (A0, 10); 
byte addresses[][6] = {"1Node"}; 

String dataTransmitted;

String bayName[numSensors] = {"bay001", "bay002", "bay003", "bay004"};
int isOccupied[numSensors] = {0, 0, 0, 0};

void setup() {
  clock_prescale_set(clock_div_16);
  Serial.begin (9600);
  
  pinMode(trigger[0], OUTPUT);
  pinMode(trigger[1], OUTPUT);
  pinMode(trigger[2], OUTPUT);
  pinMode(trigger[3], OUTPUT);
  pinMode(echo[0], INPUT);
  pinMode(echo[1], INPUT);
  pinMode(echo[2], INPUT);
  pinMode(echo[3], INPUT);

  timeoutDist = 100;            //100cm
  timeout = timeoutDist * 58;   //corresponding time inmicroSeconds

  Serial.println(F("Parking Data Transmit Test")); 
  //dataTransmitted = 100; 
  myRadio.begin(); 
  myRadio.setChannel(108); 
  myRadio.setPALevel(RF24_PA_MIN); 
  myRadio.openWritingPipe( addresses[0]); 
  
  delay(1000); 
}
 
void loop() {

  dataTransmitted = "";
  float distance[numSensors] = {0, 0, 0, 0};

  for (int i = 0; i < numSensors; i++) {
    distance[i] =  getDistance(trigger[i], echo[i]);
    isOccupied[i] = checkOccupied(distance[i]);
    Serial.print("Distance1: ");
    Serial.print(distance[i]);
    Serial.print(" cm \t");
    Serial.print("status:");
    Serial.println(isOccupied[i]);
    delay(wait);
  }

  for (int j = 0; j < numSensors; j++) {
    dataTransmitted += (bayName[j] + " " + isOccupied[j] + delimiter);
  }

  Serial.print("String for transmission: ");
  Serial.println (dataTransmitted);
  myRadio.write( &dataTransmitted, sizeof(dataTransmitted) );
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



