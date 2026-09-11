# AI Desktop Robot — Project Checklist

## Hardware

### Core
- [ ] ESP32-S3 development board
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
- [ ] ESP32-S3 firmware project
- [ ] Wi-Fi connection
- [ ] Wi-Fi reconnection handling
- [ ] Device configuration
- [ ] Error handling
- [ ] OTA firmware updates

### OLED / robot face
- [ ] OLED initialization
- [ ] Basic text rendering
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
- [ ] I2S microphone input
- [ ] Audio buffering
- [ ] Recording start / stop
- [ ] Audio encoding
- [ ] Speaker output via I2S
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

### Backend
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
- [ ] Gemini integration
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
- [ ] ESP32-S3 running
- [ ] OLED working
- [ ] Microphone working
- [ ] Speaker working
- [ ] Button working
- [ ] RGB LED working

### Milestone 2 — Robot
- [ ] Animated eyes
- [ ] Audio recording
- [ ] Audio playback
- [ ] Wi-Fi connectivity

### Milestone 3 — AI
- [ ] Backend working
- [ ] STT working
- [ ] LLM working
- [ ] TTS working
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
