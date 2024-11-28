#include <esp_now.h>
#include <WiFi.h>

// Motor control pins definitions
#define RIGHT_MOTOR_FORWARD_PIN 5    // Right motor forward
#define RIGHT_MOTOR_BACKWARD_PIN 18  // Right motor backward
#define LEFT_MOTOR_FORWARD_PIN 19    // Left motor forward
#define LEFT_MOTOR_BACKWARD_PIN 21   // Left motor backward
#define ENA 2   // Enable pin for left motor (PWM)
#define ENB 4   // Enable pin for right motor (PWM) 

struct MotorControl {
    int16_t throttle;
    int16_t steering;
};

MotorControl myData;

void printMacAddress() {
    WiFi.mode(WIFI_STA);
    Serial.println("MAC Address: " + WiFi.macAddress());
}

void OnDataRecv(const esp_now_recv_info_t *esp_now_info, const uint8_t *incomingData, int len) {
    // Print received data details
    Serial.println("Data Received:");
    Serial.print("Length: ");
    Serial.println(len);
    
    // Verify data size matches expected struct
    if (len != sizeof(MotorControl)) {
        Serial.println("ERROR: Received data size mismatch!");
        return;
    }

    // Copy received data
    memcpy(&myData, incomingData, sizeof(myData));
    
    // Print received values
    // Serial.print("Throttle: ");
    // Serial.print(myData.throttle);
    // Serial.print(" | Steering: ");
    // Serial.println(myData.steering);

    // Control motors
    controlMotors(myData.throttle, myData.steering);
}

void controlMotors(int16_t throttle, int16_t steering) {
    // Calculate motor speeds
    int16_t rightMotorSpeed = throttle + steering;
    int16_t leftMotorSpeed = throttle - steering;

    // Constrain values
    rightMotorSpeed = constrain(rightMotorSpeed, -255, 255);
    leftMotorSpeed = constrain(leftMotorSpeed, -255, 255);

    // Debug motor speeds
    Serial.print("Right Motor Speed: ");
    Serial.print(rightMotorSpeed);
    Serial.print(" | Left Motor Speed: ");
    Serial.println(leftMotorSpeed);

    
    if(rightMotorSpeed == -255 && leftMotorSpeed == -255){
        moveForward(255);
        // Serial.println("forward");
    }
    else if (rightMotorSpeed == 255 && leftMotorSpeed == 255) {
        moveBackward(255);
        // Serial.println("backward");
    }
    else if(rightMotorSpeed > leftMotorSpeed) {
        leftTurn(rightMotorSpeed);
        // Serial.println("left turn");
    }
    else if(rightMotorSpeed < leftMotorSpeed) {
        rightTurn(leftMotorSpeed);
        // Serial.println("right turn");
    }
    // when (rightMotorSpeed == 0 && leftMotorSpeed = 0) then motors stop
    else {
        stopMotors();
        // Serial.println("stop!!!");
    }
}

void setup() {
    // Initialize Serial with higher baud rate
    Serial.begin(115200);
    
    // Wait for Serial to be ready
    while(!Serial) { delay(10); }
    
    // Print initial debug info
    Serial.println("Receiver ESP32 Initializing...");
    printMacAddress();

    // Set motor pins as outputs
    pinMode(RIGHT_MOTOR_FORWARD_PIN, OUTPUT);
    pinMode(RIGHT_MOTOR_BACKWARD_PIN, OUTPUT);
    pinMode(LEFT_MOTOR_FORWARD_PIN, OUTPUT);
    pinMode(LEFT_MOTOR_BACKWARD_PIN, OUTPUT);

    WiFi.mode(WIFI_STA);
    
    // Initialize ESP-NOW with error checking
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization FAILED");
        return;
    }
    Serial.println("ESP-NOW initialized successfully");

    // Register receive callback
    esp_now_register_recv_cb(OnDataRecv);
    Serial.println("Receive callback registered. Waiting for data...");
}

void loop() {
    // Optional: Add periodic debug message
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > 5000) {
        Serial.println("Still waiting for ESP-NOW data...");
        lastDebugTime = millis();
    }
    delay(10);
}

// Function to move forward
void moveForward(int speed) {
    digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
    digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
    digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
    digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);

    analogWrite(ENA, speed);   // Set speed for left motor
    analogWrite(ENB, speed);   // Set speed for right motor
}

// Function to move backward
void moveBackward(int speed) {
    digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
    digitalWrite(LEFT_MOTOR_BACKWARD_PIN, HIGH);
    digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
    digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, HIGH);

    analogWrite(ENA, speed);   // Set speed for left motor
    analogWrite(ENB, speed);   // Set speed for right motor
}

// Function to turn right
void rightTurn(int speed) {
    digitalWrite(LEFT_MOTOR_FORWARD_PIN, HIGH);
    digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
    digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
    digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);

    analogWrite(ENA, speed); // Set speed for left motor
    analogWrite(ENB, 0);     // Stop right motor
}

// Function to turn left
void leftTurn(int speed) {
    digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
    digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
    digitalWrite(RIGHT_MOTOR_FORWARD_PIN, HIGH);
    digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);

    analogWrite(ENA, 0);         // Stop left motor 
    analogWrite(ENB, speed);     // Set speed for right motor
}

// Function to stop motors
void stopMotors() {
    digitalWrite(LEFT_MOTOR_FORWARD_PIN, LOW);
    digitalWrite(LEFT_MOTOR_BACKWARD_PIN, LOW);
    digitalWrite(RIGHT_MOTOR_FORWARD_PIN, LOW);
    digitalWrite(RIGHT_MOTOR_BACKWARD_PIN, LOW);

    analogWrite(ENA, 0);   // Stop left motor
    analogWrite(ENB, 0);   // Stop right motor
}