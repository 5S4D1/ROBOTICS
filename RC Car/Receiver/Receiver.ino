#include <esp_now.h>
#include <WiFi.h>

// Motor control pins
#define MOTOR_A1 25  // Right motor forward
#define MOTOR_A2 26  // Right motor reverse
#define MOTOR_B1 32  // Left motor forward
#define MOTOR_B2 33  // Left motor reverse

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
    Serial.print("Throttle: ");
    Serial.print(myData.throttle);
    Serial.print(" | Steering: ");
    Serial.println(myData.steering);

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

    // Right motor control
    if (rightMotorSpeed > 0) {
        digitalWrite(MOTOR_A1, HIGH);
        digitalWrite(MOTOR_A2, LOW);
    } else {
        digitalWrite(MOTOR_A1, LOW);
        digitalWrite(MOTOR_A2, HIGH);
    }

    // Left motor control
    if (leftMotorSpeed > 0) {
        digitalWrite(MOTOR_B1, HIGH);
        digitalWrite(MOTOR_B2, LOW);
    } else {
        digitalWrite(MOTOR_B1, LOW);
        digitalWrite(MOTOR_B2, HIGH);
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
    pinMode(MOTOR_A1, OUTPUT);
    pinMode(MOTOR_A2, OUTPUT);
    pinMode(MOTOR_B1, OUTPUT);
    pinMode(MOTOR_B2, OUTPUT);

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