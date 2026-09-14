# Stato implementazione e piano test

Data verifica: 2026-09-11

## Stato generale

Il firmware PlatformIO per ESP32-S3 compila correttamente, ma la Demo V1 non e ancora verificata end-to-end su hardware reale.

Build verificata:

```text
RC:0
[SUCCESS]
```

## Implementato

- Progetto PlatformIO per ESP32-S3 con Arduino framework.
- Boot e logging seriale.
- Macchina a stati di base.
- Gestione Wi-Fi con timeout e riconnessione di base.
- Moduli I2S per input microfono e output speaker.
- Generatore di tono di test.
- Client WebSocket WSS diretto verso Gemini Live API.
- Messaggio `setup` Gemini.
- Invio audio PCM del microfono tramite `realtimeInput.audio`.
- Dipendenze WebSockets e ArduinoJson.
- Input audio configurato a 16 kHz, PCM 16-bit little-endian.
- Output audio previsto a 24 kHz, PCM 16-bit little-endian.
- Gestione di placeholder per secret e pin hardware.
- Modulo OLED SSD1306 I2C per la visualizzazione del `SystemState`.
- Registro delle incertezze in `UNCERTAINTIES.txt`.

## Mancante o non verificato

- Il PCM ricevuto da Gemini viene individuato, ma non ancora decodificato da Base64 e inviato a `AudioOutput`.
- Lo stato `SPEAKING` non e ancora collegato realmente alla ricezione audio.
- L'evento `interrupted` viene registrato, ma non svuota il buffer e non ferma il playback.
- Il tono di test deve essere disattivato durante il test Gemini.
- La sessione deve essere considerata pronta solo dopo la conferma `setupComplete`.
- TLS/WSS non e stato testato su ESP32 reale.
- Wi-Fi, microfono, MAX98357A e speaker non sono stati testati fisicamente.
- Flash, monitor seriale e conversazione end-to-end non sono stati eseguiti su device.
- Buffer thread-safe e task FreeRTOS dedicati non sono ancora implementati.
- Board reale, modello microfono, pin I2S, alimentazione e schema elettrico sono ancora da confermare.
- GPIO SDA/SCL, alimentazione e indirizzo del modulo OLED sono ancora da confermare su hardware reale.

## Configurazione locale

1. Copiare `secrets.example.h` come `secrets.h`.
2. Inserire localmente SSID, password Wi-Fi e API key Gemini.
3. Non committare `secrets.h`.
4. Confermare il metodo di autenticazione e il modello Gemini secondo la documentazione ufficiale Google.
5. Aggiornare `UNCERTAINTIES.txt` quando un dato viene verificato.

Comandi principali:

```text
pio run
pio device monitor -b 115200
```

## Piano di completamento software

1. Decodificare Base64 da `inlineData`.
2. Inserire i campioni ricevuti in un buffer output.
3. Inviare il buffer a `AudioOutput` a 24 kHz.
4. Collegare gli eventi audio agli stati `LISTENING` e `SPEAKING`.
5. Fermare il playback e svuotare il buffer su `interrupted`.
6. Gestire overflow, underflow, JSON invalido e allocazioni fallite.
7. Disattivare il tono continuo durante l'uso di Gemini.
8. Valutare task FreeRTOS e code audio dopo i primi test reali.

## Prerequisiti hardware

- Modello esatto della board ESP32-S3.
- Modello e datasheet del microfono I2S.
- Modello e datasheet del modulo MAX98357A.
- Pin BCLK, WS/LRCLK, DATA e MCLK eventualmente richiesto.
- Alimentazione, massa comune e collegamento speaker 4 ohm / 3 W.
- Porta seriale visibile dal computer.

I GPIO non devono essere dedotti: vanno confermati dalla documentazione dei componenti e riportati in `docs/hardware.md` e `UNCERTAINTIES.txt`.

## Piano test incrementale

### Test 1 - Boot

- Flash del firmware.
- Verifica del messaggio `[BOOT]`.
- Verifica dell'assenza di reset e watchdog.
- Se collegato, verifica della visualizzazione di `BOOT` e delle successive transizioni sul display OLED.

### Test 1a - OLED

- Confermare VCC, GND, SDA e SCL sul modulo reale.
- Verificare la risposta I2C a `0x3C`; provare `0x3D` se necessario.
- Verificare la visualizzazione degli stati senza bloccare il boot quando l'OLED e scollegato.

### Test 2 - Wi-Fi

- Inserimento dei secret locali.
- Verifica connessione, IP, timeout e riconnessione.
- Verifica che i secret non compaiano nei log.

### Test 3 - Microfono

- Configurazione dei GPIO confermati.
- Verifica inizializzazione I2S RX.
- Verifica presenza di campioni e assenza di overflow.

### Test 4 - Speaker

- Disabilitazione temporanea di Gemini.
- Riproduzione di un tono breve a volume basso.
- Verifica I2S TX, MAX98357A e speaker.
- Verifica assenza di underflow e rumore anomalo.

### Test 5 - WSS e setup

- Verifica DNS e handshake TLS.
- Verifica autenticazione.
- Verifica ricezione di `setupComplete`.
- Nessuna stampa di API key, token o password.

### Test 6 - Audio input Gemini

- Invio di chunk PCM dal microfono.
- Verifica eventi server e continuita del flusso.
- Monitoraggio heap e latenza.

### Test 7 - Audio output Gemini

- Decodifica del chunk audio ricevuto.
- Scrittura PCM a 24 kHz su I2S TX.
- Verifica playback sullo speaker.

### Test 8 - Interruzione

- Parlare durante una risposta.
- Verificare l'evento `interrupted`.
- Svuotare il buffer e fermare il playback.

### Test 9 - End-to-end

- Eseguire almeno cinque turni voce -> Gemini -> voce.
- Annotare latenza, heap minimo, errori e riconnessioni.
- Aggiornare README, `docs/architecture.md`, `docs/hardware.md` e `UNCERTAINTIES.txt`.

## Criterio di completamento V1

La V1 sara completata solo dopo la verifica fisica del percorso:

```text
Microfono -> ESP32-S3 -> WSS -> Gemini -> WSS -> ESP32-S3 -> MAX98357A -> Speaker
```

Fino a quel momento lo stato corretto e: **compilata, ma non verificata su hardware**.
