#include "SIGFOX.h"
static const String device = "g88pi";
static const bool useEmulator = false;
static const bool echo = true;
static const Country country = COUNTRY_SA;
static UnaShieldV2S transceiver(country, useEmulator, device, echo);

void setup() {
  Serial.begin(9600);
  if (!transceiver.begin()) stop(F("Unable to init Sigfox module, may be missing"));
}

void loop() {
  static int successCount = 0, failCount = 0;
  int timestamp = millis();
  Serial.println("Timestamp: " + String(timestamp));

  Message msg(transceiver);
  msg.addField("timestamp", timestamp);

  unsigned long StartTime = millis();

  if (msg.send()) {
    successCount++;  //  If successful, count the message sent successfully.
  } else {
    failCount++;  //  If failed, count the message that could not be sent.
  }

  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;
  Serial.println("Transmission time: " + String(ElapsedTime));
  
  //  Show updates every 10 messages.
  Serial.print(F("Messages sent successfully: "));   
  Serial.print(successCount);
  Serial.print(F(", failed: "));  
  Serial.println(failCount);

  Serial.println(F("Waiting 15 seconds..."));
  delay(15000);
}

