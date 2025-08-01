# 🎬 MJPEG Video Player - ESP32-S3 + TFT Display + Audio + Commlock

An interactive MJPEG video playback system for ESP32-S3 with a pre-soldered and working 1.8" TFT display, synchronized MP3 audio, and a special Commlock system inspired by "1999: A Space Odyssey". Ideal for creating interactive installations, informational displays, or presentation systems.

## 📋 System Overview

![Complete System](images/wiring_diagram.svg)

*Complete wiring diagram showing all components and connections*

## ✨ Features

- **MJPEG Video Playback** on 1.8" TFT display (128x160 pixels)
- **Synchronized MP3 Audio** via DFPlayer Mini module
- **Interactive Control** via physical buttons (up to 9 videos)
- **Commlock System** - Special display inspired by "1999: A Space Odyssey"
- **Configurable LED System** - LED activation for specific videos with customizable timing
- **Button Tone Feedback** when buttons are pressed
- **Simplified Display Management** through `DisplayManager` class
- **State System** (splash, home, menu, playback, commlock)
- **Integrated Error Handling** with recovery mechanisms
- **Button Debouncing** for reliable control
- **Professional Logging System** with multiple levels and categories

## 🧰 Required Hardware

### Main Components

![ESP32-S3 Board](images/board.jpg)

- **ESP32-S3** (tested with ESP32-S3-DevKitC-1)
- **1.8" TFT ST7735 Display** (pre-soldered and working)
- **DFPlayer Mini MP3 Module** (TF-16P)
- **Pushbuttons** (up to 9 for video selection)
- **Buzzer** (for button tone feedback)
- **LED** (for video-specific feedback)
- **Power Supply** USB or regulated 3.3V
- **MicroSD Card** (for MP3 files)

### Pin Layout Reference

![ESP32-S3 Pinout](images/pins.jpg)

*ESP32-S3 pin layout for reference*

## 🔌 ESP32-S3 Pinout (Pre-soldered Display + Audio + LED)

### Complete ESP32-S3 Pin Mapping

| **Pin** | **GPIO** | **Function** | **Status** | **Usage** |
|---------|----------|--------------|------------|-----------|
| **3V3** | - | 3.3V Power Supply | Power | Display, DFPlayer |
| **GND** | - | Ground | Ground | All components |
| **EN** | - | Enable | System | Boot system |
| **GPIO 0** | 0 | Boot Mode | Available | - |
| **GPIO 1** | 1 | U0TXD | Available | - |
| **GPIO 2** | 2 | Display DC/A0 | **Used** | Display Data/Command |
| **GPIO 3** | 3 | U0RXD | Available | - |
| **GPIO 4** | 4 | Display RESET | **Used** | Display Reset |
| **GPIO 5** | 5 | Menu Button | **Used** | Menu Button |
| **GPIO 6** | 6 | Flash SCK | Available | - |
| **GPIO 7** | 7 | Flash SDA | Available | - |
| **GPIO 8** | 8 | LED + Buzzer | **Used** | LED and Buzzer (shared) |
| **GPIO 9** | 9 | DFPlayer RX | **Used** | Serial2 RX |
| **GPIO 10** | 10 | DFPlayer TX | **Used** | Serial2 TX |
| **GPIO 11** | 11 | Flash WP | Available | - |
| **GPIO 12** | 12 | Flash HD | Available | - |
| **GPIO 13** | 13 | Display SDA/MOSI | **Used** | SPI MOSI |
| **GPIO 14** | 14 | Display SCK | **Used** | SPI SCK |
| **GPIO 15** | 15 | Display CS | **Used** | SPI CS |
| **GPIO 16** | 16 | Button 3 | **Used** | Video Button 3 |
| **GPIO 17** | 17 | Button 4 | **Used** | Video Button 4 |
| **GPIO 18** | 18 | - | Available | - |
| **GPIO 19** | 19 | Button 5 | **Used** | Video Button 5 |
| **GPIO 20** | 20 | Button 6 | **Used** | Video Button 6 |
| **GPIO 21** | 21 | Button 7 | **Used** | Video Button 7 |
| **GPIO 22** | 22 | - | Available | - |
| **GPIO 23** | 23 | - | Available | - |
| **GPIO 24** | 24 | - | Available | - |
| **GPIO 25** | 25 | - | Available | - |
| **GPIO 26** | 26 | - | Available | - |
| **GPIO 27** | 27 | - | Available | - |
| **GPIO 28** | 28 | - | Available | - |
| **GPIO 29** | 29 | - | Available | - |
| **GPIO 30** | 30 | - | Available | - |
| **GPIO 31** | 31 | - | Available | - |
| **GPIO 32** | 32 | - | Available | - |
| **GPIO 33** | 33 | - | Available | - |
| **GPIO 34** | 34 | - | Available | - |
| **GPIO 35** | 35 | Button 2 | **Used** | Video Button 2 |
| **GPIO 36** | 36 | - | Available | - |
| **GPIO 37** | 37 | - | Available | - |
| **GPIO 38** | 38 | - | Available | - |
| **GPIO 39** | 39 | - | Available | - |
| **GPIO 40** | 40 | - | Available | - |
| **GPIO 41** | 41 | - | Available | - |
| **GPIO 42** | 42 | - | Available | - |
| **GPIO 43** | 43 | - | Available | - |
| **GPIO 44** | 44 | - | Available | - |
| **GPIO 45** | 45 | - | Available | - |
| **GPIO 46** | 46 | Button 1 | **Used** | Video Button 1 |
| **GPIO 47** | 47 | Button 8 | **Used** | Video Button 8 |
| **GPIO 48** | 48 | Button 9 | **Used** | Video Button 9 + Commlock |

