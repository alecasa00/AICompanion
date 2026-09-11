# AICompanion — Demo V1

## Obiettivo

Realizzare una demo funzionante di un assistente AI vocale fisico basato su:

* ESP32-S3
* Wi-Fi
* microfono I2S
* MAX98357A tramite I2S
* speaker 4 Ohm / 3W
* Gemini Live API
* WebSocket sicuro (WSS)

### Architettura obbligatoria della demo

La demo deve utilizzare esclusivamente questa architettura:

```text
Microfono
    │
    │ I2S
    ▼
ESP32-S3
    │
    │ Wi-Fi
    │ WSS / WebSocket
    ▼
Gemini Live API
    │
    │ WSS / WebSocket
    ▼
ESP32-S3
    │
    │ I2S
    ▼
MAX98357A
    │
    ▼
Speaker
```

## IMPORTANTE

### NON creare un backend

Questa versione è una demo/proof-of-concept.

NON creare:

* backend Java
* Spring Boot
* server locale
* proxy WebSocket
* API server intermedio
* database
* microservizi
* Docker per il backend

L'ESP32-S3 deve comunicare **direttamente** con Gemini Live API.

Il backend potrà eventualmente essere introdotto in una versione futura, ma è completamente fuori dallo scope di questa demo.

---

# 1. Prima fase: analisi del repository

Prima di scrivere codice:

1. analizzare il repository esistente;
2. identificare il framework utilizzato;
3. verificare quale scheda ESP32-S3 è configurata;
4. verificare quali librerie sono già presenti;
5. non eliminare codice esistente senza una motivazione;
6. verificare la configurazione hardware già presente.

Se il progetto non è ancora configurato, utilizzare una struttura semplice e adatta all'ESP32-S3.

Preferire PlatformIO se il progetto lo utilizza già.

Non introdurre framework o librerie non necessarie.

---

# 2. Regola fondamentale: non inventare informazioni

NON inventare mai:

* API key
* token
* password Wi-Fi
* SSID
* URL
* endpoint WebSocket
* modello Gemini
* parametri API
* GPIO
* sample rate
* formato audio
* codec
* certificati
* nomi o versioni delle librerie
* configurazioni hardware

Se un'informazione non è verificabile, NON inventarla.

Utilizzare un placeholder e registrare l'incertezza.

---

# 3. File UNCERTAINTIES.txt

Creare nella root del progetto:

```text
UNCERTAINTIES.txt
```

Questo file deve contenere ogni informazione che Copilot non può determinare con certezza.

Per ogni voce specificare:

```text
[TODO]
Elemento:
Stato:
Perché è necessario:
Dove deve essere configurato:
Valore placeholder:
Come verificarlo:
```

Esempio:

```text
[TODO]
Elemento: Gemini API Key
Stato: MANCA
Perché è necessario: autenticazione verso Gemini Live API
Dove deve essere configurato: secrets/configurazione locale
Valore placeholder: YOUR_GEMINI_API_KEY
Come verificarlo: documentazione ufficiale Gemini API
```

Se durante lo sviluppo emerge un'altra informazione incerta, aggiungerla immediatamente al file.

NON lasciare informazioni incerte solamente nei commenti del codice.

---

# 4. Gestione dei secret

NON inserire mai secret reali nel repository.

Non hardcodare:

```text
API_KEY
TOKEN
PASSWORD
WIFI_PASSWORD
CLIENT_SECRET
PRIVATE_KEY
```

Utilizzare una configurazione locale esclusa da Git.

Creare, se appropriato:

```text
secrets.example.h
```

con placeholder come:

```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define GEMINI_API_KEY "YOUR_GEMINI_API_KEY"
```

Il file reale:

```text
secrets.h
```

deve essere inserito in `.gitignore`.

Aggiornare `.gitignore` di conseguenza.

Non stampare mai secret nei log seriali.

---

# 5. Documentazione ufficiale

Per tutte le informazioni relative a Gemini Live API utilizzare esclusivamente la documentazione ufficiale di Google.

Non inventare il protocollo WebSocket.

Verificare dalla documentazione ufficiale:

* endpoint;
* autenticazione;
* modello disponibile;
* formato del messaggio `setup`;
* formato `realtimeInput`;
* formato delle risposte;
* formato dell'audio;
* sample rate;
* formato PCM;
* gestione delle interruzioni;
* chiusura della sessione;
* eventuali requisiti TLS.

