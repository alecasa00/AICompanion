# Stato implementazione e piano test

Data verifica: 2026-10-16

## Stato generale

Il firmware PlatformIO compila. Tutti i componenti hardware sono collegati e testati sul setup attuale e sembrano funzionare; la voce registrata si sente in loopback. La saldatura definitiva e rimandata finché non sarà verificata una risposta end-to-end da Gemini Live. La quota gratuita è quasi esaurita e nelle prove Gemini ha restituito errori di quota.

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
- Configurazione I2S RX a 32 bit per INMP441 e conversione dei campioni a PCM 16-bit.
- Buffer locale di loopback: riproduzione della registrazione dopo 2 secondi, con ricampionamento 16 -> 24 kHz, attenuazione e dissolvenze per il test sullo speaker.
- Clock I2S del microfono separati da quelli dell'amplificatore: INMP441 SCK/WS su GPIO 18/19, MAX98357 BCLK/LRC su GPIO 16/17. La condivisione dei clock tra due periferiche master con frequenze diverse causava acquisizione distorta.
- Loopback della voce verificato fisicamente dopo la separazione dei clock.
- Log audio con campioni acquisiti/inviati, min/max, RMS e campioni vicini al full scale.
- Log Gemini per connessione, `setupComplete`, frame ricevuti, errori e audio instradato all'uscita.
- Gestione di placeholder per secret e pin hardware.
- Modulo OLED SSD1306 I2C per la visualizzazione del `SystemState`.
- Registro delle incertezze in `UNCERTAINTIES.txt`.

## Mancante o non verificato

- La risposta audio di Gemini e la conversazione end-to-end non sono ancora state verificate con successo.
- Nelle prove Gemini il server ha chiuso la sessione con errore 1011 per quota/billing; il loopback locale funziona indipendentemente da Gemini.
- Buffer thread-safe e task FreeRTOS dedicati non sono ancora implementati.

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

## TODO prossima sessione

- Creare un metodo per impostare il volume dell'uscita audio, con livello configurabile e applicato sia al loopback sia all'audio ricevuto da Gemini.

## Piano di completamento software

1. Gestire overflow, underflow, JSON invalido e allocazioni fallite.
2. Verificare su hardware il playback a 24 kHz e il comportamento su `interrupted`.
3. Disattivare il tono continuo durante l'uso di Gemini.
4. Valutare task FreeRTOS e code audio dopo i primi test reali.

## Stato hardware

- Componenti e collegamenti hardware sono stati provati sul setup attuale e sembrano funzionare, incluso il loopback microfono-speaker.
- La saldatura definitiva è in attesa della conferma che Gemini riceva l'audio e risponda.

## Piano test incrementale

### Test 1 - Boot

- Verificato: boot e toni diagnostici udibili, senza problemi rilevati.
- Flash del firmware.
- Verifica del messaggio `[BOOT]`.
- Verifica dell'assenza di reset e watchdog.
- Se collegato, verifica della visualizzazione di `BOOT` e delle successive transizioni sul display OLED.

### Test 1a - OLED

- Verificato sul setup hardware attuale.
- Confermare VCC, GND, SDA e SCL sul modulo reale.
- Verificare la risposta I2C a `0x3C`; provare `0x3D` se necessario.
- Verificare la visualizzazione degli stati senza bloccare il boot quando l'OLED e scollegato.

### Test 2 - Wi-Fi

- Inserimento dei secret locali.
- Verifica connessione, IP, timeout e riconnessione.
- Verifica che i secret non compaiano nei log.

### Test 3 - Microfono

- Verificato: registrazione e loopback vocale udibile dopo aver separato i clock I2S.
- Configurazione dei GPIO confermati.
- Verifica inizializzazione I2S RX.
- Verifica presenza di campioni e assenza di overflow.

### Test 4 - Speaker

- Verificato: toni diagnostici e loopback udibili dallo speaker.
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

La V1 sara completata solo dopo la verifica end-to-end del percorso:

```text
Microfono -> ESP32-S3 -> WSS -> Gemini -> WSS -> ESP32-S3 -> MAX98357A -> Speaker
```

Stato attuale: **hardware collegato e verificato sul setup attuale; loopback microfono-speaker funzionante; conversazione Gemini end-to-end da confermare prima della saldatura definitiva**.