### Pin Summary:
- **🟢 Used**: 18 pins (40%)
- **🟡 Available**: 27 pins (60%)
- **🔴 Reserved**: 0 pins

### 🎯 Available Pin Suggestions

#### **Recommended Pins for Expansions:**

**Communication:**
- **GPIO 0, 1, 3** - Additional serial communication
- **GPIO 6, 7, 11, 12** - Additional SPI/I2C interfaces

**Analog Controls:**
- **GPIO 18, 19** - PWM for analog controls
- **GPIO 22-45** - General input/output

**Sensors and Actuators:**
- **GPIO 22-45** - Temperature, motion, light sensors
- **GPIO 18, 19** - Servomotors, speed controls

**Future Expansions:**
- **Extra Buttons** - Any free GPIO
- **Status LEDs** - Any free GPIO
- **Wireless Communication** - GPIO 0, 1, 3
- **Additional Interfaces** - GPIO 6, 7, 11, 12

### 📍 Physical Pin Layout (ESP32-S3 Board)

| **Left Side** | **GPIO** | **Function** | **Status** | **Right Side** | **GPIO** | **Function** | **Status** |
|---------------|----------|--------------|------------|----------------|----------|--------------|------------|
| **3V3** | - | Power Supply | Power | **GND** | - | Ground | Ground |
| **3V3** | - | Power Supply | Power | **TX** | 1 | U0TXD | Available |
| **RST** | - | Reset | System | **RX** | 3 | U0RXD | Available |
| **4** | 4 | Display RESET | **Used** | **1** | 1 | U0TXD | Available |
| **5** | 5 | Menu Button | **Used** | **2** | 2 | Display DC/A0 | **Used** |
| **6** | 6 | Flash SCK | Available | **42** | 42 | - | Available |
| **7** | 7 | Flash SDA | Available | **41** | 41 | - | Available |
| **15** | 15 | Display CS | **Used** | **40** | 40 | - | Available |
| **16** | 16 | Button 3 | **Used** | **39** | 39 | - | Available |
| **17** | 17 | Button 4 | **Used** | **38** | 38 | - | Available |
| **18** | 18 | - | Available | **37** | 37 | - | Available |
| **8** | 8 | LED + Buzzer | **Used** | **36** | 36 | - | Available |
| **3** | 3 | U0RXD | Available | **35** | 35 | Button 2 | **Used** |
| **46** | 46 | Button 1 | **Used** | **0** | 0 | Boot Mode | Available |
| **9** | 9 | DFPlayer RX | **Used** | **45** | 45 | - | Available |
| **10** | 10 | DFPlayer TX | **Used** | **48** | 48 | Button 9 | **Used** |
| **11** | 11 | Flash WP | Available | **47** | 47 | Button 8 | **Used** |
| **12** | 12 | Flash HD | Available | **21** | 21 | Button 7 | **Used** |
| **13** | 13 | Display SDA/MOSI | **Used** | **20** | 20 | Button 6 | **Used** |
| **14** | 14 | Display SCK | **Used** | **19** | 19 | Button 5 | **Used** |
| **5Vin** | - | 5V Power Supply | Power | **GND** | - | Ground | Ground |
| **GND** | - | Ground | Ground | **GND** | - | Ground | Ground |

