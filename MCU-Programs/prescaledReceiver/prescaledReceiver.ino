#include <avr/power.h>
#include <SPI.h> 
#include "nRF24L01.h" 
#include "RF24.h" 
#include "LowPower.h"

#define NUM_PARKINGS 24
#define NUM_SENSORS 4
#define ROW_NUM 1

RF24 myRadio (A0, 10); 
byte addresses[][6] = {"1Node"}; 

signed char dataReceived[NUM_PARKINGS+1];
signed char dataTransmitted[NUM_PARKINGS+1];

void setup() { 
  clock_prescale_set(clock_div_16);
  Serial.begin(9600); 
  delay(1000); 

  // reset array 
  dataTransmitted[0] = ROW_NUM;
  for (int j = 1; j < NUM_PARKINGS+1; j++) {
    dataTransmitted[j] = -1;
  }
  
  Serial.println(F("RF24/Simple Receive string Test")); 
  myRadio.begin(); 
  myRadio.setDataRate(RF24_250KBPS);
  myRadio.setPALevel(RF24_PA_MIN); 
  myRadio.setChannel(108); 
  myRadio.openReadingPipe(1, addresses[0]); 
  myRadio.startListening(); 
} 

void loop() { 

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

  while(!myRadio.available()){Serial.println("Waiting for transmission");}
  if (myRadio.available()) { 
    while (myRadio.available()) { 
      myRadio.read( &dataReceived, sizeof(dataReceived)); 
    }

    Serial.println("Array received: ");
    for (int k = 0; k < NUM_PARKINGS+1; k++) {
      Serial.print(dataReceived[k]);
      Serial.print(" ");
    }
  }

  int i = 0;
  for (int j = 0; j < NUM_SENSORS; j++){
    while (dataReceived[i] < 0) {
      i++;
    }
    dataTransmitted[i] = dataReceived[i];
    i++;
  }
  
}


