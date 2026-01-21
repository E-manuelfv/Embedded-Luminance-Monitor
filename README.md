# Embedded LDR Monitor – ESP32 (Local Web Server)

## Overview
This project is an embedded system based on the ESP32 that reads ambient light levels using an LDR sensor and exposes the data through a local web interface hosted directly on the microcontroller.

The main goal of the current state is to validate sensor acquisition, Wi-Fi connectivity, and embedded web visualization in a simple, functional and inspectable way.

## Objectives
- Practice analog sensor acquisition using the ESP32 ADC
- Understand ADC scaling and signal normalization
- Implement Wi-Fi connectivity on an embedded device
- Serve a dynamic HTML dashboard directly from the ESP32
- Visualize sensor data using standard web technologies

## Hardware
- ESP32 C3 Mini
- LDR Module (Light Dependent Resistor)
- Breadboard and jumper wires

## Software Stack
- ESP32 Arduino framework
- WiFi and WebServer libraries
- HTML, CSS and JavaScript
- Bootstrap 5 for layout
- Chart.js for data visualization

## Firmware Architecture
The firmware is organized around a simple synchronous execution model:

- **Initialization phase**
  - Serial interface setup
  - ADC configuration
  - Wi-Fi connection
  - HTTP server initialization

- **Runtime phase**
  - HTTP request handling
  - On-demand sensor acquisition
  - Dynamic HTML generation with current sensor values

Sensor readings occur when the root HTTP endpoint (`/`) is accessed by a client.

## Sensor Processing
- Raw ADC values are read in the range `0–4095`
- Values are mapped to a percentage scale (`0–100%`)
- A simple rule-based classification defines light status:
  - Very Dark
  - Low Light
  - Bright
  - Very Bright

## Web Interface
The embedded web interface provides:
- Raw ADC value display
- Light level percentage
- Textual light condition status
- Visual progress bar
- Doughnut chart representing light vs darkness

The dashboard is responsive and can be accessed from any device connected to the same local network.

## Current Features
- LDR sensor reading via ESP32 ADC
- Data normalization and classification
- Wi-Fi connectivity
- Embedded HTTP server
- Dynamic HTML page generation
- Local network dashboard visualization

## Known Limitations
- Sensor data is not stored
- No timestamping of measurements
- Sensor reading and web server logic are coupled
- Page refresh required to update values
- No external API or cloud integration

## Project Status
🟢 Functional prototype – local sensor monitoring via embedded web server

## Future Modifications
- Decouple sensor acquisition from HTTP request handling
- Implement periodic sensor sampling
- Add REST API endpoint for external data consumers
- Introduce data buffering and timestamps
- Prepare firmware for cloud backend integration
- Improve code modularization and separation of concerns