### ⚡ Important Pin Notes

#### **Power Pins:**
- **3V3** - 3.3V power supply for display and DFPlayer
- **5Vin** - 5V power supply (not used in this project)
- **GND** - Common ground for all components

#### **Special Pins:**
- **RST** - Board reset (not used in code)
- **TX/RX** - Serial0 (GPIO 1, 3) - available for debugging
- **GPIO 0** - Boot mode (available after startup)

#### **Flash Pins (Unused):**
- **GPIO 6, 7, 11, 12** - Connected to internal flash
- **Available** for general use (don't interfere with flash)

### 🔧 Practical Wiring Guide

#### **Main Connections:**
1. **Display** - Pre-soldered, no wiring required
2. **DFPlayer** - Connect RX→GPIO 9, TX→GPIO 10
3. **Buttons** - Connect to GPIO pins specified in table
4. **LED/Buzzer** - Connect to GPIO 8 (shared)

#### **Recommended Wiring Order:**
1. **Power** - 3V3 and GND
2. **Display** - Verify operation
3. **DFPlayer** - Test audio
4. **Buttons** - Test interaction
5. **LED/Buzzer** - Test feedback

### Simplified Wiring Diagram

*Simplified wiring diagram for quick reference*

### Detailed Connections

#### Display Connections
| Display Pin | ESP32 Pin | Function |
|-------------|-----------|----------|
| VCC         | 3.3V      | Power |
| GND         | GND       | Ground |
| CS          | GPIO 15   | Chip Select |
| RESET       | GPIO 4    | Reset |
| DC/RS       | GPIO 2    | Data/Command |
| MOSI (SDA)  | GPIO 13   | SPI Data |
| SCK         | GPIO 14   | SPI Clock |
| LED (BL)    | 3.3V      | Backlight |

#### DFPlayer Mini Connections
| DFPlayer Pin | ESP32 Pin | Function |
|--------------|-----------|----------|
| VCC          | 3.3V      | Power |
| GND          | GND       | Ground |
| RX           | GPIO 9    | Data In |
| TX           | GPIO 10   | Data Out |
| SPK_1        | Speaker + | Audio Output |
| SPK_2        | Speaker - | Audio Output |

#### Buzzer Connection
| Buzzer Pin | ESP32 Pin | Function |
|------------|-----------|----------|
| +          | GPIO 8    | Tone Output |
| -          | GND       | Ground |

#### LED Connection
| LED Pin | ESP32 Pin | Function |
|---------|-----------|----------|
| +       | GPIO 8    | LED Control |
| -       | GND       | Ground |

> **Note:** The display is pre-soldered and working. Pin configuration is in `PINS_ESP32-S3-LCD-ST7735_1_8.h`.

## 🎮 Button Control

The system supports up to 9 buttons for video selection:

| Button | GPIO Pin | Video File | Audio File | LED Behavior |
|--------|----------|------------|------------|--------------|
| 1      | 46       | `video_1.mjpeg` | `0001.mp3` | 5s duration, 500ms interval |
| 2      | 35       | `video_2.mjpeg` | `0002.mp3` | 3s duration, 300ms interval |
| 3      | 16       | `video_3.mjpeg` | `0003.mp3` | 4s duration, 400ms interval |
| 4      | 17       | `video_4.mjpeg` | `0004.mp3` | 6s duration, 600ms interval |
| 5      | 19       | `video_5.mjpeg` | `0005.mp3` | 2s duration, 200ms interval |
| 6      | 20       | `video_6.mjpeg` | `0006.mp3` | 7s duration, 700ms interval |
| 7      | 21       | `video_7.mjpeg` | `0007.mp3` | 3.5s duration, 350ms interval |
| 8      | 47       | `video_8.mjpeg` | `0008.mp3` | 4.5s duration, 450ms interval |
| 9      | 48       | `video_9.mjpeg` | `0009.mp3` | **Commlock System** |
| **Menu** | **5** | **Return to menu** | **No audio** | **No LED** |

> **Note:** Home video and menu have no audio. Button presses trigger a tone feedback. LED behavior is configurable in `LEDConfig.h`.

## 🚀 Commlock System - Level 1 Enhanced

### Special Feature: Authentic Commlock Display

**Button 9** activates a professional Commlock display inspired by "1999: A Space Odyssey":

- **Real-time Lunar Time Display** with seconds (HH:MM:SS)
- **Authentic Audio Feedback** with startup beep, ticks, and confirmations
- **Advanced Visual Effects** with CRT scan lines and glow effects
- **Lunar Day Progress** calculation and display
- **15-second duration** (configurable)
- **Professional Space Interface** with authentic colors

### 🎬 About Commlock - "1999: A Space Odyssey" Reference

#### **What is Commlock?**

**Commlock** is a fictional computer interface system featured in Stanley Kubrick's iconic 1968 film "2001: A Space Odyssey" (often referred to as "1999" in the context of the film's timeline). In the movie, Commlock serves as the primary computer interface aboard the Discovery One spacecraft, used by astronauts to communicate with the ship's AI system, HAL 9000.

