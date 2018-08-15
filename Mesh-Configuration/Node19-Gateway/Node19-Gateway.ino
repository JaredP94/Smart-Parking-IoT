#include <SPI.h>  
#include "RF24.h" 
#include "RF24Network.h"

#define NUM_PARKINGS 24
#define NUM_SENSORS 4
#define NUM_ROWS 3

RF24 myRadio (2, 15); 
RF24Network network(myRadio);
int status; 
bool received_results;
signed char dataReceived[NUM_PARKINGS];
signed char parkingBayData[NUM_ROWS][NUM_PARKINGS];

// Octal Mapping
const uint16_t this_node = 00;
const uint16_t node4 = 01;
const uint16_t node10 = 02;
const uint16_t node16 = 03;

bool ack4 = false;
bool ack10 = false;
bool ack16 = false;

void setup() { 
  Serial.begin(9600); 
  Serial.println(F("Gateway - Node19")); 
  
  for (int i = 0; i < NUM_PARKINGS; i++) {
    dataReceived[i] = -1;
  }

  for (int i = 0; i < NUM_ROWS; i++) {
    for (int j = 0; j < NUM_PARKINGS; j++){
      parkingBayData[i][j] = 0;
    }
  }
  
  
  SPI.begin();
  myRadio.begin(); 
  network.begin(90, this_node);
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_MIN); 
} 

void loop() { 
  status = 1;
  received_results = false;
  
  network.update();
  //===== Sending =====//
  while(!ack4 && !ack10 && !ack16){
    RF24NetworkHeader header2(node4);     // (Address where the data is going)
    ack4 = network.write(header2, &status, sizeof(status)); // Send the data
    Serial.print("Data transmitted to Node4: ");
    Serial.println(status);
    RF24NetworkHeader header3(node10);     // (Address where the data is going)
    ack10 = network.write(header3, &status, sizeof(status)); // Send the data
    Serial.print("Data transmitted to Node10: ");
    Serial.println(status);
    RF24NetworkHeader header4(node16);     // (Address where the data is going)
    ack16 = network.write(header4, &status, sizeof(status)); // Send the data
    Serial.print("Data transmitted to Node16: ");
    Serial.println(status);
    delay(1000);
  }

  ack4 = false;
  ack10 = false;
  ack16 = false;

  while(!received_results){
    network.update();
    //===== Receiving =====//
    while ( network.available() ) {     // Is there any incoming data?
      RF24NetworkHeader header;
      network.read(header, &dataReceived, sizeof(dataReceived)); // Read the incoming data
      Serial.print("Data received from admin: ");
      if (header.from_node == 01){
        Serial.println("Array received: ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print(dataReceived[k]);
          Serial.print(" ");
        }
        Serial.println("");
        received_results = true;
      }
    }
  }

  ESP.deepSleep(32e6);
}


