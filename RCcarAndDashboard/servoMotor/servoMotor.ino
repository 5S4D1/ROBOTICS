#include <ESP32Servo.h>

// Create a Servo object
Servo myservo;

// Define the pin the servo is connected to
const int servoPin = 18;

void setup() {
  // Attach the servo to the pin
  myservo.attach(servoPin);
}

void loop() {
  // Sweep the servo from 0 to 180 degrees
  for (int pos = 0; pos <= 180; pos++) {
    myservo.write(pos);
    delay(15);  // Wait 15ms for the servo to reach the position
  }

  // Sweep the servo from 180 to 0 degrees
  for (int pos = 180; pos >= 0; pos--) {
    myservo.write(pos);
    delay(15);  // Wait 15ms for the servo to reach the position
  }
}