#### **Film Context:**
- **Movie**: "2001: A Space Odyssey" (1968)
- **Director**: Stanley Kubrick
- **Setting**: Discovery One spacecraft en route to Jupiter
- **Time Period**: 1999-2001 (in the film's timeline)
- **Purpose**: Computer interface for spacecraft operations

#### **Commlock Features in the Film:**
- **Lunar Time Display**: Shows time in a lunar/space format
- **Deep Blue Interface**: Authentic space-age computer aesthetic
- **Audio Feedback**: Beeps and tones for user interaction
- **Professional Styling**: Clean, futuristic computer interface
- **Space Mission Context**: Used for spacecraft navigation and communication

#### **Our Implementation:**
This project recreates the authentic Commlock experience with:

- **🎵 Authentic Audio**: Startup beeps, tick sounds, and confirmation tones
- **🌙 Lunar Time**: Real-time display with lunar day progress calculation
- **🎨 Visual Effects**: CRT scan lines and glow effects for authentic feel
- **🚀 Space Interface**: Professional styling inspired by the film
- **⚡ Interactive**: Button 9 activates the full Commlock experience

### 🔧 How Commlock Works

#### **Activation:**
- **Trigger**: Press Button 9 (GPIO 48)
- **Duration**: 15 seconds (configurable)
- **Exit**: Any button press or automatic timeout

#### **Audio System:**
```cpp
// Authentic Commlock sounds
#define COMMLOCK_BEEP_FREQ 1200      // Startup beep
#define COMMLOCK_TICK_FREQ 800       // Second tick
#define COMMLOCK_CONFIRM_FREQ 1500   // Button confirmation
```

#### **Visual Effects:**
- **CRT Scan Lines**: Subtle blue scan lines every 100ms
- **Glow Effects**: Blue glow around display edges
- **Color Scheme**: Authentic space-age blue and green
- **Typography**: Professional computer interface styling

#### **Lunar Time Calculation:**
```cpp
// Real-time lunar time with seconds
commlockHour = random(0, 24);    // Random start time
commlockMinute = random(0, 60);
commlockSecond = 0;              // Real-time seconds
lunarDayProgress = 0.0f;         // Lunar day percentage
```

#### **Technical Implementation:**
- **State Management**: Dedicated `STATE_COMMLOCK` state
- **Time Updates**: Real-time seconds with lunar day progress
- **Audio Integration**: Synchronized with visual effects
- **Performance**: Optimized for smooth 30 FPS display

### Commlock Level 1 Features:
- ✅ **Authentic Audio System**
  - Startup beep (1200Hz, 200ms)
  - Tick sound every second (800Hz, 50ms)
  - Confirmation sounds for interactions (1500Hz, 150ms)
- ✅ **Realistic Lunar Time**
  - Real-time seconds display
  - Accurate lunar day progress calculation
  - Authentic time formatting
- ✅ **Advanced Visual Effects**
  - CRT scan line effects
  - Subtle glow effects around edges
  - Authentic Commlock color scheme
  - Professional visual styling
- ✅ **Enhanced Interface**
  - Larger time display with seconds
  - Lunar day progress percentage
  - Professional Commlock styling
  - Authentic 1999: A Space Odyssey theme

## 🔴 LED System

### Configurable LED Behavior

Each video can trigger a specific LED behavior:

- **Duration**: How long the LED blinks (milliseconds)
- **Interval**: Time between on/off states (milliseconds)
- **Enable/Disable**: Per-video LED activation
- **Automatic Activation**: LED starts when video begins

### Configuration

Edit `LEDConfig.h` to customize LED behavior:

```cpp
// Example configurations:
{0, 5000, 500, true},   // Video 1: 5s duration, 500ms interval
{1, 3000, 300, true},   // Video 2: 3s duration, 300ms interval
{2, 4000, 400, false},  // Video 3: LED disabled
```

### LED Examples:
- **Fast Alert**: `{0, 3000, 100, true}` - 3s, 100ms interval
- **Slow Ambient**: `{1, 10000, 1000, true}` - 10s, 1s interval
- **Continuous**: `{3, 15000, 200, true}` - 15s, 200ms interval
- **Disabled**: `{2, 5000, 500, false}` - LED off for video 3

## 🎵 Commlock Audio System

### Authentic Sound Design

The Commlock system features professional audio feedback:

#### Audio Frequencies:
- **Startup Beep**: 1200Hz, 200ms - Activates when Commlock starts
- **Tick Sound**: 800Hz, 50ms - Plays every second for authentic feel
- **Confirmation**: 1500Hz, 150ms - Plays when buttons are pressed

#### Audio Implementation:
```cpp
// Audio functions in Video.ino
void playCommlockBeep();     // Startup sound
void playCommlockTick();     // Second tick
void playCommlockConfirm();  // Button confirmation
```

## 🌙 Lunar Time System

### Realistic Lunar Calculations

The Commlock displays authentic lunar time:

#### Time Features:
- **Real-time Seconds**: Updates every second (HH:MM:SS)
- **Lunar Day Progress**: Shows percentage of lunar day completed
- **Accurate Calculations**: Based on actual lunar day duration

#### Technical Implementation:
```cpp
// Lunar time constants
#define LUNAR_DAY_HOURS 24
#define LUNAR_DAY_MINUTES (LUNAR_DAY_HOURS * 60)
#define LUNAR_DAY_SECONDS (LUNAR_DAY_MINUTES * 60)
#define LUNAR_DAY_MILLIS (LUNAR_DAY_SECONDS * 1000L)

// Time update functions
void updateLunarTimeRealistic();
void calculateLunarDayProgress();
```

## 🎨 Visual Effects System

### Advanced Commlock Interface

Professional visual effects for authentic experience:

#### Visual Features:
- **CRT Scan Lines**: Subtle blue scan lines every 100ms
- **Glow Effects**: Subtle blue glow around display edges
- **Authentic Colors**: Professional Commlock color scheme
- **Enhanced Typography**: Larger time display with seconds

#### Color Scheme:
- **Commlock Blue**: RGB(0, 150, 255) - Primary interface color
- **Commlock Green**: RGB(0, 255, 100) - Secondary elements
- **Commlock Orange**: RGB(255, 150, 0) - Progress indicators

#### Technical Implementation:
```cpp
// Visual effect functions
void drawCommlockAdvanced();
void drawCommlockCRTEffect();
void drawCommlockGlowEffect();
void updateCommlockVisualEffects();
```

## 📚 Required Libraries

Install these libraries via Arduino IDE:

1. **Arduino_GFX_Library** - Display graphics management
2. **LittleFS** - File system (included with ESP32)
3. **JPEGDEC** - JPEG decoding
4. **DFRobotDFPlayerMini** - MP3 audio playback
5. **SoftwareSerial** - Serial communication (included with ESP32)

## 📁 Project Structure

```
Video/
├── Video.ino                    # Main sketch
├── DisplayManager.h             # Display management class
├── PINS_ESP32-S3-LCD-ST7735_1_8.h  # Pin configuration
├── MjpegClass.h                # MJPEG decoding class
├── README.md                   # Documentation
├── examples/                   # Example sketches
│   ├── DisplayExample.ino      # Display usage example
│   ├── AudioExample.ino        # Audio usage example
│   └── README.md              # Examples documentation
├── data/                       # MJPEG video files
│   ├── home.mjpeg             # Home video (no audio)
│   ├── video_1.mjpeg          # Video 1
│   └── ...                     # Other videos
└── images/                     # Documentation images
    ├── board.jpg
    ├── pins.jpg
    ├── wiring_diagram.svg      # Complete wiring diagram
    └── wiring_diagram_simple.svg  # Simplified wiring diagram
```

## 🎞️ Video and Audio Preparation

### Video Conversion
Convert your videos to MJPEG format:

```bash
ffmpeg -i input.mp4 -t 5 -vcodec mjpeg -an -s 320x220 -aspect 1:1 -q:v 5 output.mjpeg
```

### Audio Preparation
1. Prepare MP3 files with the following naming convention:
   - `0001.mp3` for Video 1
   - `0002.mp3` for Video 2
   - `0003.mp3` for Video 3
   - ... and so on
2. Copy all MP3 files to the root of a microSD card
3. Insert the microSD card into the DFPlayer Mini module

### Parameter Explanation

- `-i input.mp4` - Input video file
- `-t 5` - 5 seconds duration
- `-vcodec mjpeg` - MJPEG codec
- `-an` - Remove audio
- `-s 320x220` - Output resolution
- `-aspect 1:1` - Force 1:1 aspect ratio
- `-q:v 5` - Quality (2=high, 31=low)

> **Tip:** Resize output to 128x160 to match display resolution.



## 🚀 Usage

### 1. File Preparation

1. Convert your videos to MJPEG format
2. Upload video files to the `data/` folder in the project
3. Prepare MP3 files with correct naming (0001.mp3, 0002.mp3, etc.)
4. Copy MP3 files to microSD card root directory
5. Insert microSD card into DFPlayer Mini module

### 2. Hardware Setup

1. Connect DFPlayer Mini to ESP32-S3:
   - RX (DFPlayer) → GPIO 9 (ESP32)
   - TX (DFPlayer) → GPIO 10 (ESP32)
   - VCC → 3.3V
   - GND → GND
2. Connect buzzer to GPIO 8 for button feedback
3. Connect speakers to DFPlayer Mini SPK_1 and SPK_2

### 3. Sketch Upload

1. Open `Video.ino` in Arduino IDE
2. Select ESP32-S3 board
3. Verify all libraries are installed
4. Compile and upload the sketch

### 4. System Operation

The system provides a complete multimedia experience:

- **🎬 Video Playback:** High-quality MJPEG videos on 1.8" TFT display
- **🎵 Audio Synchronization:** MP3 audio perfectly synchronized with video
- **🎮 Interactive Control:** 9 buttons for video selection + menu button
- **🔊 Audio Feedback:** Button tone feedback for user interaction
- **📱 State Management:** Smooth transitions between splash, home, menu, and video states

#### Operation Flow:
- **Startup:** Shows splash screen for 2 seconds
- **Home:** Automatically plays `home.mjpeg` (no audio)
- **Menu:** Press Menu button (GPIO 5) to access menu
- **Video Selection:** Press buttons 1-9 to play corresponding videos with audio
- **Button Feedback:** Each button press produces a tone
- **Exit:** Press Menu during playback to return to menu

## 🎯 System in Action

The complete system provides a professional multimedia experience with:

- **📺 Visual Display:** Crisp 128x160 TFT screen with vibrant colors
- **🎵 Audio Output:** High-quality stereo audio via DFPlayer Mini
- **🎮 User Interface:** Intuitive button control with tactile feedback
- **⚡ Performance:** Smooth 30 FPS video playback with synchronized audio
- **🔧 Reliability:** Robust error handling and state management

### Key Features Demonstrated:

1. **Synchronized Multimedia:** Video and audio start/stop together
2. **Interactive Feedback:** Button presses trigger immediate tone response
3. **Professional UI:** Clean menu system with centered text and colors
4. **Modular Design:** Easy to customize and extend functionality

## 🔧 DisplayManager Class

Display management has been simplified through the `DisplayManager` class, providing easy-to-use methods for display operations including text rendering, screen clearing, and JPEG frame drawing.

## 📚 Examples

The project includes example sketches in the `examples/` folder:

- **DisplayExample.ino** - Basic display operations and text rendering
- **AudioExample.ino** - DFPlayer Mini usage and audio control

See `examples/README.md` for detailed information about each example.

## 🎵 Audio Management

### DFPlayer Mini Functions

```cpp
// Initialize DFPlayer
void initializeDFPlayer() {
  mySerial.begin(9600, SERIAL_8N1, 9, 10);
  if (player.begin(mySerial)) {
    player.volume(25); // Volume 0-30
  }
}

// Play audio track
void startAudio(uint8_t trackNumber) {
  player.play(trackNumber); // Plays 0001.mp3, 0002.mp3, etc.
}

// Stop audio
void stopAudio() {
  player.stop();
}

// Button tone feedback
void playButtonTone() {
  tone(8, 800, 100); // 800Hz for 100ms
}
```

### Audio Features

- **Synchronized Playback:** Video and audio start together
- **Automatic Stop:** Audio stops when video ends
- **Volume Control:** Configurable volume (0-30)
- **Button Feedback:** Tone when buttons are pressed
- **Error Handling:** Graceful handling if DFPlayer is not connected

## 🎯 System States

1. **STATE_SPLASH** - Welcome screen
2. **STATE_HOME** - Home video playback (no audio)
3. **STATE_MENU** - Video selection menu
4. **STATE_PLAYING_VIDEO** - Video and audio playback
5. **STATE_ERROR** - Error handling

## 🛠️ Customization

### Modify Button Pins

```cpp
const uint8_t BUTTON_PINS[TOTAL_VIDEOS] = {
  46, 35, 16, 17, 19, 20, 21, 47, 48  // Modify here
};
```

### Add Videos with Audio

```cpp
const VideoInfo videoDatabase[TOTAL_VIDEOS] = {
  {"/video_1.mjpeg", "Video 1", 1},  // 0001.mp3
  {"/video_2.mjpeg", "Video 2", 2},  // 0002.mp3
  // Add here
};
```

### Audio Configuration

```cpp
#define BUTTON_TONE_FREQ 800        // Button tone frequency
#define BUTTON_TONE_DURATION 100    // Button tone duration (ms)
player.volume(25);                  // Audio volume (0-30)
```

### Modify Configuration

- **FPS:** `#define TARGET_FPS 30`
- **Buffer:** `#define MJPEG_BUFFER_SIZE (40 * 1024)`
- **Debounce:** `#define DEBOUNCE_DELAY 50`
- **Audio Volume:** `player.volume(25)`

## 🐛 Troubleshooting

### Display Not Working
- Check 3.3V power supply
- Verify SPI connections
- Check pin configuration in `PINS_ESP32-S3-LCD-ST7735_1_8.h`

### Audio Not Working
- Verify DFPlayer Mini connections (RX/TX swapped)
- Check microSD card format (FAT32)
- Ensure MP3 files are named correctly (0001.mp3, 0002.mp3, etc.)
- Check speaker connections
- Verify power supply (DFPlayer needs stable 3.3V)

### Video Not Loading
- Verify MJPEG format
- Check file size (max 1MB recommended)
- Verify memory space

### Buttons Not Responding
- Check connections (pull-up)
- Verify debounce delay
- Check GPIO pins
- Verify buzzer connection for tone feedback

## 📊 Technical Specifications

- **Display:** 1.8" TFT ST7735 (128x160)
- **Microcontroller:** ESP32-S3
- **Memory:** 8MB PSRAM
- **Video Format:** MJPEG
- **Audio Format:** MP3
- **Audio Module:** DFPlayer Mini TF-16P
- **FPS:** 30 (configurable)
- **Buttons:** 9 + Menu
- **Audio Output:** Stereo via DFPlayer Mini
- **Button Feedback:** Tone via buzzer

## 🏆 Project Showcase

This project demonstrates advanced ESP32-S3 capabilities:

- **🎬 Multimedia Integration:** Seamless video and audio synchronization
- **🎮 Interactive Design:** Professional user interface with tactile feedback
- **🔧 Modular Architecture:** Clean, maintainable code with DisplayManager class
- **📚 Comprehensive Documentation:** Complete wiring diagrams and setup guides
- **⚡ Performance Optimized:** Efficient memory usage and smooth playback

### Perfect For:
- **Interactive Installations:** Museums, exhibitions, public spaces
- **Educational Projects:** Learning ESP32, multimedia, and embedded systems
- **Prototyping:** Rapid development of multimedia interfaces
- **Custom Applications:** Easy to modify for specific use cases

## 🤝 Contributions

Feel free to fork, customize, and improve this project!

---

**Happy coding! 🚀**
