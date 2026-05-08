#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  
  // Wait for Serial Monitor to be opened (essential for S3 Native USB)
  while (!Serial) {
    delay(10);
  }

  Serial.println("\n--- Starting MAC Address Scan ---");

  // Set Wi-Fi to Station mode
  WiFi.mode(WIFI_STA);
  
  // Give the radio a moment to initialize
  delay(500);

  String mac = WiFi.macAddress();
  
  if (mac == "00:00:00:00:00:00" || mac == "") {
    Serial.println("Error: MAC address not ready. Retrying...");
    ESP.restart(); // Reboot if it fails to grab the ID
  } else {
    Serial.print("Receiver MAC Address: ");
    Serial.println(mac);
  }
}

void loop() {
  // Just sit here
}