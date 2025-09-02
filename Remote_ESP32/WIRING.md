# Hardware Wiring Diagrams

## Basic ESP32 Setup (WiFi Only)

```
┌─────────────────┐    ┌─────────────┐
│   Computer      │    │    ESP32    │
│                 │    │             │
│   Arduino IDE   ├────┤  USB Port   │
│   or PlatformIO │    │             │
└─────────────────┘    └─────────────┘
```

## ESP32 with GPS Module

```
┌─────────────────┐    ┌─────────────┐    ┌─────────────────┐
│   GPS Module    │    │    ESP32    │    │   Power Supply  │
│                 │    │             │    │                 │
│  VCC ───────────┼────┤ 3.3V        │    │  USB/Battery ───┤
│  GND ───────────┼────┤ GND         │    │                 │
│  TX  ───────────┼────┤ GPIO 16 (RX)│    └─────────────────┘
│  RX  ───────────┼────┤ GPIO 17 (TX)│
│                 │    │             │
└─────────────────┘    └─────────────┘
```

## Pin Assignments

| Component | ESP32 Pin | Function |
|-----------|-----------|----------|
| GPS TX | GPIO 16 | ESP32 RX (Serial2) |
| GPS RX | GPIO 17 | ESP32 TX (Serial2) |
| Battery Monitor | A0 (GPIO 36) | ADC for voltage reading |
| Status LED (optional) | GPIO 2 | Built-in LED |

## GPS Module Connections (Detailed)

```
NEO-6M/NEO-8M GPS Module:
┌─────────────────────┐
│  [●] VCC → 3.3V     │  Connect to ESP32 3.3V rail
│  [●] GND → GND      │  Connect to ESP32 ground
│  [●] TX  → GPIO 16  │  GPS transmit to ESP32 receive
│  [●] RX  → GPIO 17  │  GPS receive to ESP32 transmit
│  [●] PPS (not used) │  Pulse per second (optional)
└─────────────────────┘
```

## Power Supply Options

### Option 1: USB Power (Development)
```
Computer USB ──────── ESP32 USB Port
```

### Option 2: Battery Power (Portable)
```
3.7V LiPo Battery ──── ESP32 Battery Connector
     │
     └─── Battery Management IC (built into most ESP32 boards)
```

### Option 3: External 5V Supply
```
5V Power Supply ──────── ESP32 VIN Pin
```

## Complete Portable Setup

```
                  ┌─────────────────────────────────┐
                  │         Enclosure               │
                  │                                 │
                  │  ┌─────────────┐                │
                  │  │    ESP32    │                │
                  │  │   DevKit    │                │
                  │  └─────────────┘                │
                  │         │                       │
                  │  ┌─────────────┐                │
                  │  │ GPS Module  │                │
                  │  │   (NEO-6M)  │                │
                  │  └─────────────┘                │
                  │         │                       │
                  │  ┌─────────────┐                │
                  │  │ 3.7V LiPo   │                │
                  │  │ Battery     │                │
                  │  └─────────────┘                │
                  │                                 │
                  └─────────────────────────────────┘
                          │
                          │ Antenna extends outside
                          ▼
                   ┌─────────────┐
                   │ GPS Antenna │
                   │  (ceramic)  │
                   └─────────────┘
```

## Notes

1. **GPS Antenna**: Ensure GPS module has clear view of sky for satellite reception
2. **Power Consumption**: GPS + WiFi + ESP32 ≈ 100-200mA typical usage
3. **Battery Life**: 1000mAh battery ≈ 5-10 hours continuous operation
4. **Enclosure**: Use weatherproof case for outdoor deployment
5. **Signal Strength**: Keep WiFi antenna away from GPS antenna to avoid interference