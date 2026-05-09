#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "joystick.h"

// RECEIVER MAC
uint8_t receiverAddress[] = {0x30, 0x76, 0xF5, 0x90, 0x5C, 0x4C};

// BUTTONS
#define BTN_WALK   14
#define BTN_STAND  27
#define BTN_JUMP   26

char command;

// SEND CALLBACK
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {

  Serial.print("Send Status: ");

  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Success");
  } else {
    Serial.println("Fail");
  }
}

void sendCommand(char cmd) {

  command = cmd;

  esp_err_t result = esp_now_send(
    receiverAddress,
    (uint8_t *)&command,
    sizeof(command)
  );

  if (result == ESP_OK) {
    Serial.print("Sent command: ");
    Serial.println(command);
  } else {
    Serial.println("Send Error");
  }
}

void setup() {

  Serial.begin(115200);
  joystick.begin();

  Serial.println("Joystick Ready");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); 
  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);
  Serial.println("Controller Ready");

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};

  memcpy(peerInfo.peer_addr, receiverAddress, 6);

  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to Add Peer");
    return;
  }

  Serial.println("Peer Added");
}

void loop() {
  joystick.update();

  JoyDirection dir = joystick.getDirection();

  Serial.print("X: ");
  Serial.print(joystick.getX());

  Serial.print(" | Y: ");
  Serial.print(joystick.getY());

  Serial.print(" | Direction: ");
  Serial.print(joystick.directionToString(dir));

  Serial.print(" | Button: ");
  Serial.println(joystick.getButtonState());

  // Button events
  if (joystick.isPressed()) {
    Serial.println("Button Pressed");
    sendCommand('w');
  }

  if (joystick.isReleased()) {
    Serial.println("Button Released");
    sendCommand('q');
  }

  delay(100);
}