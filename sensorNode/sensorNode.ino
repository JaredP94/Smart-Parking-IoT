#include <avr/power.h>

const int trig1 = 2; 
const int trig2 = 3; 
const int trig3 = 4; 
const int trig4 = 5; 

const int echo1 = 6;
const int echo2 = 7;
const int echo3 = 8;
const int echo4 = 9;

const float wait = 0.1;

 
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
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}
 
void loop() {
 
  float distance1, distance2, distance3, distance4;
  float duration1, duration2, duration3, duration4;
  
  digitalWrite(trig1, LOW);
  delayMicroseconds(2); 
  digitalWrite(trig1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig1, LOW);
  duration1 = pulseIn(echo1, HIGH);
  distance1 = (duration1/2) / 29.1;
  Serial.print("Distance1: ");
  Serial.print(distance1);
  Serial.println(" cm");
  //delay(wait);
  
  digitalWrite(trig2, LOW);
  delayMicroseconds(2);
  digitalWrite(trig2, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trig2, LOW);
  duration2 = pulseIn(echo2, HIGH);
  distance2 = (duration2/2) / 29.1;
  Serial.print("Distance2: ");
  Serial.print(distance2);
  Serial.println(" cm");
  //delay(wait);
  
  digitalWrite(trig3, LOW);
  delayMicroseconds(2);
  digitalWrite(trig3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig3, LOW);
  duration3 = pulseIn(echo3, HIGH);
  distance3 = (duration3/2) / 29.1;
  Serial.print("Distance3: ");
  Serial.print(distance3);
  Serial.println(" cm");
  //delay(wait);

  digitalWrite(trig4, LOW);
  delayMicroseconds(2); 
  digitalWrite(trig4, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig4, LOW);
  duration4 = pulseIn(echo4, HIGH);
  distance4 = (duration4/2) / 29.1;
  Serial.print("Distance4: ");
  Serial.print(distance4);
  Serial.println(" cm\n");
  //delay(wait);
}

//float getDistance(const int pin){
//  float distance, duration;
//  duration = pulseIn(pin, HIGH);
//  distance = (duration/2) / 29.1;
//  return distance;
//}

