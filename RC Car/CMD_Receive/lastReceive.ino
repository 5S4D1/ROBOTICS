#include <esp_now.h>
#include <WiFi.h>

// Motor pins
#define LEFT_MOTOR_FORWARD_PIN 18
#define LEFT_MOTOR_BACKWARD_PIN 19
#define RIGHT_MOTOR_FORWARD_PIN 21
#define RIGHT_MOTOR_BACKWARD_PIN 22

// Callback function when data is received
void onReceive(const esp_now_recv_info_t *macAddr, const uint8_t *data, int len) {
  char command = (char)data[0];
  Serial.print("Received command: ");
  Serial.println(command);
  handleCommand(command);
}

void setup() {
  Serial.begin(115200);

  // Initialize motor pins
  pinMode(LEFT_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_BACKWARD_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_BACKWARD_PIN, OUTPUT);

  // Initialize Wi-Fi in station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // callback function
  esp_now_register_recv_cb(onReceive);

  Serial.println("ESP-NOW Initialized. Ready to receive data.");
}

void loop() {}

void handleCommand(char command) {
  switch (command) {
    case 'w': moveForward(); break;
    case 's': moveBackward(); break;
    case 'a': turnLeft(); break;
    case 'd': turnRight(); break;
    case 'x': stopCar(); break;
    default: break;
  }
}

void moveForward() {
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
  Serial.println("Moving forward");
}

void moveBackward() {
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, HIGH);
  Serial.println("Moving backward");
}

void turnLeft() {
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
  Serial.println("Turning left");
}

void turnRight() {
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
  Serial.println("Turning right");
}

void stopCar() {
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
  Serial.println("Stopping car");
}
