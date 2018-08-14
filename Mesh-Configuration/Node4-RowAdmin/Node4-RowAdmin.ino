#include <avr/power.h>
#include <SPI.h> 
#include "RF24.h" 
#include "RF24Network.h"
#include "LowPower.h"

RF24 myRadio (A0, 10); 
RF24Network network(myRadio);
int dataTransmitted;
bool returned_results;

// Octal Mapping
const uint16_t this_node = 01;
const uint16_t gateway_node = 00;
const uint16_t node1 = 011;
const uint16_t node2 = 021;
const uint16_t node3 = 031;
const uint16_t node5 = 041;
const uint16_t node6 = 051;

void setup() {
  clock_prescale_set(clock_div_16);
  Serial.begin(9600);
  Serial.println(F("Row Admin / Sensor Node - Node4")); 
  SPI.begin();
  myRadio.begin();
  network.begin(90, this_node); //(channel, node address)
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_MIN); 
  dataTransmitted = 100; 
}
void loop() {
  returned_results = false;
  
  for(int i = 0; i < 4; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }

  while(!returned_results){
    network.update();
    //===== Receiving =====//
    while ( network.available() ) {     // Is there any incoming data?
      RF24NetworkHeader header;
      unsigned long incomingData = 0;
      network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
      Serial.print("Data received: ");
      Serial.println(incomingData);
      if (header.from_node == 00) {    // If data comes from Node 19
        RF24NetworkHeader header2(node1);     // (Address where the data is going)
        bool ok = network.write(header2, &dataTransmitted, sizeof(dataTransmitted)); // Send the data
        Serial.print("Data transmitted: ");
        Serial.println(dataTransmitted);
        dataTransmitted++;
      }
      if (header.from_node == 011) {    // If data comes from Node 1
        Serial.println(F("Data received from Node1"));
        RF24NetworkHeader header3(gateway_node);     // (Address where the data is going)
        bool ok = network.write(header3, &dataTransmitted, sizeof(dataTransmitted)); // Send the data
        Serial.print("Data transmitted: ");
        Serial.println(dataTransmitted);
        dataTransmitted++;
        returned_results = true;
      }
    }
  }
}


