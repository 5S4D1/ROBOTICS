#include <WiFi.h>
#include <esp_now.h>

// Receiver MAC address
uint8_t receiverMAC[] = {0x08, 0xA6, 0xF7, 0xBD, 0x22, 0x30};

void sendCommand(const char* command) {
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)command, strlen(command));
  if (result == ESP_OK) {
    Serial.println("Command sent: " + String(command));
  } else {
    Serial.println("Error sending command");
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Example commands; replace with actual input method
  sendCommand("FORWARD");
  delay(2000);
  sendCommand("LEFT");
  delay(2000);
  sendCommand("RIGHT");
  delay(2000);
  sendCommand("STOP");
  delay(2000);
}
