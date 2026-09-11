# Architettura demo AICompanion V1

## Vista generale

```text
                  ┌──────────────────┐
                  │  Gemini Live API │
                  └────────▲─────────┘
                           │
                           │ WSS
                           │
                    ┌──────┴──────┐
                    │   ESP32-S3  │
                    │             │
                    │ Wi-Fi       │
                    │ WebSocket   │
                    │ Audio       │
                    └───┬─────┬───┘
                        │     │
                      I2S     I2S
                        │     │
                        ▼     ▼
                   Microfono MAX98357A
                              │
                              ▼
                           Speaker
```

## Architettura software

Il firmware è pensato per essere modulare e per ridurre la complessità del primo ciclo di sviluppo:

- boot e state machine
- Wi‑Fi manager
- audio input (microfono I2S)
- audio output (speaker / MAX98357A via I2S)
- Gemini client WSS
- configurazione e segreti locali

Il client Gemini usa l'endpoint WSS ufficiale documentato da Google, invia il
messaggio `setup` con la modalita `AUDIO` e trasmette l'input come raw PCM
16-bit, 16 kHz, little-endian tramite `realtimeInput.audio`. Le risposte server
sono parse come JSON `BidiGenerateContentServerMessage`; l'audio ricevuto e
individuato nei blocchi `serverContent.modelTurn.parts[].inlineData`.

Il design attuale segue la regola del progetto: non esiste un backend. La comunicazione avviene direttamente tra ESP32-S3 e Gemini Live API tramite WebSocket sicuro.

## Perché non esiste un backend

La demo utilizza una connessione client-to-server diretta tra ESP32-S3 e Gemini Live API per ridurre complessità e latenza. La gestione sicura delle credenziali tramite token effimeri potrà essere aggiunta in una versione successiva.

## Stato attuale

- boot base pronto
- struttura project verificabile
- client WebSocket Gemini e setup sessione compilati
- invio audio input predisposto
- dettagli hardware ancora da confermare
- file di incertezza aggiornato

## Note di sviluppo

- Le credenziali non vengono mai hardcoded nel repository.
- I pin del microfono e del DAC sono configurati come placeholder per non inventare il design hardware.
- L’implementazione audio è pronta per essere adattata al microfono e al DAC reali non appena disponibili.
