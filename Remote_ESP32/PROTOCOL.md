# Owntracks Message Format Documentation

This document describes the JSON message format used by the Remote ESP32 to communicate with Owntracks brokers and clients.

## Location Message Format

The Remote ESP32 sends location messages in the standard Owntracks JSON format:

```json
{
  "_type": "location",
  "lat": -35.30826,
  "lon": 149.12447,
  "acc": 10,
  "tst": 1693612800,
  "batt": 95,
  "tid": "E1",
  "sat": 8,
  "alt": 650,
  "vel": 25
}
```

## Required Fields

| Field | Type | Description |
|-------|------|-------------|
| `_type` | string | Always "location" for location messages |
| `lat` | number | Latitude in decimal degrees |
| `lon` | number | Longitude in decimal degrees |
| `tst` | integer | Unix timestamp (seconds since epoch) |

## Standard Optional Fields

| Field | Type | Description |
|-------|------|-------------|
| `acc` | integer | Location accuracy in meters |
| `batt` | integer | Battery level (0-100) |
| `tid` | string | Tracker ID (2 characters, displayed on map) |

## Enhanced GPS Fields

These fields are included when GPS data is available:

| Field | Type | Description |
|-------|------|-------------|
| `sat` | integer | Number of satellites used for fix |
| `alt` | integer | Altitude in meters above sea level |
| `vel` | integer | Speed in kilometers per hour |

## MQTT Topic Structure

Messages are published to topics following this pattern:
```
owntracks/{username}/{deviceid}
```

Example:
```
owntracks/myuser/esp32_remote
```

## Message Retention

When `RETAIN_MESSAGES` is enabled in config.h, location messages are retained on the MQTT broker. This means:
- Last known location is available to new subscribers
- Devices can see each other's last location when connecting
- Compatible with Owntracks OSM smartwatch behavior

## Compatibility Notes

- Message format is fully compatible with official Owntracks apps
- Works with Owntracks OSM smartwatch app
- Compatible with Owntracks Recorder and other ecosystem tools
- Follows Owntracks JSON specification v1.0

## Additional Message Types (Future)

The Owntracks protocol supports additional message types that could be implemented:

- `waypoint`: Named locations/points of interest
- `transition`: Geofence enter/exit events  
- `beacon`: iBeacon detection events
- `cmd`: Command messages for remote control

Currently, only `location` messages are implemented in the Remote ESP32.