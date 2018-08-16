#include <avr/power.h>
#include <SPI.h> 
#include "RF24.h" 
#include "RF24Network.h"
#include "LowPower.h"

#define NUM_SENSORS 4

RF24 myRadio (A0, 10); 
RF24Network network(myRadio);
int dataTransmitted;
bool returned_results;

signed char parkingsTracked[NUM_SENSORS] = {10, 11, 12, 13};
int isOccupied[NUM_SENSORS] = {0, 0, 0, 0};

// Octal Mapping
const uint16_t this_node = 031;
const uint16_t row_admin = 01;

void setup() {
  clock_prescale_set(clock_div_16);
  Serial.begin(9600);
  Serial.println(F("Sensor Node - Node3")); 
  SPI.begin();
  myRadio.begin();
  network.begin(90, this_node); //(channel, node address)
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_MIN); 
  dataTransmitted = 100; 
}
void loop() {
  for(int i = 0; i < 4; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }

  returned_results = false;

  while(!returned_results){
    network.update();
    //===== Receiving =====//
    while ( network.available() ) {     // Is there any incoming data?
      RF24NetworkHeader header;
      unsigned long incomingData = 0;
      network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
      Serial.print("Data received: ");
      Serial.println(incomingData);
      if (header.from_node == row_admin) {    // If data comes from Node 4
        RF24NetworkHeader header2(row_admin);     // (Address where the data is going)
  
        bool ok = network.write(header2, &parkingsTracked, sizeof(parkingsTracked)); // Send the data
        Serial.print("Data transmitted to row admin: ");
        //Serial.println(dataTransmitted);
        //dataTransmitted++;
        returned_results = true;
      }
    }
  }
}


