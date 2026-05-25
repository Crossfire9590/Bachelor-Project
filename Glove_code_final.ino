
#include "WiFi.h"
#include <esp_now.h>

#define LITTLE 5 
#define RING 6
#define MIDDLE 7 
#define INDEX 9 
#define THUMB 10 

// Structure to match the receiver
typedef struct struct_message {
  int fingers[5]; 
} struct_message;

struct_message myData;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

int calibMin[5] = {4095, 4095, 4095, 4095, 4095};
int calibMax[5] = {0, 0, 0, 0, 0};
const int pins[5] = {LITTLE, RING, MIDDLE, INDEX, THUMB};

esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  analogReadResolution(12);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // --- STABLE CALIBRATION SECTION ---
  Serial.println("--- STARTING STABLE CALIBRATION (10s) ---");
  Serial.println("Action: Move all fingers slowly through full range!");

  unsigned long calibStart = millis();
  while (millis() - calibStart < 10000) {
    unsigned long sampleStart = millis();
    unsigned long totals[5] = {0, 0, 0, 0, 0};
    unsigned long count = 0;

    // Inner loop: Average for 50ms to get a stable "snapshot"
    while (millis() - sampleStart < 50) {
      for (int i = 0; i < 5; i++) {
        totals[i] += analogRead(pins[i]);
      }
      count++;
    }

    // Update Min/Max using the averaged snapshot
    for (int i = 0; i < 5; i++) {
      int avg = (int)(totals[i] / count);
      if (avg < calibMin[i]) calibMin[i] = avg;
      if (avg > calibMax[i]) calibMax[i] = avg;
    }

    // Visual feedback every second
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 1000) {
      Serial.printf("Calibrating... %lu s left\n", 10 - (millis() - calibStart) / 1000);
      lastMsg = millis();
    }
  }
  Serial.println("--- CALIBRATION COMPLETE ---");
}

void loop() {
  unsigned long startTime = millis();
  unsigned long totals[5] = {0, 0, 0, 0, 0};
  unsigned long count = 0;

  // Standard 50ms averaging for transmission
  while (millis() - startTime < 50) {
    for (int i = 0; i < 5; i++) {
      totals[i] += analogRead(pins[i]);
    }
    count++;
  }

  for (int i = 0; i < 5; i++) {
    float avg = (float)totals[i] / count;
    
    // Convert to 0-1000 (Mapping high ADC to 0 and low ADC to 1000)
    int norm = map(avg, calibMin[i], calibMax[i], 0, 1000); 
    myData.fingers[i] = constrain(norm, 0, 1000);
  }

  // Send data
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  // Debug output
  for(int i=0; i<5; i++) {
    Serial.print(myData.fingers[i]);
    Serial.print(i == 4 ? "" : ",");
  }
  Serial.println();
}
