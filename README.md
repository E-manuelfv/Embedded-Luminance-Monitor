# Embedded Sensor Monitor – ESP32

## Overview
This project is a simple embedded system developed to practice fundamental firmware concepts using the ESP32.

The system periodically reads a digital sensor input and sends the collected data through serial output, focusing on proper timing control, code organization and hardware/software interaction.

## Objectives
- Practice embedded firmware structure
- Understand GPIO configuration and sensor reading
- Implement deterministic timing without blocking delays
- Establish a clean base for future RTOS and IoT integration

## Hardware
- ESP32 (ESP32 Mini)
- Single digital sensor or input device (e.g. button, digital output, encoder channel)

## Firmware Architecture
The firmware is organized around a main application loop responsible for:
- Hardware initialization
- Periodic sensor acquisition
- Serial logging of sensor data

Timing control is handled using FreeRTOS tick-based delays to ensure predictable execution.

## Current Features
- GPIO initialization
- Periodic sensor reading
- Serial logging via ESP-IDF logging system

## Project Status
🚧 Initial development – basic firmware structure and polling-based sensor monitoring

## Next Steps
- Refactor sensor reading using interrupt-driven approach
- Improve firmware modularization
- Introduce RTOS-based task separation
- Prepare communication layer for IoT integration
