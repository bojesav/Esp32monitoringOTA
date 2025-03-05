#include <Arduino.h>
#include <gpio_viewer.h>
#include "src.h"              
#include <WiFi.h>
#include <ArduinoOTA.h>

#define relay_pin  18
#define relay_pin2  17
#define relay_pin1  16
#define LED_BUILTIN 2         

GPIOViewer gpioViewer;
bool pinState = true;

void RelayTask(void *pvParameters) {
  while (1) {
    pinState = !pinState;
    digitalWrite(relay_pin2, pinState);
    log_i("Current pin state: %d", pinState);
    vTaskDelay(30000 / portTICK_PERIOD_MS); // Delay 1 menit
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println(__FILE__);

  pinMode(relay_pin, OUTPUT);
  pinMode(relay_pin1, OUTPUT);
  pinMode(relay_pin2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);   
  digitalWrite(LED_BUILTIN, LOW); 

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");

  gpioViewer.setSamplingInterval(125);
  gpioViewer.begin();

  // OTA Setup
  ArduinoOTA.setHostname("ESP32-OTA-Demo");
  ArduinoOTA.onStart([]() {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Start OTA Update");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA Update Complete!");
    digitalWrite(LED_BUILTIN, LOW);
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]\n", error);
    digitalWrite(LED_BUILTIN, LOW);
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Membuat Task FreeRTOS untuk Relay
  xTaskCreatePinnedToCore(
    RelayTask,     // Fungsi task
    "RelayTask",   // Nama task
    2048,          // Ukuran stack
    NULL,          // Parameter task
    1,             // Prioritas task
    NULL,          // Handle task
    0              // Core 0 (agar OTA tetap di Core 1)
  );
}

void loop() {
  ArduinoOTA.handle(); // Tetap jalankan OTA tanpa terganggu task relay
}
