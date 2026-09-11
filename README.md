# AICompanion

Demo V1 per assistente AI vocale ESP32-S3 diretto verso Gemini Live API.

## Stato del progetto

- Framework: PlatformIO + Arduino core per ESP32
- Hardware target: ESP32-S3
- Architettura: client diretto verso Gemini Live API tramite WebSocket WSS
- Backend: non previsto in questa demo
- Hardware reale: non ancora disponibile, quindi tutti i pin e i dettagli del circuito sono da confermare
- Gemini Live API: client WSS base implementato secondo la documentazione ufficiale Google

## Milestone attuale

### Milestone 1 - Boot

Implementato il punto di partenza del firmware:
- boot seriale con logging chiaro
- macchina a stati base
- struttura modulare del progetto
- placeholder per credenziali e incertezze hardware
- client WebSocket WSS e messaggio di setup Gemini
- invio dei chunk PCM del microfono quando l'I2S sara configurato

## Struttura progetto

```text
src/
  main.cpp
  config/
    config.h

docs/
  architecture.md
  hardware.md

platformio.ini
UNCERTAINTIES.txt
secrets.example.h
.gitignore
```

## Configurazione iniziale

Prima di testare il firmware reale, creare un file locale `secrets.h` basato su `secrets.example.h` e inserire i valori corretti per la rete e per Gemini.

Non committare secret reali.

## Build

```bash
pio run
```

## Debug

```bash
pio device monitor
```

## Note

Tutti i dettagli non verificati vengono registrati in `UNCERTAINTIES.txt` e non vengono inventati nel codice.
