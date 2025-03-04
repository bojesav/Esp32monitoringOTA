#include <Arduino.h>
#include <gpio_viewer.h>
#include "src.h"              // rename the "_secrets.h" file before building
#include <WiFi.h>             // Include WiFi library for ESP32
#include <ArduinoOTA.h>       // Include ArduinoOTA library for OTA updates

#define DEMO_PIN  17

GPIOViewer gpioViewer;
bool pinState = false;

void setup() {
  delay(500);
  Serial.begin(115200);
  Serial.setDebugOutput(true);    // send ESP inbuilt log messages to Serial
  Serial.println(__FILE__);

  pinMode(DEMO_PIN, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");

  // Initialize GPIOViewer
  gpioViewer.setSamplingInterval(125);
  gpioViewer.begin();

  // Configure OTA
  ArduinoOTA.setHostname("ESP32-OTA-Demo"); // Set a unique hostname for your device
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA Update Complete!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle(); // Handle OTA updates

  pinState = !pinState;
  digitalWrite(DEMO_PIN, pinState);
  log_i("Current pin state: %d", pinState);
  Serial.println();
  delay(1000);
}