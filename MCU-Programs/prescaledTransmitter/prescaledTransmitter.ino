#include <avr/power.h>
#include <SPI.h> 
#include "nRF24L01.h" 
#include "RF24.h" 

RF24 myRadio (A0, 10); 
byte addresses[][6] = {"1Node"}; 
int dataTransmitted; 
const int TRIGGER = 5;
const int ECHO = 4;
bool result;

void setup() { 
  clock_prescale_set(clock_div_16);
  Serial.begin(9600); 
  delay(1000); 
  pinMode(16,OUTPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.println(F("RF24/Simple Transmit data Test")); 
  dataTransmitted = 100; 
  myRadio.begin(); 
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_MIN); 
  myRadio.setChannel(108); 
  myRadio.openWritingPipe( addresses[0]); 
  
  delay(1000); 
} 

void loop() { 
  digitalWrite(16, HIGH);
  result = myRadio.write( &dataTransmitted, sizeof(dataTransmitted) ); 
  Serial.print(F("Data Transmitted = ")); 
  Serial.print(dataTransmitted); 
  Serial.println("Data received - no ACK payload");
  dataTransmitted = dataTransmitted + 1; 
  delay(1000);
  digitalWrite(16, LOW); 
  float duration, distance;
  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(1000);
}


