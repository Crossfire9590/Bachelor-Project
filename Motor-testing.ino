#include <ESP32Servo.h>

Servo myServo;

// Recommended pins for ESP32: 18, 19, 21, 22, 23, 25, 26
int servoPin = 18;

void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin); // attaches the servo on pin 18
}

void loop() {
  myServo.write(0);    // 0 degrees
  delay(1000);
  
  myServo.write(25);   // 90 degrees
  delay(1000);
  
  myServo.write(25.4);  // 180 degrees
  delay(1000);
}
