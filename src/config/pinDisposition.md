AI Companion — Pinout Hardware
ESP32-S3 N16R8
GPIO	Funzione
GPIO 4	INMP441 → SD
GPIO 15	MAX98357 → DIN
GPIO 16	I²S Clock → SCK/BCLK
GPIO 17	I²S Word Select → WS/LRC
GPIO 8	OLED → SDA
GPIO 9	OLED → SCL
GPIO 5	Pulsante → INPUT
🎤 INMP441 — Microfono
Pin INMP441	Collegare a
VDD	ESP32 3V3
GND	ESP32 GND
L/R	ESP32 GND
SCK	ESP32 GPIO 16
WS	ESP32 GPIO 17
SD	ESP32 GPIO 4

L/R → GND = canale sinistro.

🔊 MAX98357 — Amplificatore
Pin MAX98357	Collegare a
VIN	ESP32 5V
GND	ESP32 GND
SD	ESP32 3V3
DIN	ESP32 GPIO 15
LRC	ESP32 GPIO 17
BCLK	ESP32 GPIO 16
GAIN	Non collegato
Speaker
MAX98357	Speaker
+	+ Speaker
−	− Speaker

Non collegare il − dello speaker a GND.

🖥️ OLED — I²C
Pin OLED	Collegare a
VCC	ESP32 3V3
GND	ESP32 GND
SDA	ESP32 GPIO 8
SCL	ESP32 GPIO 9
🔘 Pulsante
Pin	Collegare a
Un terminale	GPIO 5
Altro terminale	GND

Nel firmware useremo normalmente:

INPUT_PULLUP

quindi non serve una resistenza esterna.

Schema complessivo
                         ESP32-S3 N16R8
                    ┌─────────────────────┐
                    │                     │
       3V3 ─────────┼── INMP441 VDD      │
                    │                     │
       GND ─────────┼── INMP441 GND      │
                    │   INMP441 L/R      │
                    │                     │
 GPIO 4 ◄───────────┼── INMP441 SD       │
 GPIO 16 ───────────┼── INMP441 SCK      │
 GPIO 17 ───────────┼── INMP441 WS       │
                    │                     │
 GPIO 15 ───────────┼── MAX98357 DIN     │
 GPIO 16 ───────────┼── MAX98357 BCLK    │
 GPIO 17 ───────────┼── MAX98357 LRC     │
       3V3 ─────────┼── MAX98357 SD      │
       5V ──────────┼── MAX98357 VIN     │
       GND ─────────┼── MAX98357 GND     │
                    │                     │
 GPIO 8 ────────────┼── OLED SDA         │
 GPIO 9 ────────────┼── OLED SCL         │
       3V3 ─────────┼── OLED VCC         │
       GND ─────────┼── OLED GND         │
                    │                     │
 GPIO 5 ◄───────────┼── BUTTON ── GND    │
                    │                     │
                    └─────────────────────┘

                         MAX98357
                         + ───── Speaker +
                         − ───── Speaker −
Configurazione GPIO da copiare nel firmware