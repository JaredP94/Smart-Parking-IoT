#include <SPI.h>  
#include "RF24.h" 
#include "RF24Network.h"

#define NUM_PARKINGS 24
#define NUM_SENSORS 4
#define NUM_ROWS 3

RF24 myRadio (2, 15);
RF24Network network(myRadio);

bool received_results;
bool incoming;
int row_number;
signed char dataReceived[NUM_PARKINGS];
signed char parkingBayData[NUM_ROWS][NUM_PARKINGS];

// Octal Mapping
const uint16_t this_node = 04;
const uint16_t gateway_node = 00;

bool row1 = false;
bool row2 = false;
bool row3 = false;


void setup() { 
  Serial.begin(9600); 
  Serial.println(F("Transmitter - Node19")); 
  
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
  incoming = false;
  received_results = false;
  row1 = false;
  row2 = false;
  row3 = false;

  while(!received_results){
    network.update();

    if (network.available() == true || network.available() == false){
      incoming = network.available();
    }
    
    if (incoming){
      Serial.println("Recv");
      RF24NetworkHeader header;
      network.read(header, &dataReceived, sizeof(dataReceived)); // Read the incoming data
      Serial.print("Data received from admin: ");
      
      if (header.from_node == gateway_node && !row1){
        memcpy(parkingBayData[0], dataReceived, NUM_PARKINGS * sizeof(char));
        
        Serial.println("Row 1 from gateway: ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print(parkingBayData[0][k]);
          Serial.print(" ");
        }
        Serial.println("");
        
        row1 = true; 
      }

      else if (header.from_node == gateway_node && !row2){
        memcpy(parkingBayData[1], dataReceived, NUM_PARKINGS * sizeof(char));
        
        Serial.println("Row 2 from gateway ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print(parkingBayData[1][k]);
          Serial.print(" ");
        }
        Serial.println("");
        
        row2 = true;
      }

      else if (header.from_node == gateway_node && !row3){
        memcpy(parkingBayData[2], dataReceived, NUM_PARKINGS * sizeof(char));
        
        Serial.println("Row 3 from gateway ");
        for (int k = 0; k < NUM_PARKINGS; k++) {
          Serial.print(parkingBayData[2][k]);
          Serial.print(" ");
        }
        Serial.println("");
        
        row3 = true;
      }

      if (row1 && row2 && row3){
        received_results = true;
      }
    }
    
    delay(0);
  }

  Serial.flush();

  ESP.deepSleep(8e6);
}
