// sender_esp32.ino
#include <esp_now.h>
#include <WiFi.h>

// Replace with your receiver ESP32's MAC address
uint8_t receiverMacAddress[] = {0x08, 0xA6, 0xF7, 0xBD, 0x22, 0x30};

// Data structure for motor controls
struct MotorControl {
    int16_t throttle;
    int16_t steering;
};

// Create a struct_message called myData
MotorControl myData;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
    Serial.begin(115200);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register peer
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    // Register callback
    esp_now_register_send_cb(OnDataSent);
}

void loop() {
    if (Serial.available() >= 4) {  // Wait for 4 bytes (2 int16_t values)
        Serial.readBytes((char*)&myData, 4);
        
        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &myData, sizeof(myData));
        
        if (result != ESP_OK) {
            Serial.println("Error sending the data");
        }
    }
}