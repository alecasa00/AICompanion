# AI Desktop Robot — Project Checklist

## Current project status — 2026-10-06

This repository is the Demo V1 ESP32-S3 firmware, using a direct WebSocket connection to Gemini Live API. A separate backend is outside this demo's scope. Checkmarks below indicate code or documentation present in the repository; they do not imply successful testing on physical hardware. The firmware build was recorded as successful in `docs/implementation-status.md`, but Wi-Fi, OLED, I2S devices, WSS/TLS, and end-to-end voice conversation have not been verified on hardware.

Current implementation includes PlatformIO/Arduino setup, Wi-Fi management, OLED state text, I2S microphone and speaker modules, a test tone, and a Gemini Live WebSocket client that sends setup and microphone PCM. Gemini audio output is not yet decoded and played through the speaker; interruption handling, confirmed session readiness, and hardware pin selection remain incomplete.

## Hardware

### Core
- [x] ESP32-S3 development board
- [ ] ESP32-S3 variant with PSRAM
- [ ] USB-C cable / power supply

### Display
- [ ] 1.3" OLED display
- [ ] 128×64 resolution
- [ ] I2C interface
- [ ] SSD1306 or SH1106 controller

### Audio input
- [ ] INMP441 I2S microphone
- [ ] Microphone mounting / acoustic opening in the case

### Audio output
- [ ] MAX98357A I2S amplifier
- [ ] 4 Ω / 3 W speaker
- [ ] Speaker grille / acoustic opening in the case

### User controls
- [ ] Physical push button
- [ ] RGB LED (WS2812B / SK6812 or equivalent)

### Prototyping
- [ ] Breadboard / prototyping PCB
- [ ] Dupont / jumper wires
- [ ] Resistors and basic passive components
- [ ] Headers / connectors as required

### Optional / later
- [ ] Second microphone for better audio processing
- [ ] Li-Po / Li-Ion battery
- [ ] USB-C battery charging circuit
- [ ] Power switch
- [ ] Custom PCB
- [ ] Additional LEDs / status indicators

---

## Firmware

### Basic system
- [x] ESP32-S3 firmware project (PlatformIO, Arduino framework)
- [ ] Wi-Fi connection
- [x] Wi-Fi reconnection handling (implemented; hardware behavior unverified)
- [x] Device configuration placeholders
- [ ] Error handling (partial; see `docs/implementation-status.md`)
- [ ] OTA firmware updates

### OLED / robot face
- [x] OLED initialization (implemented; hardware unverified)
- [x] Basic text rendering for system state
- [ ] Eye rendering
- [ ] Eye blinking animation
- [ ] Looking left / right
- [ ] Idle animation
- [ ] Listening animation
- [ ] Thinking animation
- [ ] Speaking animation
- [ ] Happy / positive state
- [ ] Sad / negative state
- [ ] Error state
- [ ] Sleeping state

### Audio
- [x] I2S microphone input module (hardware unverified)
- [ ] Audio buffering
- [ ] Recording start / stop
- [x] Audio encoding for Gemini input (PCM to Base64; end-to-end unverified)
- [x] Speaker output via I2S module and test tone (hardware unverified)
- [ ] Volume control
- [ ] Audio playback interruption handling

### User interaction
- [ ] Push-to-talk mode
- [ ] Recording indicator
- [ ] Processing indicator
- [ ] Response indicator
- [ ] Wake word detection
- [ ] Wake word such as "Hey Robo"

### Audio processing
- [ ] Voice Activity Detection (VAD)
- [ ] Noise suppression
- [ ] Acoustic Echo Cancellation (AEC)
- [ ] Optional multi-microphone processing

---

## AI / Cloud

### Backend (outside Demo V1 scope)
- [ ] Spring Boot backend
- [ ] Device authentication
- [ ] Device identification
- [ ] HTTPS communication
- [ ] Request / response protocol
- [ ] Logging
- [ ] Configuration management
- [ ] Provider abstraction

### Speech-to-Text
- [ ] STT provider integration
- [ ] Whisper / Groq integration
- [ ] Audio upload
- [ ] Transcription handling
- [ ] STT error handling
- [ ] Italian language support

