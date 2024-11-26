#include <WiFi.h>
#include <esp_now.h>

// Define motor control pins
#define LEFT_MOTOR_FORWARD_PIN  18   // D18
#define LEFT_MOTOR_BACKWARD_PIN 19   // D19
#define RIGHT_MOTOR_FORWARD_PIN 21   // D21
#define RIGHT_MOTOR_BACKWARD_PIN 22  // D22

// Define commands
#define CMD_FORWARD "FORWARD"
#define CMD_LEFT    "LEFT"
#define CMD_RIGHT   "RIGHT"
#define CMD_STOP    "STOP"

// Function to stop all motors
void stopMotors() {
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
}

// Function to move forward
void moveForward() {
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
}

// Function to turn left
void turnLeft() {
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW); // Stop left motors
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);
}

// Function to turn right
void turnRight() {
  digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW); // Stop right motors
}

// Callback function to handle received data
void onDataReceive(const esp_now_recv_info_t *recvInfo, const uint8_t *data, int dataLen) {
  char command[dataLen + 1];
  memcpy(command, data, dataLen);
  command[dataLen] = '\0'; // Null-terminate the received command

  Serial.print("Command received: ");
  Serial.println(command);

  if (strcmp(command, CMD_FORWARD) == 0) {
    moveForward();
  } else if (strcmp(command, CMD_LEFT) == 0) {
    turnLeft();
  } else if (strcmp(command, CMD_RIGHT) == 0) {
    turnRight();
  } else if (strcmp(command, CMD_STOP) == 0) {
    stopMotors();
  } else {
    Serial.println("Unknown command");
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize motor pins as output
  pinMode(LEFT_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_BACKWARD_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_BACKWARD_PIN, OUTPUT);

  // Stop all motors initially
  stopMotors();

  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register receive callback
  esp_now_register_recv_cb(onDataReceive);
}

void loop() {
  // Do nothing, waiting for commands
}
