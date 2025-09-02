/*
 * Diagnostic and Test Script for Remote ESP32
 * 
 * This sketch helps diagnose common issues with the Remote ESP32 setup.
 * Upload this to test individual components before using the main sketch.
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "../config.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n=== Remote ESP32 Diagnostic Test ===");
  Serial.println("Version: 1.0");
  Serial.println("Compiled: " __DATE__ " " __TIME__);
  Serial.println();
  
  // Test 1: WiFi Configuration
  testWiFiConfig();
  
  // Test 2: WiFi Connection
  testWiFiConnection();
  
  // Test 3: MQTT Configuration
  testMQTTConfig();
  
  // Test 4: MQTT Connection
  testMQTTConnection();
  
  // Test 5: JSON Message Creation
  testJSONCreation();
  
  // Test 6: GPS Serial (basic)
  testGPSSerial();
  
  Serial.println("\n=== Diagnostic Complete ===");
  Serial.println("Check results above for any issues.");
}

void loop() {
  // Keep MQTT connection alive for testing
  if (mqttClient.connected()) {
    mqttClient.loop();
  }
  delay(1000);
}

void testWiFiConfig() {
  Serial.println("Test 1: WiFi Configuration");
  Serial.print("  SSID: ");
  Serial.println(WIFI_SSID);
  
  if (String(WIFI_SSID) == "your_wifi_ssid") {
    Serial.println("  ❌ ERROR: WiFi SSID not configured!");
    Serial.println("     Edit config.h with your WiFi network name");
  } else {
    Serial.println("  ✅ WiFi SSID configured");
  }
  
  if (String(WIFI_PASSWORD) == "your_wifi_password") {
    Serial.println("  ❌ ERROR: WiFi password not configured!");
  } else {
    Serial.println("  ✅ WiFi password configured");
  }
  Serial.println();
}

void testWiFiConnection() {
  Serial.println("Test 2: WiFi Connection");
  
  if (String(WIFI_SSID) == "your_wifi_ssid") {
    Serial.println("  ⏭️  Skipped: WiFi not configured");
    Serial.println();
    return;
  }
  
  Serial.print("  Connecting to: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("  ✅ WiFi connected successfully!");
    Serial.print("  IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("  Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println();
    Serial.println("  ❌ ERROR: WiFi connection failed!");
    Serial.println("     Check SSID, password, and signal strength");
  }
  Serial.println();
}

void testMQTTConfig() {
  Serial.println("Test 3: MQTT Configuration");
  Serial.print("  Server: ");
  Serial.println(MQTT_SERVER);
  Serial.print("  Port: ");
  Serial.println(MQTT_PORT);
  Serial.print("  Username: ");
  Serial.println(MQTT_USERNAME);
  Serial.print("  Device ID: ");
  Serial.println(DEVICE_ID);
  Serial.print("  Tracker ID: ");
  Serial.println(TRACKER_ID);
  
  if (String(MQTT_SERVER) == "your.mqtt.broker.com") {
    Serial.println("  ❌ ERROR: MQTT server not configured!");
    Serial.println("     Edit config.h with your MQTT broker address");
  } else {
    Serial.println("  ✅ MQTT server configured");
  }
  
  if (String(MQTT_USERNAME) == "your_username") {
    Serial.println("  ❌ ERROR: MQTT username not configured!");
  } else {
    Serial.println("  ✅ MQTT username configured");
  }
  Serial.println();
}

void testMQTTConnection() {
  Serial.println("Test 4: MQTT Connection");
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("  ⏭️  Skipped: WiFi not connected");
    Serial.println();
    return;
  }
  
  if (String(MQTT_SERVER) == "your.mqtt.broker.com") {
    Serial.println("  ⏭️  Skipped: MQTT not configured");
    Serial.println();
    return;
  }
  
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
  Serial.print("  Connecting to MQTT broker...");
  
  String clientId = String(DEVICE_ID) + "_test_" + String(random(0xffff), HEX);
  
  if (mqttClient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.println();
    Serial.println("  ✅ MQTT connected successfully!");
    
    // Test publish
    String testTopic = "owntracks/" + String(MQTT_USERNAME) + "/" + String(DEVICE_ID) + "/test";
    if (mqttClient.publish(testTopic.c_str(), "diagnostic test")) {
      Serial.println("  ✅ Test message published");
    } else {
      Serial.println("  ❌ Failed to publish test message");
    }
  } else {
    Serial.println();
    Serial.println("  ❌ ERROR: MQTT connection failed!");
    Serial.print("     Error code: ");
    Serial.println(mqttClient.state());
    Serial.println("     Check server address, port, username, and password");
  }
  Serial.println();
}

void testJSONCreation() {
  Serial.println("Test 5: JSON Message Creation");
  
  DynamicJsonDocument doc(512);
  doc["_type"] = "location";
  doc["lat"] = -35.30826;
  doc["lon"] = 149.12447;
  doc["acc"] = 10;
  doc["tst"] = millis() / 1000;
  doc["batt"] = 95;
  doc["tid"] = TRACKER_ID;
  
  String payload;
  serializeJson(doc, payload);
  
  if (payload.length() > 0) {
    Serial.println("  ✅ JSON message created successfully");
    Serial.print("  Sample message: ");
    Serial.println(payload);
  } else {
    Serial.println("  ❌ ERROR: Failed to create JSON message");
  }
  Serial.println();
}

void testGPSSerial() {
  Serial.println("Test 6: GPS Serial Communication");
  
  HardwareSerial gpsSerial(2);
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  
  Serial.print("  GPS Serial initialized on pins ");
  Serial.print(GPS_RX_PIN);
  Serial.print(" (RX) and ");
  Serial.print(GPS_TX_PIN);
  Serial.println(" (TX)");
  
  Serial.println("  Checking for GPS data (10 second test)...");
  
  unsigned long startTime = millis();
  int bytesReceived = 0;
  
  while (millis() - startTime < 10000) { // 10 second test
    if (gpsSerial.available()) {
      char c = gpsSerial.read();
      bytesReceived++;
      // Print first few characters as sample
      if (bytesReceived <= 50) {
        Serial.print(c);
      }
    }
    delay(10);
  }
  
  Serial.println();
  if (bytesReceived > 0) {
    Serial.print("  ✅ GPS data received (");
    Serial.print(bytesReceived);
    Serial.println(" bytes)");
    Serial.println("     If you see NMEA sentences above, GPS module is working");
  } else {
    Serial.println("  ❌ No GPS data received");
    Serial.println("     Check wiring and GPS module power");
  }
  Serial.println();
}