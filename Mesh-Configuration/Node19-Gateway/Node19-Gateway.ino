#include <avr/power.h>
#include <SPI.h>  
#include "RF24.h" 
#include "RF24Network.h"
#include "LowPower.h"

#define NUM_PARKINGS 24
#define NUM_SENSORS 4
#define NUM_ROWS 3

RF24 myRadio (A0, 10);
RF24Network network(myRadio);
int status; 
bool received_results;
signed char dataReceived[NUM_PARKINGS];
signed char parkingBayData[NUM_ROWS][NUM_PARKINGS];

// Octal Mapping
const uint16_t this_node = 00;
const uint16_t node4 = 01;
const uint16_t node16 = 02;

bool ack4 = false;
bool ack16 = false;

bool row1 = false;
bool row2 = false;
bool row3 = false;

void setup() { 
  clock_prescale_set(clock_div_16);
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
  for(int i = 0; i < 1; i++){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
  
  status = 1;
  received_results = false;
  row1 = false;
  row2 = false;
  row3 = false;
  
  network.update();
  //===== Sending =====//
  while(!ack4){
    RF24NetworkHeader header2(node4);     // (Address where the data is going)
    ack4 = network.write(header2, &status, sizeof(status)); // Send the data
    Serial.print("Data transmitted to Node4: ");
    Serial.println(status);
  }

  while(!ack16){
    RF24NetworkHeader header4(node16);     // (Address where the data is going)
    ack16 = network.write(header4, &status, sizeof(status)); // Send the data
    Serial.print("Data transmitted to Node16: ");
    Serial.println(status);
    delay(1000);
  }

  ack4 = false;
  ack16 = false;

  while(!received_results){
    network.update();
    //delay(1);
    //===== Receiving =====//
    //Serial.println("Here");
    while ( network.available() ) {     // Is there any incoming data?'
      Serial.println("Recv");
      RF24NetworkHeader header;
      network.read(header, &dataReceived, sizeof(dataReceived)); // Read the incoming data
      Serial.print("Data received from admin: ");
      
      if (header.from_node == node4 && !row1){
        memcpy(parkingBayData[0], dataReceived, NUM_PARKINGS * sizeof(char));
        
        Serial.println("Array received from RowAdmin4: ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print(parkingBayData[0][k]);
          Serial.print(" ");
        }
        Serial.println("");
        
        row1 = true; 
      }

//      else if (header.from_node == node10){
//        memcpy(parkingBayData[1], dataReceived, NUM_PARKINGS * sizeof(char));
//        
//        Serial.println("Array received from RowAdmin10: ");
//        for (int k = 0; k < NUM_PARKINGS; k++) {
//          Serial.print(parkingBayData[1][k]);
//          Serial.print(" ");
//        }
//        Serial.println("");
//        
//        row2 = true;
//      }

      else if (header.from_node == node16 && !row3){
        memcpy(parkingBayData[2], dataReceived, NUM_PARKINGS * sizeof(char));
        
        Serial.println("Array received from RowAdmin16: ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print(parkingBayData[2][k]);
          Serial.print(" ");
        }
        Serial.println("");
        
        row3 = true;
      }

      if (row1 && row3){
        received_results = true;
      }
    }
  }

  Serial.flush();
}


