# Hardware e connessioni

## Componenti previsti

- ESP32-S3
- microfono I2S
- MAX98357A
- speaker 4Ω / 3W
- connessione Wi‑Fi
- OLED SSD1306 I2C 128x64, 0,96 inch

## Collegamenti

Il design logico richiesto per la demo è:

```text
ESP32-S3
   │
   ├── I2S RX ◄── Microphone
   │
   ├── I2S TX ──► MAX98357A ──► Speaker
   │
   └── I2C SDA/SCL ──► OLED SSD1306
```

## GPIO

TODO — GPIO da confermare

Non è corretto inventare i pin del microfono o del MAX98357A senza la scheda reale e il datasheet ufficiale.

## Alimentazione

- verificare la tensione di alimentazione del microfono
- verificare la tensione di alimentazione del MAX98357A
- verificare la compatibilità con la logica dell'ESP32-S3
- verificare il speaker 4Ω / 3W

## I2S

- configurazione da adattare al modello hardware reale
- canale RX dedicato al microfono
- canale TX dedicato al MAX98357A
- campionamento e buffer da verificare con la documentazione ufficiale di Gemini Live API

## Jumper e compatibilità

- controllare eventuali jumper per audio, power e mute
- verificare eventuali problemi di compatibilità di livello logico
- verificare se sono necessari pull-up, pull-down o filtraggi

## Stato attuale

Il pinout e la configurazione hardware non sono ancora stati verificati: vengono quindi lasciati come placeholder e registrati in UNCERTAINTIES.txt.

## OLED I2C

Collegamento previsto:

| OLED | ESP32-S3 |
|---|---|
| VCC | alimentazione compatibile con il modulo |
| GND | GND comune |
| SDA | GPIO configurabile `OLED_SDA_PIN` |
| SCL | GPIO configurabile `OLED_SCL_PIN` |

Il firmware usa `0x3C` come indirizzo predefinito e visualizza il solo `SystemState`.
Alcuni moduli usano `0x3D`: in quel caso aggiornare `OLED_I2C_ADDRESS` in `src/config/config.h`.
I GPIO SDA/SCL e la presenza dei pull-up devono essere confermati sul modello reale della board e del modulo.