Se un dettaglio non è verificabile, inserirlo in `UNCERTAINTIES.txt`.

---

# 6. Hardware

L'hardware target è:

```text
ESP32-S3
Microfono I2S
MAX98357A
Speaker 4Ω / 3W
```

## Microfono

Il microfono deve essere collegato all'ingresso I2S dell'ESP32-S3.

## MAX98357A

Il MAX98357A deve essere collegato all'uscita I2S dell'ESP32-S3.

Schema logico:

```text
ESP32-S3
   │
   ├── I2S RX ◄── Microphone
   │
   └── I2S TX ──► MAX98357A ──► Speaker
```

NON assumere automaticamente i GPIO.

Se i GPIO non sono definiti nel progetto o nella configurazione hardware, registrarli in:

```text
UNCERTAINTIES.txt
```

e usare una configurazione facilmente modificabile.

---

# 7. Audio input

Configurare il microfono in base ai requisiti effettivi di Gemini Live API.

Il formato previsto dalla documentazione deve essere verificato prima dell'implementazione definitiva.

L'architettura software deve essere:

```text
Microphone
    ↓
I2S RX
    ↓
PCM buffer
    ↓
audio chunk
    ↓
Gemini Live WebSocket
```

Utilizzare buffer piccoli e adatti allo streaming realtime.

Evitare di accumulare grandi quantità di audio in RAM.

La dimensione dei chunk deve essere scelta sulla base delle raccomandazioni ufficiali dell'API.

---

# 8. Audio output

L'audio ricevuto da Gemini deve seguire:

```text
Gemini
    ↓
WebSocket
    ↓
decode
    ↓
PCM buffer
    ↓
I2S TX
    ↓
MAX98357A
    ↓
Speaker
```

Il formato audio deve essere quello effettivamente restituito da Gemini Live API.

Non effettuare conversioni inutili.

Se è necessaria una conversione di sample rate, implementarla solamente dopo aver verificato il formato reale dell'API.

---

# 9. WebSocket

Implementare una connessione:

```text
ESP32-S3
     │
     │ WSS
     ▼
Gemini Live API
```

La gestione WebSocket deve supportare:

* connessione;
* handshake;
* setup della sessione;
* invio audio;
* ricezione eventi;
* ricezione audio;
* gestione errori;
* timeout;
* disconnessione;
* riconnessione.

Non implementare un protocollo proprietario se non necessario.

Seguire il protocollo ufficiale Gemini Live API.

---

# 10. TLS

La connessione deve utilizzare WSS/TLS.

Non disabilitare TLS per semplificare il prototipo.

Se per il funzionamento è necessario configurare un certificato CA o una modalità specifica di verifica TLS:

1. verificare la documentazione della libreria utilizzata;
2. implementare la configurazione corretta;
3. documentarla;
4. se manca un'informazione, inserirla in `UNCERTAINTIES.txt`.

---

# 11. Architettura software

Separare il firmware in componenti logici.

Una possibile struttura:

```text
src/
│
├── main.cpp
│
├── wifi/
│   └── wifi_manager.*
│
├── audio/
│   ├── audio_input.*
│   ├── audio_output.*
│   └── audio_buffer.*
│
├── gemini/
│   ├── gemini_client.*
│   └── gemini_protocol.*
│
└── config/
    └── config.*
```

Adattare la struttura al progetto esistente.

Non creare file inutili.

---

# 12. Task e concorrenza

L'audio realtime non deve dipendere da un unico ciclo `loop()` bloccante.

Quando appropriato, utilizzare task FreeRTOS separati per:

```text
Wi-Fi / WebSocket
        │
        ├── Audio RX
        │
        └── Audio TX
```

I buffer devono essere gestiti tramite strutture thread-safe.

Prestare attenzione a:

* race condition;
* deadlock;
* overflow;
* underflow;
* frammentazione della heap;
* watchdog;
* priorità dei task.

Non creare un numero eccessivo di task.

---

# 13. Macchina a stati

Implementare una macchina a stati semplice:

```text
BOOT
  ↓
CONNECTING_WIFI
  ↓
WIFI_CONNECTED
  ↓
CONNECTING_GEMINI
  ↓
READY
  ↓
LISTENING
  ↓
SPEAKING
  ↓
LISTENING
```

Con stati di errore:

