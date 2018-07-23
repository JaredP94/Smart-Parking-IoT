#include <avr/power.h>
#include <string.h>

const int trig1 = 2; 
const int trig2 = 3; 
const int trig3 = 4; 
const int trig4 = 5; 
const int echo1 = 6;
const int echo2 = 7;
const int echo3 = 8;
const int echo4 = 9;

const float wait = 500;
float timeoutDist;
float timeout;

struct bayInfo {
  const char bayName[];
  int occupancyStatus;
}; 

 
void setup() {
  clock_prescale_set(clock_div_16);
  Serial.begin (9600);
  
  pinMode(trig1, OUTPUT);
  pinMode(trig2, OUTPUT);
  pinMode(trig3, OUTPUT);
  pinMode(trig4, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(echo2, INPUT);
  pinMode(echo3, INPUT);
  pinMode(echo4, INPUT);

  timeoutDist = 100; //100cm
  timeout = timeoutDist * 58; //microSeconds

  bayInfo bay1 = {"bay001", 0};
  bayInfo bay2 = {"bay002", 0};
  bayInfo bay3 = {"bay003", 0};
  bayInfo bay4 = {"bay004", 0};
}
 
void loop() {
 
  float distance1, distance2, distance3, distance4;

  distance1 = getDistance(trig1, echo1);
  bay1.occupancyStatus = checkOccupied(distance1);
  Serial.print("Distance1: ");
  Serial.print(distance1);
  Serial.print(" cm \t");
  Serial.print("status:")
  Serial.println(bay1.occupancyStatus);
  delay(wait);

  distance2 = getDistance(trig2, echo2);
  bay2.occupancyStatus = checkOccupied(distance2);
  Serial.print("Distance2: ");
  Serial.print(distance2);
  Serial.println(" cm");
  delay(wait);

  distance3 = getDistance(trig3, echo3);
  bay3.occupancyStatus = checkOccupied(distance3);
  Serial.print("Distance3: ");
  Serial.print(distance3);
  Serial.println(" cm");
  delay(wait);

  distance4 = getDistance(trig4, echo4);
  bay4.occupancyStatus = checkOccupied(distance4);
  Serial.print("Distance4: ");
  Serial.print(distance4);
  Serial.println(" cm\n");
  delay(wait);
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
  int occupanceStatus;
  if (distance < 0 && distance <= 100){
    occupanceStatus = 1;
  } else {
    occupanceStatus = 0;
  }
  return occupanceStatus;
}



