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
- supporto opzionale a OLED SSD1306 I2C 128x64 per visualizzare il `SystemState`

Per abilitare il display, configurare `OLED_SDA_PIN` e `OLED_SCL_PIN` in `src/config/config.h`.
L'indirizzo predefinito e `0x3C`; alcuni moduli richiedono `0x3D`. Se il display non viene
rilevato, il firmware continua a funzionare e mantiene il logging seriale.

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

## Configurazione iniziale e gestione sicura dei secret

Creare il file locale `secrets.h` partendo dal template:

```powershell
Copy-Item secrets.example.h secrets.h
```

Inserire in `secrets.h` SSID, password Wi-Fi e chiave Gemini. Il file è escluso da Git. Non inserirlo mai con `git add -f` e non copiare credenziali nei log o nei documenti del progetto. `secrets.example.h` contiene solo placeholder e può essere versionato.

Attivare il controllo automatico prima dei commit, una sola volta per questo repository:

```powershell
git config --local core.hooksPath .githooks
```

L'hook `pre-commit` controlla i file staged, blocca i file di credenziali e rileva alcuni formati comuni di chiavi e password senza stamparne i valori. Richiede Python 3. Dopo aver aggiunto un secret per errore allo staging, rimuoverlo con `git restore --staged <file>` e verificare la versione staged prima di riprovare.

Il controllo è una protezione aggiuntiva, non un rilevatore universale: verificare sempre `git diff --cached` prima del commit e revocare/ruotare immediatamente qualsiasi credenziale già condivisa o committata.

Poiché questa demo comunica direttamente con Gemini, la chiave viene incorporata nel firmware compilato e usata per aprire la connessione WebSocket. `secrets.h` evita di versionarla nel repository, ma non protegge la chiave da chi può leggere il firmware del dispositivo: usare una chiave dedicata e con restrizioni adeguate per i prototipi.

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
