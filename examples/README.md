# 📚 Examples

This folder contains example sketches demonstrating different aspects of the ESP32-S3 Video Player system.

## 🖥️ DisplayExample.ino

A simple example showing how to use the `DisplayManager` class for basic display operations.

### Features Demonstrated:
- Display initialization
- Text rendering (simple and centered)
- Color management
- Display information display
- Status checking

### Usage:
1. Open `DisplayExample.ino` in Arduino IDE
2. Upload to ESP32-S3
3. Watch the display show various text examples

## 🎵 AudioExample.ino

A comprehensive example demonstrating DFPlayer Mini usage for MP3 audio playback.

### Features Demonstrated:
- DFPlayer Mini initialization
- Audio track playback
- Button tone feedback
- Audio control (play, pause, stop, volume)
- Event handling for audio completion

### Usage:
1. Open `AudioExample.ino` in Arduino IDE
2. Connect DFPlayer Mini and buzzer
3. Prepare microSD card with MP3 files (0001.mp3, 0002.mp3, etc.)
4. Upload and test audio functionality

## 🔧 Hardware Requirements

### For DisplayExample.ino:
- ESP32-S3
- TFT Display (pre-soldered)

### For AudioExample.ino:
- ESP32-S3
- DFPlayer Mini module
- Buzzer (GPIO 8)
- MicroSD card with MP3 files
- Speakers

## 📖 Learning Path

1. **Start with DisplayExample.ino** - Learn basic display operations
2. **Move to AudioExample.ino** - Understand audio integration
3. **Study Video.ino** - See complete system integration

## 🎯 Use Cases

- **Learning:** Understand individual components
- **Testing:** Verify hardware connections
- **Development:** Use as starting point for custom projects
- **Troubleshooting:** Isolate component-specific issues 