### LLM
- [ ] LLM provider integration
- [x] Gemini Live API client and setup message (implemented; WSS/session unverified)
- [ ] Conversation prompt
- [ ] System personality
- [ ] Conversation history
- [ ] Context management
- [ ] Response length control
- [ ] Error / timeout handling
- [ ] Provider abstraction

### Text-to-Speech
- [ ] TTS provider integration
- [ ] Google Cloud TTS and/or Gemini TTS
- [ ] Italian voice
- [ ] Audio format compatible with ESP32
- [ ] Streaming or chunked playback
- [ ] TTS error handling

---

## Conversation

- [ ] User speaks to robot
- [ ] Robot detects / receives speech
- [ ] Speech converted to text
- [ ] Text sent to LLM
- [ ] LLM response received
- [ ] Response converted to speech
- [ ] Robot speaks response
- [ ] Robot animates while listening
- [ ] Robot animates while thinking
- [ ] Robot animates while speaking
- [ ] Conversation interruption
- [ ] Conversation timeout
- [ ] Network failure recovery

---

## Robot Personality

- [ ] Robot name
- [ ] Base personality
- [ ] System prompt
- [ ] Emotional states
- [ ] Emotion → eye animation mapping
- [ ] Emotion → voice/TTS settings
- [ ] Greeting
- [ ] Farewell
- [ ] Idle behavior
- [ ] Sleep behavior
- [ ] Startup animation
- [ ] Shutdown animation
- [ ] Random small idle animations

---

## AI Tools / Assistant Functions

### Initial tools
- [ ] Timer
- [ ] Reminders
- [ ] Weather
- [ ] Web search
- [ ] Calendar

### Advanced tools
- [ ] Home automation
- [ ] Smart lights
- [ ] Smart plugs
- [ ] PC control
- [ ] Music control
- [ ] Custom user-defined commands
- [ ] Function / tool calling

---

## Memory

- [ ] Short-term conversation memory
- [ ] Conversation history
- [ ] Persistent user preferences
- [ ] Long-term memory
- [ ] Memory retrieval
- [ ] Memory management / deletion
- [ ] Privacy controls

---

## Connectivity & Reliability

- [ ] Wi-Fi setup
- [ ] Wi-Fi credentials management
- [ ] Automatic reconnection
- [ ] Backend availability check
- [ ] API timeout handling
- [ ] Offline state
- [ ] User-visible error state
- [ ] Automatic retry
- [ ] Firmware OTA update
- [ ] Remote configuration

---

## 3D Printed Body

### Mechanical design
- [ ] Define final dimensions
- [ ] Define OLED position
- [ ] Define microphone position
- [ ] Define speaker position
- [ ] Define USB-C access
- [ ] Define button position
- [ ] Define LED position
- [ ] Define PCB mounting points
- [ ] Define cable routing
- [ ] Define ventilation / acoustic openings
- [ ] Define assembly method
- [ ] Define removable back / access panel

### Aesthetic
- [ ] Minimal design
- [ ] Cute robot appearance
- [ ] Face integrated into OLED
- [ ] Rounded geometry
- [ ] Decide body material / filament
- [ ] Finalize color scheme
- [ ] Design supports / stand
- [ ] Print prototype
- [ ] Test fit
- [ ] Final print

---

## Development Milestones

### Milestone 1 — Electronics
- [ ] ESP32-S3 running on physical hardware
- [ ] OLED working
- [ ] Microphone working
- [ ] Speaker working
- [ ] Button working
- [ ] RGB LED working

### Milestone 2 — Robot
- [ ] Animated eyes
- [ ] Audio recording verified on hardware
- [ ] Gemini audio playback verified on hardware
- [ ] Wi-Fi connectivity

### Milestone 3 — AI
- [ ] Gemini Live session verified
- [ ] Gemini audio input and output verified
- [ ] End-to-end voice conversation

### Milestone 4 — Personality
- [ ] Robot personality
- [ ] Emotional states
- [ ] Animations synchronized with conversation
- [ ] Wake word

### Milestone 5 — Assistant
- [ ] Tool calling
- [ ] Weather
- [ ] Calendar
- [ ] Reminders
- [ ] Web search
- [ ] Home automation

### Milestone 6 — Final Hardware
- [ ] Final component selection
- [ ] Custom PCB
- [ ] Battery system
- [ ] 3D body prototype
- [ ] Acoustic testing
- [ ] Final enclosure
- [ ] Final assembled robot
