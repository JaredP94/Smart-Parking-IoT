#include <avr/power.h>
#include <SPI.h> 
#include "RF24.h" 
#include "RF24Network.h"

RF24 myRadio (A0, 10); 

RF24Network network(myRadio);      // Include the radio in the network
const uint16_t this_node = 01;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t master00 = 00;    // Address of the other node in Octal format

int dataTransmitted;

void setup() {
  clock_prescale_set(clock_div_16);
  Serial.begin(9600);
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
    unsigned long incomingData;
    network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
    Serial.print("Data received: ");
    Serial.println(incomingData);
    delay(1000);
    if (header.from_node == 0) {    // If data comes from Node 02
      RF24NetworkHeader header2(master00);     // (Address where the data is going)
      bool ok = network.write(header2, &dataTransmitted, sizeof(dataTransmitted)); // Send the data
      Serial.print("Data transmitted: ");
      Serial.println(dataTransmitted);
      dataTransmitted++;
      delay(1000);
    }
  }
}


