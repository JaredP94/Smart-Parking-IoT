#include <avr/power.h>
#include <SPI.h> 
#include "RF24.h" 
#include "RF24Network.h"

RF24 myRadio (A0, 10); 
RF24Network network(myRadio);
int dataTransmitted;

// Octal Mapping
const uint16_t this_node = 01;
const uint16_t gateway_node = 00;
const uint16_t node1 = 010;
const uint16_t node2 = 011;
const uint16_t node3 = 012;
const uint16_t node5 = 013;
const uint16_t node6 = 014;

void setup() {
  clock_prescale_set(clock_div_16);
  Serial.begin(9600);
  Serial.println(F("Row Admin - Node4")); 
  SPI.begin();
  myRadio.begin();
  network.begin(90, this_node); //(channel, node address)
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_MIN); 
  dataTransmitted = 100; 
}
void loop() {
  network.update();
  //===== Receiving =====//
  while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    unsigned long incomingData = 0;
    network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
    Serial.print("Data received: ");
    Serial.println(incomingData);
    delay(1000);
    if (header.from_node == 00) {    // If data comes from Node 19
      RF24NetworkHeader header2(gateway_node);     // (Address where the data is going)
      bool ok = network.write(header2, &dataTransmitted, sizeof(dataTransmitted)); // Send the data
      Serial.print("Data transmitted: ");
      Serial.println(dataTransmitted);
      dataTransmitted++;
      delay(1000);
    }
  }
}


