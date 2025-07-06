# Audio Spectrum Analyser
An Audio Spectrum Analyzer that visualizes the frequency and amplitude of sound using an 8×32 WS2812B LED matrix controlled by an ESP32 and analog microphone.


## Equipment Required

- ESP32-WROOM (any variant with ADC support)

- WS2812B LED matrix (32×8, vertical serpentine layout)
- Analog Microphone Module (e.g., MAX4466)
- 5V DC Power Supply with a detachable screw terminal connector
- Jumper wires
- WAGO electrical connectors (one 3-way, one 4-way)
- Wire stripper/cutters
- USB cable with data transfer support

*Optional:* A breadboard can be used for prototyping instead of WAGO connectors. However, WAGO connectors are preferred for higher current handling. All devices must share a **common ground (GND)** connection.

When connecting wires to the WAGO connectors, cut and strip the plastic coating of the wire to expose the copper, twist and insert into the connector.

##  Hardware Set-up
### ESP32 Connections
- `VIN` -> 3-way WAGO Connector
- `D2` -> LED Matrix `DATA`
- `D34` -> Microphone `OUT`
- `3.3V` -> Microphone `VCC`
- `GND` -> 4-way WAGO Connector

### Matrix Connections
- `VCC` -> 3-way WAGO Connector
- `DATA` -> ESP32 `D2`
- `GND` -> 4-way WAGO Connector

### Microphone Connections
- `VCC` -> ESP32 `3.3V`
- `OUT` -> ESP32 `D34`
- `GND` -> 4-way WAGO Connector

### Power Supply Connections
- `+5V` -> 3-way WAGO Connector
- `GND` -> 4-way WAGO Connector

## Firmware Setup

1. Open the Arduino project and fill in your Blynk and WiFi credentials:
   - `ssid[]`
   - `pass[]`
   - `auth[]`
   - `BLYNK_TEMPLATE_ID`
   - `BLYNK_TEMPLATE_NAME`

2. Upload the firmware to the ESP32 using the Arduino IDE.

3. Once uploaded, power the system on via the external 5V supply.

> Ensure the ESP32, microphone, and LED matrix share a **common ground** to avoid erratic behavior.
