# Changelog

All notable changes to the Remote ESP32 component will be documented in this file.

## [1.0.0] - 2024-09-02

### Added
- Initial release of Remote ESP32 component for Owntracks OSM
- Basic ESP32 sketch with WiFi and MQTT connectivity
- Support for simulated location data
- Configuration system via config.h
- PlatformIO project configuration
- Arduino IDE library requirements documentation
- GPS-enhanced example sketch with TinyGPS++ integration
- Diagnostic and testing utilities
- Comprehensive documentation and setup guides
- Owntracks protocol compatibility
- Battery level monitoring
- Configurable update frequency
- TLS/SSL connection support (configuration)
- Message retention support
- Integration with existing Owntracks OSM smartwatch app

### Features
- Compatible with Owntracks JSON message format
- MQTT client with automatic reconnection
- GPS coordinate parsing and accuracy reporting
- Satellite count and altitude reporting
- Speed calculation from GPS data
- Battery percentage monitoring
- Configurable tracker ID for map display
- Serial debugging and diagnostics

### Documentation
- Complete setup and installation guide
- Hardware connection diagrams
- Troubleshooting section
- Protocol specification documentation
- Example configurations
- Library requirements

### Examples
- Basic Remote ESP32 sketch
- GPS-enhanced version with real coordinates
- Diagnostic test script for troubleshooting

### Configuration
- WiFi network settings
- MQTT broker configuration
- Device identification settings
- GPS module pin assignments
- Update frequency control
- Message retention options