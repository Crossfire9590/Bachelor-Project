/**
 * Flex sensor test - Averaged over 100ms
 */

#define FLEX_PIN 5 

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
}

void loop() {
  unsigned long startTime = millis();
  unsigned long sum = 0;
  unsigned long count = 0;

  // Collect samples for 100 milliseconds
  while (millis() - startTime < 100) {
    sum += analogRead(FLEX_PIN);
    count++;
  }

  // Calculate the average raw value
  float averageRaw = (float)sum / count;
  
  // Convert to voltage
  // Formula: (Average / Max Resolution) * Reference Voltage
  float voltage = (averageRaw / 4095.0) * 3.3;

  Serial.print("Samples: ");
  Serial.print(count);
  Serial.print(" | Average Voltage: ");
  Serial.println(voltage);
}