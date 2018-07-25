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

  for (int i = 0; i < numSensors; i++) {
    pinMode(trigger[i], OUTPUT);
    pinMode(echo[i], INPUT);
  }
  
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

  float distance[numSensors] = {0, 0, 0, 0};

  for (int i = 0; i < numSensors; i++) {
    distance[i] =  getDistance(trigger[i], echo[i]);
    isOccupied[i] = checkOccupied(distance[i]);
    Serial.print("Distance: ");
    Serial.print(distance[i]);
    Serial.print(" cm \t");
    Serial.print("status:");
    Serial.println(isOccupied[i]);
    delay(wait);
  }
  
  dataTransmitted = "";
  for (int j = 0; j < numSensors; j++) {
    dataTransmitted += (bayName[j] + " " + isOccupied[j] + delimiter);
  }

  Serial.println("String for transmission: ");
  Serial.println(dataTransmitted);
  Serial.println(sizeof(dataTransmitted));
  
  myRadio.write(&dataTransmitted, 6);
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



