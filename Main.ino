/*****************************************************
 * ESP32 + SELEC EM2M ENERGY METER
 * RS485 + BLYNK CLOUD
 * Measurement Algorithm UNCHANGED
 *****************************************************/

/* ---------- BLYNK CONFIG ---------- */
#define BLYNK_TEMPLATE_ID   "Blynk_Template_ID"
#define BLYNK_TEMPLATE_NAME "Blynk_Template_Name"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ModbusMaster.h>

/* ---------- WIFI & BLYNK CREDENTIALS ---------- */
char auth[] = "****************";
char ssid[] = "WiFi_Network_Name";
char pass[] = "WiFi_Password";

/* ---------- RS485 PIN DEFINITIONS (ESP32) ---------- */
#define RXD2      16
#define TXD2      17
#define MAX485_DE 4
#define MAX485_RE 5
#define SLAVE_ID  1

HardwareSerial rs485(2);
ModbusMaster node;

/* ---------- ENERGY VARIABLES ---------- */
float energy_kWh = 0.0;
unsigned long lastMillis = 0;

/* ---------- RS485 DIRECTION CONTROL ---------- */
void preTransmission() {
  digitalWrite(MAX485_RE, HIGH);
  digitalWrite(MAX485_DE, HIGH);
}

void postTransmission() {
  digitalWrite(MAX485_DE, LOW);
  digitalWrite(MAX485_RE, LOW);
}

/* ---------- READ FLOAT (2 INPUT REGISTERS) ---------- */
float readInputFloat(uint16_t reg) {
  uint8_t result = node.readInputRegisters(reg, 2);
  if (result != node.ku8MBSuccess) {
    Serial.print("Modbus error @ 0x");
    Serial.println(reg, HEX);
    return 0.0;
  }

  uint16_t lowWord  = node.getResponseBuffer(0);
  uint16_t highWord = node.getResponseBuffer(1);

  uint32_t raw = ((uint32_t)highWord << 16) | lowWord;
  float value;
  memcpy(&value, &raw, sizeof(value));
  return value;
}

/* ---------- SETUP ---------- */
void setup() {
  Serial.begin(115200);
  delay(1500);

  pinMode(MAX485_DE, OUTPUT);
  pinMode(MAX485_RE, OUTPUT);
  digitalWrite(MAX485_DE, LOW);
  digitalWrite(MAX485_RE, LOW);

  /* RS485 UART */
  rs485.begin(9600, SERIAL_8N1, RXD2, TXD2);
  node.begin(SLAVE_ID, rs485);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  /* WiFi Connection */
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  /* Blynk Connection */
  Blynk.begin(auth, ssid, pass);

  lastMillis = millis();

  Serial.println("SELEC EM2M Modbus Started (ESP32)");
  Serial.println("Energy calculated using V × I × PF");
}

/* ---------- LOOP ---------- */
void loop() {
  Blynk.run();

  /*
   * SELEC EM2M REGISTER OFFSETS (0-BASED)
   * Voltage      : 30020 → 0x14
   * Current      : 30022 → 0x16
   * Power Factor : 30024 → 0x18
   * Frequency    : 30026 → 0x1A
   */

  float voltage   = readInputFloat(0x14);   delay(50);
  float current   = readInputFloat(0x16);   delay(50);
  float pf        = readInputFloat(0x18);   delay(50);
  float frequency = readInputFloat(0x1A);   delay(50);

  /* ---------- ACTIVE POWER (UNCHANGED) ---------- */
  float activePower = voltage * current * pf;
  if (activePower < 0) activePower = 0;

  /* ---------- ENERGY CALCULATION (UNCHANGED) ---------- */
  unsigned long now = millis();
  float deltaHours = (now - lastMillis) / 3600000.0;
  lastMillis = now;

  energy_kWh += (activePower / 1000.0) * deltaHours;

  /* ---------- SEND TO BLYNK CLOUD ---------- */
  Blynk.virtualWrite(V0, voltage);
  Blynk.virtualWrite(V1, current);
  Blynk.virtualWrite(V2, pf);
  Blynk.virtualWrite(V3, frequency);
  Blynk.virtualWrite(V4, activePower);
  Blynk.virtualWrite(V5, energy_kWh);

  /* ---------- SERIAL OUTPUT ---------- */
  Serial.println("================================");
  Serial.print("Voltage (V)     : "); Serial.println(voltage, 2);
  Serial.print("Current (A)     : "); Serial.println(current, 2);
  Serial.print("Power Factor    : "); Serial.println(pf, 2);
  Serial.print("Frequency (Hz)  : "); Serial.println(frequency, 2);
  Serial.print("Power (W)       : "); Serial.println(activePower, 2);
  Serial.print("Energy (kWh)    : "); Serial.println(energy_kWh, 6);
  Serial.println("================================\n");

  delay(500);
}
