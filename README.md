# Automatic-Car-Parking-System-Using-IoT

🚗 IoT-Based Automatic Car Parking System
This project is an IoT-based smart parking system designed to manage urban parking challenges by providing real-time vehicle detection, automated gate control, and remote monitoring capabilities using ESP32 microcontrollers, IR/Ultrasonic sensors, servo motors, and an OLED display.

📌 Features
🚘 Detects vehicle presence at entrance, exit, and parking slots

🌐 Sends real-time data via Wi-Fi using HTTP

📟 Displays slot status on OLED screen

🚪 Automatically opens/closes gates using servo motors

🌐 Web interface for monitoring (via ESP32-hosted server)

🧠 How It Works
The system has two modules:

1. Sensor Module
Detects vehicles using IR sensors (slots) and Ultrasonic sensors (entrance/exit)

Sends status data via HTTP in JSON format to a web server every 2 seconds

2. Server & Display Module
Receives data and displays parking slot status on an OLED

Controls servo motors for gate actuation

Hosts a simple web API for monitoring and control
