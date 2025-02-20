# OCEAN_Logger_Sonar_Radio
This is my bachelor thesis

# OCEAN_Logger_Sonar_Radio

## Overview
OCEAN_Logger_Sonar_Radio is an embedded system designed for ocean data logging and communication. This project utilizes an STM32 microcontroller to collect sonar data, transmits it via LoRa, and integrates with a REST API for cloud-based storage and analysis. Additionally, it incorporates an LLM (Large Language Model) for data interpretation and intelligent insights.

## Features
- **STM32 Microcontroller**: Handles data acquisition and processing from sonar sensors.
- **LoRa Communication**: Enables long-range, low-power data transmission.
- **REST API Integration**: Connects to a backend for data storage and remote access.
- **LLM Support**: Uses AI to analyze and interpret collected data.

## System Architecture
1. **Data Acquisition**: The STM32 microcontroller collects sonar data.
2. **Processing & Transmission**: Processed data is sent via LoRa to a base station.
3. **Cloud Integration**: Data is uploaded to a REST API for remote access.
4. **AI Analysis**: LLM provides insights and analysis based on the collected data.

## Technologies Used
- **Microcontroller**: STM32 (C/C++)
- **Wireless Communication**: LoRa
- **Backend**: REST API (Python, Node.js, etc.)
- **AI/ML**: Large Language Model (LLM) for data analysis

## Installation & Setup
1. Flash the STM32 firmware onto the microcontroller.
2. Configure the LoRa module for communication.
3. Set up the REST API backend.
4. Deploy the LLM-based data analysis module.

## Usage
- Deploy the system in an ocean environment.
- Monitor sonar data remotely through the REST API.
- Receive AI-driven insights for decision-making.

## Future Enhancements
- Improved AI models for enhanced data analysis.
- Additional sensors for environmental monitoring.
- Enhanced real-time processing and visualization.

## License
[Specify License]

## Contributors
- [Your Name]
- [Other Contributors]

