# 💧 Smart Water Level Indicator

[![Arduino](https://img.shields.io/badge/Arduino-UNO%20%7C%20Nano-00979D?logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Proteus](https://img.shields.io/badge/Proteus-Simulation-1F6FEB)](https://www.labcenter.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Project Type](https://img.shields.io/badge/Project-Embedded%20Systems-orange)](https://github.com/)

## 📌 Overview

The **Smart Water Level Indicator** is an embedded systems and electronics project designed to monitor the water level in a tank and provide clear real-time feedback through LEDs, LCD output, and an optional buzzer alarm. It is built around an **Arduino-based control system** and can be validated using **Proteus Design Suite** for simulation before deploying to hardware.

This project helps with:

- Monitoring water levels in overhead or underground tanks
- Preventing overflow conditions and unnecessary water waste
- Avoiding dry-run damage in pumps caused by empty tanks
- Demonstrating an embedded automation concept for beginner to intermediate learners

## 🚀 Features

- Water level detection using sensor probes or equivalent input modules
- LED and/or LCD indication of tank status
- Buzzer alarm for low-level or overflow warnings
- Arduino-based control logic for real-time decision making
- Proteus simulation support for safe testing and validation
- Low-cost embedded solution using widely available components
- Real-time monitoring and status updates

## 🛠 Technologies Used

| Technology | Purpose |
| --- | --- |
| Arduino UNO / Nano | Main microcontroller platform |
| Embedded C / C++ | Firmware development |
| Arduino IDE | Code writing, compiling, and uploading |
| Proteus Design Suite | Circuit design and simulation |
| Water level sensors / probes | Input sensing for tank level detection |
| LEDs / Buzzer / LCD | Visual and audio output indicators |
| Serial communication | Debugging and runtime monitoring, if used |

## 🔌 Hardware Components

| Component | Description | Typical Role |
| --- | --- | --- |
| Arduino board | Arduino UNO or Nano | Reads sensor inputs and controls outputs |
| Water level sensors | Probe-type or sensor module | Detects water presence at different tank levels |
| LEDs | Green, yellow, red indicators | Shows low, medium, and high water levels |
| Buzzer | Piezo buzzer or active buzzer | Provides warning alerts |
| LCD display | 16x2 LCD or similar | Displays tank status and water percentage |
| Jumper wires | Male-to-male / male-to-female | Connects components on the breadboard |
| Breadboard | Prototyping platform | Supports temporary circuit assembly |
| Power supply | 5V source or adapter | Powers the Arduino and peripherals |
| Resistors and other components | Current limiting and signal conditioning | Protects LEDs and stabilizes the circuit |

## ⚙️ System Architecture

The system follows a simple embedded control flow:

1. **Sensor input processing**: Water level probes or sensor inputs send signals to the Arduino.
2. **Level detection logic**: The Arduino compares the sensor states to determine whether the tank is empty, low, medium, or full.
3. **Output indicators**: LEDs, LCD text, and the buzzer provide immediate feedback based on the detected level.
4. **Control flow**: The firmware continuously reads inputs in a loop, updates the display, and activates alerts when threshold conditions are met.

In a typical implementation, lower probes indicate minimum water presence, while upper probes indicate near-full or overflow conditions. The Arduino interprets these inputs and converts them into a clear user-facing status.

## 🔧 Circuit Diagram

The circuit is implemented in **Proteus Design Suite** and wired to represent the real hardware connections.

Typical wiring includes:

- Sensor outputs connected to Arduino digital or analog pins
- LEDs connected through current-limiting resistors to output pins
- Buzzer driven from a digital output pin
- LCD connected through parallel pins or an I2C module, depending on the design
- Common ground shared across the Arduino and peripherals

If your repository includes simulation assets, place them in the following locations:

- `.pdsprj` project file for the Proteus design
- Schematic files for circuit documentation
- Simulation assets such as libraries, models, or exported images

## 📂 Project Structure

Recommended repository organization:

```text
/Arduino_Code
/Proteus_Simulation
/Documentation
/Images
README.md
```

### Folder Purpose

| Folder | Purpose |
| --- | --- |
| Arduino_Code | Contains the Arduino sketch and supporting source files |
| Proteus_Simulation | Stores Proteus project files, schematics, and simulation assets |
| Documentation | Holds design notes, wiring explanations, and technical references |
| Images | Includes circuit screenshots, block diagrams, and result previews |
| README.md | Main project overview and setup guide |

## 📥 Installation & Setup

### 1. Clone the repository

```bash
git clone https://github.com/your-username/Smart-Water-Level-Indicator.git
cd Smart-Water-Level-Indicator
```

### 2. Install Arduino IDE

Download and install the latest version of the [Arduino IDE](https://www.arduino.cc/en/software).

### 3. Install required libraries

If your sketch uses an LCD or I2C module, install the relevant libraries from the Arduino Library Manager.

### 4. Open the Arduino sketch

Open the main `.ino` file from the `Arduino_Code` folder in Arduino IDE.

### 5. Connect the hardware

Wire the Arduino board, sensors, LEDs, buzzer, and LCD according to the circuit diagram.

### 6. Upload the code

Select the correct board and COM port, then upload the sketch to the Arduino.

### 7. Run Proteus simulation

Open the `.pdsprj` file in Proteus and start the simulation to validate the control logic before deployment.

## ▶️ Usage

1. Power on the system.
2. Ensure the sensor probes or sensor module are correctly placed in the tank.
3. Observe the LED/LCD status as the water level changes.
4. Listen for buzzer alerts when the system detects critical water levels.
5. Use the Proteus simulation to test different tank states without physical hardware.

### Output Interpretation

| Indicator | Meaning |
| --- | --- |
| Green LED | Safe or sufficient water level |
| Yellow LED | Medium or warning level |
| Red LED | Low water level or overflow alert |
| Buzzer | Critical alert condition |
| LCD | Text-based water level status |

## 🧠 Code Explanation

The firmware is usually organized around the following functions:

- **Sensor reading**: Reads the state of each water level probe or sensor input.
- **Water level calculation**: Converts raw input states into a meaningful level classification.
- **Alert triggering**: Activates the buzzer or warning LEDs when a threshold is crossed.
- **Display handling**: Updates the LCD or serial monitor with the current water status.

In a typical Arduino loop, the program repeatedly checks sensor values, applies threshold logic, and updates outputs in real time.

## 🧪 Simulation Guide

To run the project in Proteus:

1. Open the Proteus project file (`.pdsprj`).
2. Verify that all components are connected correctly.
3. Load the compiled Arduino HEX file into the microcontroller model, if required.
4. Start the simulation.
5. Change sensor input states to simulate water rising or falling in the tank.
6. Observe LED, LCD, and buzzer responses.

For best results, keep the Proteus schematic and Arduino code synchronized so the simulation matches the actual circuit behavior.

## 🛡 Troubleshooting

| Problem | Possible Cause | Solution |
| --- | --- | --- |
| Sensor not responding | Incorrect wiring or loose connections | Recheck sensor pins, power, and ground connections |
| Proteus simulation errors | Missing libraries or incorrect component model | Verify Proteus assets and load the proper project files |
| Upload issues | Wrong board or COM port selected | Select the correct Arduino board and serial port |
| Incorrect readings | Sensor thresholds or logic not calibrated | Adjust threshold values in the code and retest |
| LCD not displaying text | Wiring or I2C address mismatch | Confirm pin connections and LCD configuration |

## 🔮 Future Improvements

- IoT integration for remote tank monitoring
- Mobile app notifications and alerts
- WiFi or Bluetooth connectivity
- Cloud dashboard for historical water usage tracking
- Automatic motor control for pump start/stop automation

## 🤝 Contributing

Contributions are welcome. If you want to improve the project:

1. Fork the repository
2. Create a feature branch
3. Make your changes with clear commits
4. Test the Arduino code and simulation files
5. Open a pull request with a concise description of the improvement

Please keep contributions focused, well documented, and aligned with the existing project structure.

## 📜 License

This project is licensed under the **MIT License**. See the LICENSE file for full details.

## 👨‍💻 Author

**Your Name**  
GitHub: [@your-username](https://github.com/your-username)

---

Built for learning, prototyping, and practical embedded systems development.