```text
ERROR
RECONNECTING
```

La macchina a stati deve essere semplice e facilmente estendibile.

---

# 14. Streaming realtime

L'obiettivo principale della demo è ottenere una conversazione vocale realtime.

Il flusso deve essere:

```text
Utente parla
     ↓
Microfono
     ↓
ESP32
     ↓
WebSocket
     ↓
Gemini
     ↓
Risposta audio
     ↓
ESP32
     ↓
Speaker
```

Non implementare inizialmente funzionalità non necessarie come:

* memoria persistente;
* database;
* autenticazione utente;
* UI;
* wake word;
* LLM locale;
* comandi complessi;
* tool calling;
* configurazioni cloud elaborate.

La priorità è far funzionare il ciclo voce → AI → voce.

---

# 15. Interruzione della risposta

Progettare il codice in modo che sia possibile gestire in futuro l'interruzione della risposta dell'AI.

Se Gemini segnala che l'utente ha interrotto la risposta:

```text
Gemini
   ↓
interrupted
   ↓
ESP32
   ↓
svuota output buffer
   ↓
stop playback
```

Se questa funzionalità è supportata dall'API e può essere implementata senza complicare eccessivamente la V1, implementarla.

Altrimenti predisporre l'architettura senza bloccare la successiva implementazione.

---

# 16. Voice Activity Detection

NON implementare inizialmente un VAD locale complesso.

Utilizzare le funzionalità disponibili nell'API quando possibile.

L'ESP32 deve concentrarsi su:

* acquisizione audio;
* trasmissione;
* ricezione;
* riproduzione.

Il VAD locale potrà essere aggiunto in una versione successiva.

---

# 17. Logging

Implementare logging seriale chiaro.

Esempio:

```text
[BOOT] AICompanion starting
[WIFI] Connecting...
[WIFI] Connected
[WS] Connecting to Gemini...
[WS] Connected
[AI] Session initialized
[AUDIO] Input initialized
[AUDIO] Output initialized
[AI] Listening
[AI] Response started
[AI] Response completed
```

Non stampare mai:

```text
API key
password
token
secret
cookie
```

In caso di errore, stampare informazioni utili al debugging senza rivelare credenziali.

---

# 18. Gestione degli errori

Gestire almeno:

### Wi-Fi

```text
connection failed
connection lost
timeout
```

### WebSocket

```text
connection failed
TLS error
authentication error
timeout
connection closed
```

### Audio

```text
I2S initialization failure
buffer overflow
buffer underflow
invalid audio data
```

### Memoria

Gestire in modo esplicito eventuali:

```text
allocation failure
heap exhaustion
buffer allocation failure
```

Evitare reset continui causati da errori recuperabili.

---

# 19. Performance

La demo deve privilegiare la bassa latenza.

Evitare:

* copie inutili dei buffer;
* conversioni audio non necessarie;
* allocazioni dinamiche frequenti;
* grandi buffer;
* logging eccessivo durante lo streaming;
* operazioni bloccanti.

Monitorare durante il debugging:

```text
free heap
minimum free heap
PSRAM disponibile
dimensione buffer
tempo di connessione
latenza audio
```

Se la scheda dispone di PSRAM, valutarne l'utilizzo per buffer appropriati, senza utilizzarla indiscriminatamente.

---

# 20. README.md

Creare/aggiornare `README.md`.

Deve contenere:

## Hardware

* ESP32-S3
* microfono I2S
* MAX98357A
* speaker

## Software

* framework utilizzato;
* toolchain;
* librerie;
* dipendenze.

## Configurazione

Spiegare come configurare:

```text
Wi-Fi SSID
Wi-Fi password
Gemini API key
```

senza riportare valori reali.

## Build

Spiegare come compilare il firmware.

## Flash

Spiegare come caricarlo sull'ESP32-S3.

## Run

Spiegare come avviare la demo.

## Debug

Spiegare come leggere i log seriali.

## Troubleshooting

Aggiungere i problemi più probabili:

* Wi-Fi;
* WebSocket;
* TLS;
* autenticazione;
* microfono;
* speaker;
* I2S;
* memoria.

---

# 21. docs/architecture.md

Creare:

```text
docs/architecture.md
```

Documentare l'architettura:

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

Spiegare anche perché il backend non è presente:

> La demo utilizza una connessione client-to-server diretta tra ESP32-S3 e Gemini Live API per ridurre complessità e latenza. La gestione sicura delle credenziali tramite token effimeri potrà essere aggiunta in una versione successiva.

---

# 22. docs/hardware.md

Creare:

```text
docs/hardware.md
```

Documentare:

* componenti;
* collegamenti;
* GPIO;
* alimentazione;
* I2S;
* eventuali jumper;
* eventuali problemi di compatibilità.

NON inventare i GPIO.

Se non sono ancora noti:

```text
TODO — GPIO da confermare
```

e aggiungere la voce anche in:

```text
UNCERTAINTIES.txt
```

---

# 23. Versionamento

Creare un `.gitignore` appropriato.

Escludere almeno:

```text
secrets.h
.env
.env.*
credentials.*
*.pem
*.key
```

Non escludere accidentalmente file necessari alla compilazione.

Creare eventualmente:

```text
secrets.example.h
```

come template pubblico.

---

# 24. Ordine di implementazione

NON implementare tutto contemporaneamente.

Procedere in questo ordine.

## Milestone 1 — Boot

```text
ESP32-S3
    ↓
Boot
    ↓
Serial logging
```

Obiettivo: verificare che il firmware parta correttamente.

---

## Milestone 2 — Wi-Fi

```text
ESP32-S3
    ↓
Wi-Fi
```

Verificare:

* connessione;
* IP;
* riconnessione.

---

## Milestone 3 — Microfono

```text
Microfono
    ↓
I2S
    ↓
ESP32
```

Verificare che vengano acquisiti dati audio validi.

---

## Milestone 4 — Speaker

```text
ESP32
    ↓
I2S
    ↓
MAX98357A
    ↓
Speaker
```

Verificare la riproduzione di un segnale audio di test.

---

## Milestone 5 — WebSocket

```text
ESP32
    ↓
WSS
    ↓
Gemini
```

Prima stabilire solamente la connessione e la sessione.

---

## Milestone 6 — Gemini

Implementare:

```text
setup
```

e verificare che Gemini accetti correttamente la sessione.

---

## Milestone 7 — Audio input

Implementare:

```text
Microfono
 ↓
PCM
 ↓
WebSocket
 ↓
Gemini
```

---

## Milestone 8 — Audio output

Implementare:

```text
Gemini
 ↓
WebSocket
 ↓
PCM
 ↓
MAX98357A
 ↓
Speaker
```

---

## Milestone 9 — Conversazione completa

Ottenere:

```text
PARLO
  ↓
Gemini riceve
  ↓
Gemini elabora
  ↓
Gemini risponde
  ↓
ASCOLTO
```

---

## Milestone 10 — Stabilizzazione

Solo dopo il funzionamento completo:

* ottimizzare memoria;
* ottimizzare latenza;
* migliorare riconnessione;
* migliorare gestione errori;
* migliorare logging;
* pulire il codice.

---

# 25. Regola per ogni milestone

Al termine di ogni milestone aggiornare:

```text
README.md
UNCERTAINTIES.txt
docs/architecture.md
docs/hardware.md
```

e annotare:

```text
IMPLEMENTATO:
...

TESTATO:
...

NON TESTATO:
...

PROBLEMI:
...

INFORMAZIONI MANCANTI:
...

PROSSIMO PASSO:
...
```

Non dichiarare una funzionalità "funzionante" se non è stata effettivamente verificata.

---

# 26. Criterio di completamento della Demo V1

La demo è considerata completata quando:

```text
ESP32-S3
    │
    ├── Wi-Fi
    │
    ├── Microphone I2S
    │
    ├── WebSocket WSS
    │
    ▼
Gemini Live API
    │
    ▼
WebSocket WSS
    │
    ▼
ESP32-S3
    │
    ├── I2S
    ▼
MAX98357A
    │
    ▼
Speaker
```

permette all'utente di parlare e ricevere una risposta vocale dall'assistente in tempo reale.

Tutto ciò che non è necessario per raggiungere questo risultato deve essere rimandato.

---

# Principio generale

La regola principale del progetto è:

> **Prima far funzionare il percorso voce → Gemini → voce. Poi aggiungere funzionalità.**

Semplicità e verificabilità hanno la precedenza sulla quantità di funzionalità.

Se una decisione tecnica non è certa, NON inventarla: registrarla in `UNCERTAINTIES.txt` e utilizzare un placeholder.
