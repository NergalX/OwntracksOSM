/*
 * GPS Enhanced Remote ESP32 for Owntracks OSM
 * 
 * This example shows how to integrate a GPS module with the ESP32
 * for real location tracking instead of simulated coordinates.
 * 
 * Hardware Required:
 * - ESP32 development board
 * - GPS module (NEO-6M, NEO-8M, or similar)
 * 
 * Connections:
 * GPS VCC -> 3.3V
 * GPS GND -> GND
 * GPS TX -> GPIO 16
 * GPS RX -> GPIO 17
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include "../config.h"

// GPS objects
TinyGPSPlus gps;
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
  int satellites = 0;
  double altitude = 0.0;
  double speed = 0.0;
};

LocationData currentLocation;
unsigned long lastLocationUpdate = 0;
unsigned long lastMQTTReconnect = 0;
bool gpsHasFix = false;

void setup() {
  Serial.begin(115200);
  Serial.println("GPS Enhanced Remote ESP32 for Owntracks OSM starting...");
  
  // Initialize GPS serial
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("GPS module initialized");
  
  // Connect to WiFi
  connectWiFi();
  
  // Configure MQTT
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  
  Serial.println("Setup complete, waiting for GPS fix...");
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
  
  // Send location update if it's time and we have valid data
  if (millis() - lastLocationUpdate > (UPDATE_FREQUENCY_SECONDS * 1000)) {
    if (gpsHasFix) {
      sendLocationUpdate();
    } else {
      Serial.println("Waiting for GPS fix...");
    }
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
  Serial.print("Message received on topic: ");
  Serial.println(topic);
}

void readGPS() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        currentLocation.latitude = gps.location.lat();
        currentLocation.longitude = gps.location.lng();
        currentLocation.accuracy = gps.hdop.isValid() ? (int)(gps.hdop.hdop() * 5) : 50;
        currentLocation.timestamp = millis() / 1000;
        currentLocation.battery = getBatteryLevel();
        currentLocation.satellites = gps.satellites.isValid() ? gps.satellites.value() : 0;
        currentLocation.altitude = gps.altitude.isValid() ? gps.altitude.meters() : 0.0;
        currentLocation.speed = gps.speed.isValid() ? gps.speed.kmph() : 0.0;
        
        gpsHasFix = true;
        
        // Print GPS info periodically
        static unsigned long lastGPSInfo = 0;
        if (millis() - lastGPSInfo > 10000) { // Every 10 seconds
          Serial.printf("GPS: Lat=%.6f, Lon=%.6f, Sats=%d, HDOP=%.2f\n", 
                       currentLocation.latitude, currentLocation.longitude,
                       currentLocation.satellites, gps.hdop.hdop());
          lastGPSInfo = millis();
        }
      } else {
        gpsHasFix = false;
      }
    }
  }
  
  // Check if GPS data is stale
  if (millis() - gps.location.age() > 30000) {
    gpsHasFix = false;
  }
}

void sendLocationUpdate() {
  if (!mqttClient.connected() || !gpsHasFix) {
    return;
  }
  
  // Create enhanced Owntracks location message
  DynamicJsonDocument doc(1024);
  doc["_type"] = "location";
  doc["lat"] = currentLocation.latitude;
  doc["lon"] = currentLocation.longitude;
  doc["acc"] = currentLocation.accuracy;
  doc["tst"] = currentLocation.timestamp;
  doc["batt"] = currentLocation.battery;
  doc["tid"] = TRACKER_ID;
  
  // Add GPS-specific data
  if (currentLocation.satellites > 0) {
    doc["sat"] = currentLocation.satellites;
  }
  if (currentLocation.altitude != 0.0) {
    doc["alt"] = (int)currentLocation.altitude;
  }
  if (currentLocation.speed > 0.0) {
    doc["vel"] = (int)currentLocation.speed;
  }
  
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
  // This is a simplified implementation - adjust for your battery setup
  float voltage = analogRead(A0) * 3.3 / 4095.0;
  int percentage = map(voltage * 100, 300, 420, 0, 100);
  return constrain(percentage, 0, 100);
}