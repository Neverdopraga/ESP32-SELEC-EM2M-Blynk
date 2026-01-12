# ESP32-SELEC-EM2M-Blynk
ESP32-based energy monitoring system using SELEC EM2M via RS485 Modbus RTU with Blynk Cloud


Blynk Cloud Dashboard

## 🧰 Hardware Components
| Component | Description |
|---------|-------------|
| ESP32 Dev Board | Main IoT controller |
| SELEC EM2M | Industrial energy meter |
| MAX485 | RS485 to UART converter |
| Power Supply | 230V AC (meter side) |
| Jumper Wires | Signal connections |

---

## 🔌 ESP32 ↔ MAX485 Pin Configuration

| Signal | ESP32 GPIO | Description |
|------|------------|-------------|
| RO | GPIO16 | UART RX |
| DI | GPIO17 | UART TX |
| DE | GPIO4 | Driver Enable |
| RE | GPIO5 | Receiver Enable |
| VCC | 5V | Power |
| GND | GND | Ground |

---

## 📡 Communication Parameters
| Parameter | Value |
|---------|------|
| Protocol | Modbus RTU |
| Interface | RS485 |
| Baud Rate | 9600 |
| Data Bits | 8 |
| Stop Bits | 1 |
| Parity | None |
| Slave ID | 1 |

---

## 📊 Modbus Register Mapping (Input Registers)

| Parameter | Register Address |
|---------|------------------|
| Voltage (V) | 30020 |
| Current (A) | 30022 |
| Power Factor | 30024 |
| Frequency (Hz) | 30026 |

> Registers are accessed using **0-based offsets** in the firmware.

---

## ☁️ Blynk Virtual Pin Mapping

| Virtual Pin | Parameter |
|-----------|-----------|
| V0 | Voltage |
| V1 | Current |
| V2 | Power Factor |
| V3 | Frequency |
| V4 | Active Power |
| V5 | Energy (kWh) |

---

## ⚙️ Power & Energy Calculation

Energy is calculated using elapsed time (`millis()`), ensuring continuous and accurate measurement.

---

## 📦 Software & Libraries
- Arduino IDE  
- ESP32 Board Package  
- Libraries Used:
  - Blynk
  - ModbusMaster
  - WiFi (ESP32 Core)

---

## 🚀 How to Run the Project
1. Install Arduino IDE and ESP32 board support  
2. Install required libraries  
3. Connect hardware as per wiring table  
4. Enter WiFi & Blynk credentials  
5. Upload the code to ESP32  
6. Open Serial Monitor (115200 baud)  
7. View live data on Blynk dashboard  

---

## 🔐 Security Note
Sensitive credentials are masked for public repositories.

```cpp
char auth[] = "YOUR_BLYNK_AUTH_TOKEN";
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

