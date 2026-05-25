
#include <esp_now.h>
#include "WiFi.h"

#define PIN_LIT 7
#define PIN_RNG 6
#define PIN_MID 5
#define PIN_IDX 4
#define PIN_THM 3

const int servoPins[5] = {PIN_LIT, PIN_RNG, PIN_MID, PIN_IDX, PIN_THM};

// --- PWM CONFIGURATION FOR SERVOS ---
const uint32_t PWM_FREQ = 50;       // 50Hz for our servos
const uint8_t  PWM_RES  = 12;       // 12-bit resolution (0 to 4095 steps)

// 50Hz means a 20,000 microsecond period. 
// 500us min pulse / 20,000us * 4096 steps = ~102 (0 Degrees)
// 2400us max pulse / 20,000us * 4096 steps = ~492 (180 Degrees)
#define SERVO_MIN_DUTY 102  // Equal to 500 microseconds (0 Degrees)
#define SERVO_MAX_DUTY 492  // Equal to 2400 microseconds (180 Degrees)

// Pre-calculate the 90-degree duty cycle mark for the thumb halfway point
// Formula: 102 + (90 / 180.0) * (492 - 102) = 102 + 195 = 297
#define SERVO_90_DEG_DUTY 297

typedef struct struct_message {
  int fingers[5]; // [0]=Little, [1]=Ring, [2]=Middle, [3]=Index, [4]=Thumb
} struct_message;

struct_message incomingData;

// ESP-NOW Receive Callback
void OnDataRecv(const esp_now_recv_info_t * recv_info, const uint8_t *incoming, int len) {
  memcpy(&incomingData, incoming, sizeof(incomingData));
  
  for(int i = 0; i < 5; i++) {
    // 1. Grab the raw incoming 0-1000 value
    int rawValue = incomingData.fingers[i];

    // 2. Constrain it strictly between 200 and 800 to chop off the overshoot
    int constrainedValue = constrain(rawValue, 200, 800);

    int dutyCycle = 0;

    // 3. Apply separate mapping based on whether it is a finger or the thumb
    if (i == 4) {
      // Index 4 is the Thumb (PIN_THM) -> Map INVERTED from 90 to 0 Degrees
      // Glove 200 -> 90 Degrees (SERVO_90_DEG_DUTY)
      // Glove 800 -> 0 Degrees (SERVO_MIN_DUTY)
      dutyCycle = map(constrainedValue, 200, 800, SERVO_90_DEG_DUTY, SERVO_MIN_DUTY);
    } else {
      // Indices 0 to 3 are the Fingers -> Map to 0-180 Degrees
      // Glove 200 -> 0 Degrees (SERVO_MIN_DUTY)
      // Glove 800 -> 180 Degrees (SERVO_MAX_DUTY)
      dutyCycle = map(constrainedValue, 200, 800, SERVO_MIN_DUTY, SERVO_MAX_DUTY);
    }
    
    // Write directly to the pin
    ledcWrite(servoPins[i], dutyCycle);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Initialize the native ESP32 LEDC PWM
  for(int i = 0; i < 5; i++) {
    ledcAttach(servoPins[i], PWM_FREQ, PWM_RES);
  }

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Native ESP32-S3 Servo Receiver Ready!");
}

void loop() {
  // Driven entirely by the hardware callback
}