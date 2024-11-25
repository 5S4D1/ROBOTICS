#include <esp_now.h>
#include <WiFi.h>

// Receiver ESP32 MAC address
uint8_t receiverMAC[] = {0x08, 0xA6, 0xF7, 0xBD, 0x22, 0x30};

String command = "";

// Callback when ESP-NOW data is sent
void onSent(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);

  // Initialize Wi-Fi in station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(onSent);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW Initialized. Ready to send data.");
}

void loop() {
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    if (command.length() > 0) {
      const char *msg = command.c_str();
      esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)msg, strlen(msg));

      Serial.print("Sending command: ");
      Serial.println(msg);

      if (result == ESP_OK) {
        Serial.println("Command sent successfully.");
      } else {
        Serial.println("Error sending command.");
      }
    }
  } 
}
