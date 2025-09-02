/*
 * Remote ESP32 for Owntracks OSM
 * 
 * This sketch enables an ESP32 to send location data to an Owntracks MQTT broker,
 * making it compatible with the Owntracks OSM smartwatch app.
 * 
 * Required Libraries:
 * - WiFi (ESP32 built-in)
 * - PubSubClient by Nick O'Leary
 * - ArduinoJson by Benoit Blanchon
 * - TinyGPS++ by Mikal Hart (if using GPS)
 * 
 * Hardware Connections:
 * - GPS Module (optional):
 *   TX -> GPIO 16
 *   RX -> GPIO 17
 * 
 * Author: Generated for OwntracksOSM
 * License: Same as parent project
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include "config.h"

// GPS Serial connection
HardwareSerial gpsSerial(2);

// WiFi and MQTT clients
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Location data
struct LocationData {
  double latitude = 0.0;
  double longitude = 0.0;
  int accuracy = 0;
  long timestamp = 0;
  int battery = 0;
};

LocationData currentLocation;
unsigned long lastLocationUpdate = 0;
unsigned long lastMQTTReconnect = 0;
bool gpsConnected = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Remote ESP32 for Owntracks OSM starting...");
  
  // Initialize GPS serial
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  
  // Connect to WiFi
  connectWiFi();
  
  // Configure MQTT
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  
  Serial.println("Setup complete");
}

void loop() {
  // Maintain WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  
  // Maintain MQTT connection
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();
  
  // Read GPS data
  readGPS();
  
  // Send location update if it's time
  if (millis() - lastLocationUpdate > (UPDATE_FREQUENCY_SECONDS * 1000)) {
    sendLocationUpdate();
    lastLocationUpdate = millis();
  }
  
  delay(100);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  if (millis() - lastMQTTReconnect < 5000) {
    return; // Don't retry too frequently
  }
  
  lastMQTTReconnect = millis();
  
  Serial.print("Connecting to MQTT broker: ");
  Serial.println(MQTT_SERVER);
  
  String clientId = String(DEVICE_ID) + "_" + String(random(0xffff), HEX);
  
  if (mqttClient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.println("MQTT connected");
    
    // Subscribe to command topic if needed
    String commandTopic = "owntracks/" + String(MQTT_USERNAME) + "/" + String(DEVICE_ID) + "/cmd";
    mqttClient.subscribe(commandTopic.c_str());
  } else {
    Serial.print("MQTT connection failed, rc=");
    Serial.println(mqttClient.state());
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages if needed
  Serial.print("Message received on topic: ");
  Serial.println(topic);
}

void readGPS() {
  // If GPS module is connected, read location data
  // For now, use simulated location data
  if (!gpsConnected) {
    // Simulate location (Canberra, Australia - same as default in main app)
    currentLocation.latitude = -35.30826;
    currentLocation.longitude = 149.12447;
    currentLocation.accuracy = 10;
    currentLocation.timestamp = millis() / 1000;
    currentLocation.battery = getBatteryLevel();
  }
  
  // TODO: Add actual GPS parsing code here if GPS module is connected
}

void sendLocationUpdate() {
  if (!mqttClient.connected()) {
    return;
  }
  
  // Create Owntracks location message
  DynamicJsonDocument doc(1024);
  doc["_type"] = "location";
  doc["lat"] = currentLocation.latitude;
  doc["lon"] = currentLocation.longitude;
  doc["acc"] = currentLocation.accuracy;
  doc["tst"] = currentLocation.timestamp;
  doc["batt"] = currentLocation.battery;
  doc["tid"] = TRACKER_ID;
  
  // Convert to string
  String payload;
  serializeJson(doc, payload);
  
  // Publish to Owntracks topic
  String topic = "owntracks/" + String(MQTT_USERNAME) + "/" + String(DEVICE_ID);
  
  bool result = mqttClient.publish(topic.c_str(), payload.c_str(), RETAIN_MESSAGES);
  
  if (result) {
    Serial.println("Location update sent:");
    Serial.println(payload);
  } else {
    Serial.println("Failed to send location update");
  }
}

int getBatteryLevel() {
  // Read battery voltage and convert to percentage
  // This is a simplified implementation
  float voltage = analogRead(A0) * 3.3 / 4095.0;
  int percentage = map(voltage * 100, 300, 420, 0, 100);
  return constrain(percentage, 0, 100);
}