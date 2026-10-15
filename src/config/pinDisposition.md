# AI Companion — pinout hardware

Scheda: ESP32-S3 N16R8.

| GPIO | Funzione |
|---|---|
| 4 | INMP441 SD |
| 15 | MAX98357 DIN |
| 16 | MAX98357 BCLK |
| 17 | MAX98357 LRC/WS |
| 18 | INMP441 SCK/BCLK |
| 19 | INMP441 WS |
| 8 | OLED SDA |
| 9 | OLED SCL |
| 5 | Pulsante, con `INPUT_PULLUP` |

## INMP441 — microfono

| Pin | Collegamento |
|---|---|
| VDD | ESP32 3V3 |
| GND | ESP32 GND |
| L/R | ESP32 GND (canale sinistro) |
| SCK | ESP32 GPIO 18 |
| WS | ESP32 GPIO 19 |
| SD | ESP32 GPIO 4 |

## MAX98357 — amplificatore

| Pin | Collegamento |
|---|---|
| VIN | ESP32 5V |
| GND | ESP32 GND |
| SD | ESP32 3V3 |
| DIN | ESP32 GPIO 15 |
| LRC | ESP32 GPIO 17 |
| BCLK | ESP32 GPIO 16 |
| GAIN | Non collegato |

Collega lo speaker tra `+` e `-` dell'amplificatore. Non collegare `-` a GND.

I clock del microfono e dell'amplificatore devono restare separati: sono generati da due periferiche I2S master configurate rispettivamente a 16 kHz e 24 kHz.

## OLED — I2C

| Pin | Collegamento |
|---|---|
| VCC | ESP32 3V3 |
| GND | ESP32 GND |
| SDA | ESP32 GPIO 8 |
| SCL | ESP32 GPIO 9 |

## Pulsante

Collega un terminale al GPIO 5 e l'altro a GND. Il firmware usa `INPUT_PULLUP`.
