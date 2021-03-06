#include <SPI.h> 
#include "nRF24L01.h" 
#include "RF24.h" 

RF24 myRadio (2, 15); 
byte addresses[][6] = {"1Node"}; 
int dataReceived; 

void setup() { 
  Serial.begin(9600); 
  delay(1000); 
  Serial.println(F("RF24/Simple Receive data Test")); 
  myRadio.begin(); 
  myRadio.setChannel(108); 
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.openReadingPipe(1, addresses[0]); 
  myRadio.startListening(); 
} 

void loop() { 
  if (myRadio.available()) { 
    while (myRadio.available()) { 
      myRadio.read( &dataReceived, sizeof(dataReceived) ); 
    } Serial.print("Data received = "); 
    Serial.println(dataReceived); 
  } 
}


