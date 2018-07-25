#include <avr/power.h>
#include <SPI.h> 
#include "nRF24L01.h" 
#include "RF24.h" 

#define NUM_PARKINGS 24
//#define NUM_SENSORS 4
//#define ROW_NUM 1

RF24 myRadio (A0, 10); 
byte addresses[][6] = {"1Node"}; 

//String dataReceived = "";
//int dataReceived;
signed char dataReceived[NUM_PARKINGS+1];

void setup() { 
  clock_prescale_set(clock_div_16);
  Serial.begin(9600); 
  delay(1000); 
  Serial.println(F("RF24/Simple Receive string Test")); 
  myRadio.begin(); 
  myRadio.setChannel(108); 
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.openReadingPipe(1, addresses[0]); 
  myRadio.startListening(); 
} 

void loop() { 
  //dataReceived = "";
  //int len;
  
  if (myRadio.available()) { 
    
    while (myRadio.available()) { 
      myRadio.read( &dataReceived, sizeof(dataReceived)); 
    }

//    Serial.print("Data received = "); 
//    Serial.println(dataReceived);

    Serial.println("Array received: ");
    for (int k = 0; k < NUM_PARKINGS+1; k++) {
      Serial.print(dataReceived[k]);
      Serial.print(" ");
    }
  } 
}


