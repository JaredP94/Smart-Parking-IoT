#include <SPI.h> 
#include "nRF24L01.h" 
#include "RF24.h" 

RF24 myRadio (2, 15); 
byte addresses[][6] = {"1Node"}; 
int dataTransmitted; 

void setup() { 
  Serial.begin(9600); 
  delay(1000); 
  Serial.println(F("RF24/Simple Transmit data Test")); 
  dataTransmitted = 100; 
  myRadio.begin(); 
  myRadio.setChannel(108); 
  myRadio.setPALevel(RF24_PA_MIN); 
  myRadio.openWritingPipe( addresses[0]); 
  delay(1000); 
} 

void loop() { 
  myRadio.write( &dataTransmitted, sizeof(dataTransmitted) ); 
  Serial.print(F("Data Transmitted = ")); 
  Serial.print(dataTransmitted); 
  Serial.println(F(" No Acknowledge expected")); 
  dataTransmitted = dataTransmitted + 1; 
  delay(500); 
}


