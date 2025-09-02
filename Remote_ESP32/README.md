# Remote ESP32 for Owntracks OSM

This ESP32 component provides remote location tracking capabilities that integrate with the Owntracks OSM ecosystem. It allows ESP32-based devices to send location data to the same MQTT broker used by the smartwatch app.

## Features

- WiFi connectivity for MQTT communication
- GPS location tracking (when GPS module is connected)
- Compatible with Owntracks protocol
- Battery level monitoring
- Configurable update frequency
- TLS/SSL support for secure connections
- JSON message format compatible with Owntracks standard

## Hardware Requirements

- ESP32 development board (ESP32-WROOM-32 or similar)
- GPS module (optional - NEO-6M, NEO-8M, or similar)
- Battery (3.7V LiPo recommended for portable operation)
- Breadboard and jumper wires for prototyping

## Software Requirements

- Arduino IDE (1.8.19 or later) OR PlatformIO
- ESP32 board package for Arduino IDE
- Required libraries (see `libraries.txt`)

## Installation

### Arduino IDE Setup
1. Install ESP32 board package:
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install

2. Install required libraries:
   - See `libraries.txt` for complete list
   - Tools → Manage Libraries → Search and install each library

### PlatformIO Setup
1. Open project folder in PlatformIO
2. Libraries will be automatically installed from `platformio.ini`

## Configuration

1. Copy `config.h.example` to `config.h`
2. Edit `config.h` with your settings:
   ```cpp
   #define WIFI_SSID "your_wifi_network"
   #define WIFI_PASSWORD "your_wifi_password"
   #define MQTT_SERVER "your.mqtt.broker.com"
   #define MQTT_USERNAME "your_owntracks_username"
   #define MQTT_PASSWORD "your_owntracks_password"
   #define DEVICE_ID "esp32_remote"
   #define TRACKER_ID "E1"  // 2-character ID for map display
   ```

## Hardware Connections

### Basic Setup (WiFi only)
- ESP32 board
- USB cable for programming and power

### With GPS Module
```
GPS Module    ESP32
VCC      →    3.3V
GND      →    GND
TX       →    GPIO 16 (RX)
RX       →    GPIO 17 (TX)
```

### With Battery
- Connect 3.7V LiPo battery to ESP32 battery connector
- Enable battery monitoring in code if needed

## Quick Start

1. **Hardware Setup**: Connect GPS module if using
2. **Configure**: Edit `config.h` with your network and MQTT settings
3. **Upload**: Upload sketch to ESP32 via Arduino IDE or PlatformIO
4. **Monitor**: Open Serial Monitor (115200 baud) to see connection status
5. **Verify**: Check your Owntracks OSM smartwatch app for new device

## Usage

Once configured and running:
- Device automatically connects to WiFi and MQTT broker
- Sends location updates every 30 seconds (configurable)
- Appears as "esp32_remote" device in Owntracks ecosystem
- Compatible with Owntracks OSM smartwatch app and other Owntracks clients

## Integration with Owntracks OSM

This ESP32 device will appear as an additional location source in your Owntracks OSM smartwatch app. The device will:

1. Send location updates to the same MQTT broker
2. Use the same message format as the smartwatch
3. Appear with the configured Tracker ID on the map
4. Support the same features (retained messages, battery level, etc.)

## Troubleshooting

### WiFi Connection Issues
- Verify SSID and password in `config.h`
- Check WiFi signal strength
- Monitor Serial output for error messages

### MQTT Connection Issues
- Verify broker address and port
- Check username/password credentials
- Ensure broker allows websocket connections (for smartwatch compatibility)
- Test with MQTT client tools like mosquitto_pub/sub

### GPS Issues
- Verify wiring connections
- Allow time for GPS fix (can take several minutes outdoors)
- Check GPS module power requirements
- Monitor Serial output for GPS data reception

## Customization

- Modify `UPDATE_FREQUENCY_SECONDS` to change location update rate
- Add additional sensors or data to location messages
- Implement power saving modes for battery operation
- Add OTA (Over-The-Air) update capability

## Compatibility

- Compatible with Owntracks protocol specification
- Works with Owntracks OSM smartwatch app
- Compatible with official Owntracks Android/iOS apps
- Works with any MQTT broker supporting Owntracks