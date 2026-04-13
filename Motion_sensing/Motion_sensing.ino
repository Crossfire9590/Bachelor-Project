/**
 * Flex sensor test - Averaged over 100ms
 */
#include "WiFi.h"
#include "esp_now.h"

#define LITTLE 5 
#define RING 6
#define MIDDLE 7 
#define INDEX 9 
#define THUMB 10 

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
}

void loop() {
  unsigned long timestamp = millis();
  unsigned long startTime = millis();
  unsigned long littlef = 0;
  unsigned long ringf = 0;
  unsigned long middlef = 0;
  unsigned long indexf = 0;
  unsigned long thumbf = 0;
  unsigned long count = 0;

  // Collect samples for 100 milliseconds
  while (millis() - startTime < 100) {
    littlef += analogRead(LITTLE);
    ringf += analogRead(RING);
    middlef += analogRead(MIDDLE);
    indexf += analogRead(INDEX);
    thumbf += analogRead(THUMB);
    count++;
  }

  // Calculate the average raw value
  float littlef_avg = (float)littlef / count;
  float ringf_avg = (float)ringf / count;
  float middlef_avg = (float)middlef / count;
  float indexf_avg = (float)indexf / count;
  float thumbf_avg = (float)thumbf / count;


  

  Serial.print(timestamp);
  Serial.print(",");
  Serial.print(littlef_avg);
  Serial.print(",");
  Serial.print(ringf_avg);
  Serial.print(",");
  Serial.print(middlef_avg);
  Serial.print(",");
  Serial.print(indexf_avg);
  Serial.print(",");
  Serial.println(thumbf_avg);
}
