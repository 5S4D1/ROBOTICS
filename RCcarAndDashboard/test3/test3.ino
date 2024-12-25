#include <ESP32Servo.h>
#include <NewPing.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "UIU-CAIR";
const char* password = "12345678";

const char* websocket_server = "192.168.0.237"; // ip
const int websocket_port = 3000; // Replace with your WebSocket port

// WebSocket object
WebSocketsClient webSocket;

// Motor and sensor pins
const int LeftMotorForward = 26;
const int LeftMotorBackward = 27;
const int RightMotorForward = 32;
const int RightMotorBackward = 33;

#define trig_pin 4
#define echo_pin 5
const int servoPin = 14;

#define maximum_distance 400
boolean goesForward = false;
boolean autonomousMode = true;  // Mode toggle flag
int distance = 100;

NewPing sonar(trig_pin, echo_pin, maximum_distance);
Servo servo_motor;

void setup() {
  Serial.begin(115200);

  // Initialize motors and sensors
  pinMode(RightMotorForward, OUTPUT);
  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);
  servo_motor.attach(servoPin);
  servo_motor.write(90);
  delay(2000);

  // WiFi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // WebSocket setup
  webSocket.begin(websocket_server, websocket_port);
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  
  if (autonomousMode) {
    distance = readPing();
    Serial.print("Distance (Autonomous): ");
    Serial.println(distance);
    handleAutonomousMode();
  }
}

int readPing() {
  delay(70);
  int cm = sonar.ping_cm();
  if (cm == 0) cm = 250;
  Serial.print("Ultrasonic Data: ");
  Serial.println(cm);
  webSocket.sendTXT(String(cm));  // Send ultrasonic data
  return cm;
}

void handleAutonomousMode() {
  if (distance <= 20) {
    moveStop();
    delay(300);
    moveBackward();
    delay(400);
    moveStop();
  } else {
    moveForward();
  }
}

// WebSocket event handler
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      webSocket.sendTXT("ESP32 Connected");
      break;
    case WStype_TEXT:
      Serial.printf("Message from server: %s\n", payload);
      String message = String((char*)payload);

      if (message == "AUTONOMOUS") {
        autonomousMode = true;
      } else if (message == "MANUAL") {
        autonomousMode = false;
      } else {
        handleManualControl(message);
      }
      break;
    default:
      break;
  }
}

// Handle keyboard commands in manual mode
void handleManualControl(String command) {
  if (!autonomousMode) {
    if (command == "FORWARD") {
      moveForward();
    } else if (command == "BACKWARD") {
      moveBackward();
    } else if (command == "LEFT") {
      turnLeft();
    } else if (command == "RIGHT") {
      turnRight();
    } else if (command == "STOP") {
      moveStop();
    }
  }
}

void moveStop() {
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
  digitalWrite(LeftMotorBackward, LOW);
}

void moveForward() {
  if (!goesForward) {
    goesForward = true;
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorBackward, LOW);
  }
}

void moveBackward() {
  goesForward = false;
  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);
}

void turnRight() {
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);
  delay(500);
  moveStop();
}

void turnLeft() {
  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorForward, HIGH);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
  delay(500);
  moveStop();
}
