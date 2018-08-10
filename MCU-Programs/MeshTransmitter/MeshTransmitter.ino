#include <SPI.h>  
#include "RF24.h" 
#include "RF24Network.h"

RF24 myRadio (2, 15); 
RF24Network network(myRadio);
int dataTransmitted; 

const uint16_t this_node = 00;   // Address of this node in Octal format ( 04,031, etc)
const uint16_t node01 = 01;

void setup() { 
  Serial.begin(9600); 
  delay(1000); 
  pinMode(16,OUTPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.println(F("RF24/Simple Transmit data Test")); 
  dataTransmitted = 100; 
  SPI.begin();
  myRadio.begin(); 
  network.begin(90, this_node);
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_MIN); 
  delay(1000); 
} 

void loop() { 
  network.update();
  //===== Receiving =====//
  while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    unsigned long incomingData;
    network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
    Serial.print("Data received: ");
    Serial.println(incomingData);
  }
  delay(1000);
  //===== Sending =====//
  // Servo control at Node 01
  RF24NetworkHeader header2(node01);     // (Address where the data is going)
  bool ok = network.write(header2, &dataTransmitted, sizeof(dataTransmitted)); // Send the data
  Serial.print("Data transmitted: ");
  Serial.println(dataTransmitted);
  dataTransmitted++;
  delay(1000);
}


