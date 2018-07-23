#include <avr/power.h>
#include <SPI.h> 
#include "nRF24L01.h" 
#include "RF24.h" 

RF24 myRadio (A0, 10); 
byte addresses[][6] = {"1Node"}; 
int dataTransmitted; 

void setup() { 
  clock_prescale_set(clock_div_16);
  Serial.begin(9600); 
  delay(1000); 
  Serial.println(F("RF24/Simple Transmit data Test")); 
  dataTransmitted = 100; 
  myRadio.begin(); 
  myRadio.setChannel(108); 
  myRadio.setPALevel(RF24_PA_MIN); 
  myRadio.openWritingPipe( addresses[0]); 
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(A1, OUTPUT);
  digitalWrite(A1, HIGH);
  
  delay(1000); 
} 

void loop() { 
  digitalWrite(LED_BUILTIN, LOW);
  myRadio.write( &dataTransmitted, sizeof(dataTransmitted) ); 
  //Serial.print(F("Data Transmitted = ")); 
  //Serial.print(dataTransmitted); 
  //Serial.println(F(" No Acknowledge expected")); 
  dataTransmitted = dataTransmitted + 1; 
  delay(5000); 
}


