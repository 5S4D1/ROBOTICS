#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

const char* ssid = "UIU-CAIR";
const char* password = "12345678";
const char* websocket_server = "192.168.0.237";
const int websocket_port = 3000;

WebSocketsClient webSocket;
Servo servoMotor;

// Pin definitions
const int trigPin = 5;
const int echoPin = 18;
const int servoPin = 13;
const int leftMotorPin1 = 26;
const int leftMotorPin2 = 27;
const int rightMotorPin1 = 32;
const int rightMotorPin2 = 33;

// Variables
bool isAutonomous = false;
int distance = 0;
int servoAngle = 90;

unsigned long lastDataSent = 0;
const unsigned long dataSendInterval = 100; // Send data every 100ms

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  webSocket.begin(websocket_server, websocket_port, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  servoMotor.attach(servoPin);
  servoMotor.write(servoAngle);

  Serial.println("ESP32 initialized and ready");
}

void loop() {
  webSocket.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - lastDataSent >= dataSendInterval) {
    lastDataSent = currentMillis;
    distance = measureDistance();
    sendSensorData();
  }

  if (isAutonomous) {
    autonomousMode();
  } else {
    // In manual mode, keep servo at 90 degrees
    servoMotor.write(90);
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      break;
    case WStype_TEXT:
      {
        String message = String((char*)payload);
        Serial.println("Received message: " + message);
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, message);
        
        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
          return;
        }
        
        if (doc.containsKey("command")) {
          String command = doc["command"];
          handleCommand(command);
        }
      }
      break;
  }
}

void handleCommand(String command) {
  Serial.println("Handling command: " + command);
  if (command == "AUTONOMOUS") {
    isAutonomous = true;
    Serial.println("Switching to autonomous mode");
  } else if (command == "MANUAL") {
    isAutonomous = false;
    stopMotors();
    Serial.println("Switching to manual mode");
  } else if (!isAutonomous) {
    if (command == "FORWARD") moveForward();
    else if (command == "BACKWARD") moveBackward();
    else if (command == "LEFT") turnLeft();
    else if (command == "RIGHT") turnRight();
    else if (command == "STOP") stopMotors();
  }
}

void autonomousMode() {
  int leftDistance, rightDistance;

  if (distance < 20) {
    stopMotors();
    
    // Check left side
    servoMotor.write(0);
    delay(500);
    leftDistance = measureDistance();

    // Check right side
    servoMotor.write(180);
    delay(500);
    rightDistance = measureDistance();

    // Reset servo to center
    servoMotor.write(90);

    if (leftDistance > rightDistance && leftDistance > 20) {
      turnLeft();
      delay(500);
    } else if (rightDistance > leftDistance && rightDistance > 20) {
      turnRight();
      delay(500);
    } else {
      // If both sides have obstacles, move backward for 2 seconds
      moveBackward();
      delay(2000);
    }
  } else {
    moveForward();
  }
}

int measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

void sendSensorData() {
  DynamicJsonDocument doc(1024);
  doc["distance"] = distance;
  doc["mode"] = isAutonomous ? "AUTONOMOUS" : "MANUAL";
  
  String json;
  serializeJson(doc, json);
  
  webSocket.sendTXT(json);
  Serial.println("Sent sensor data: " + json);
}

void moveForward() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
  Serial.println("Moving forward");
}

void moveBackward() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
  Serial.println("Moving backward");
}

void turnLeft() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
  Serial.println("Turning left");
}

void turnRight() {
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
  Serial.println("Turning right");
}

void stopMotors() {
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  Serial.println("Stopping motors");
}