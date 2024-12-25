#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

const char* ssid = "Major Dalim";
const char* password = "01727093609@@";
// const char* ssid = "UIU-CAIR";
// const char* password = "12345678";

const char* websocket_server = "192.168.0.114";
const int websocket_port = 3000;

WebSocketsClient webSocket;

// Pin definitions for ultrasonic sensor
const int trigPin = 5;
const int echoPin = 18;

// Pin definitions for motor control
const int leftMotorPin1 = 26;
const int leftMotorPin2 = 27;
const int rightMotorPin1 = 32;
const int rightMotorPin2 = 33;

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize WebSocket connection
  webSocket.begin(websocket_server, websocket_port, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize motor control pins
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
}

void loop() {
  webSocket.loop();

  // Measure distance and send data every 100ms
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 100) {
    lastTime = millis();
    int distance = measureDistance();
    
    // Print distance to serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Send distance to server
    DynamicJsonDocument doc(1024);
    doc["distance"] = distance;
    
    String json;
    serializeJson(doc, json);
    
    webSocket.sendTXT(json);
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
      handleCommand((char*)payload);
      break;
  }
}

void handleCommand(String command) {
  // ... (rest of the code remains the same)
}

int measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  
  return distance;
}

// ... (motor control functions remain the same)