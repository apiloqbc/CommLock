# 🎬 MJPEG Video Player - ESP32-S3 + TFT Display

An interactive MJPEG video playback system for ESP32-S3 with a pre-soldered and working 1.8" TFT display. Ideal for creating interactive installations, informational displays, or presentation systems.

## ✨ Features

- **MJPEG Video Playback** on 1.8" TFT display (128x160 pixels)
- **Interactive Control** via physical buttons (up to 9 videos)
- **Simplified Display Management** through `DisplayManager` class
- **State System** (splash, home, menu, playback)
- **Integrated Error Handling**
- **Button Debouncing** for reliable control

## 🧰 Required Hardware

- **ESP32-S3** (tested with ESP32-S3-DevKitC-1)
- **1.8" TFT ST7735 Display** (pre-soldered and working)
- **Pushbuttons** (up to 9 for video selection)
- **Power Supply** USB or regulated 3.3V

## 🔌 ESP32-S3 Pinout (Pre-soldered Display)

The display is connected to the following pins:

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

> **Note:** The display is pre-soldered and working. Pin configuration is in `PINS_ESP32-S3-LCD-ST7735_1_8.h`.

## 🎮 Button Control

The system supports up to 9 buttons for video selection:

| Button | GPIO Pin | Video File |
|--------|----------|------------|
| 1      | 46       | `video_1.mjpeg` |
| 2      | 35       | `video_2.mjpeg` |
| 3      | 16       | `video_3.mjpeg` |
| 4      | 17       | `video_4.mjpeg` |
| 5      | 19       | `video_5.mjpeg` |
| 6      | 20       | `video_6.mjpeg` |
| 7      | 21       | `video_7.mjpeg` |
| 8      | 47       | `video_8.mjpeg` |
| 9      | 48       | `video_9.mjpeg` |
| **Menu** | **5** | **Return to menu** |

## 📚 Required Libraries

Install these libraries via Arduino IDE:

1. **Arduino_GFX_Library** - Display graphics management
2. **LittleFS** - File system (included with ESP32)
3. **JPEGDEC** - JPEG decoding

## 🎞️ Video Conversion

Convert your videos to MJPEG format:

```bash
ffmpeg -i input.mp4 -t 5 -vcodec mjpeg -an -s 320x220 -aspect 1:1 -q:v 5 output.mjpeg
```

### Parameter Explanation

- `-i input.mp4` - Input video file
- `-t 5` - 5 seconds duration
- `-vcodec mjpeg` - MJPEG codec
- `-an` - Remove audio
- `-s 320x220` - Output resolution
- `-aspect 1:1` - Force 1:1 aspect ratio
- `-q:v 5` - Quality (2=high, 31=low)

> **Tip:** Resize output to 128x160 to match display resolution.

## 📁 File Structure

```
Video/
├── Video.ino                    # Main sketch
├── DisplayManager.h             # Display management class
├── DisplayExample.ino           # Usage example
├── PINS_ESP32-S3-LCD-ST7735_1_8.h  # Pin configuration
├── MjpegClass.h                # MJPEG decoding class
├── README.md                   # Documentation
├── data/                       # MJPEG video files
│   ├── home.mjpeg             # Home video
│   ├── video_1.mjpeg          # Video 1
│   ├── video_2.mjpeg          # Video 2
│   └── ...                     # Other videos
└── images/                     # Documentation images
    ├── board.jpg
    └── pins.jpg
```

## 🚀 Usage

### 1. Video File Preparation

1. Convert your videos to MJPEG format
2. Upload files to the `data/` folder in the project
3. Ensure file names match those in the code

### 2. Sketch Upload

1. Open `Video.ino` in Arduino IDE
2. Select ESP32-S3 board
3. Verify all libraries are installed
4. Compile and upload the sketch

### 3. Operation

- **Startup:** Shows splash screen for 2 seconds
- **Home:** Automatically plays `home.mjpeg`
- **Menu:** Press Menu button (GPIO 5) to access menu
- **Video Selection:** Press buttons 1-9 to play corresponding videos
- **Exit:** Press Menu during playback to return to menu

## 🔧 DisplayManager Class

Display management has been simplified through the `DisplayManager` class:

```cpp
// Initialization
DisplayManager display;
display.begin();

// Basic operations
display.clear();
display.setTextColor(WHITE);
display.setTextSize(2);
display.setCursor(10, 20);
display.println("Text");

// Centered text
display.drawCenteredText("Centered", 50, 2);

// JPEG frame
display.drawJpegFrame(pDraw);
```

### Main Methods

- `begin()` - Initialize display
- `clear()` - Clear screen
- `drawCenteredText()` - Draw centered text
- `drawJpegFrame()` - Draw JPEG frame
- `width()/height()` - Display dimensions

## 🎯 System States

1. **STATE_SPLASH** - Welcome screen
2. **STATE_HOME** - Home video playback
3. **STATE_MENU** - Video selection menu
4. **STATE_PLAYING_VIDEO** - Video playback
5. **STATE_ERROR** - Error handling

## 🛠️ Customization

### Modify Button Pins

```cpp
const uint8_t BUTTON_PINS[TOTAL_VIDEOS] = {
  46, 35, 16, 17, 19, 20, 21, 47, 48  // Modify here
};
```

### Add Videos

```cpp
const VideoInfo videoDatabase[TOTAL_VIDEOS] = {
  {"/video_1.mjpeg", "Video 1"},
  {"/video_2.mjpeg", "Video 2"},
  // Add here
};
```

### Modify Configuration

- **FPS:** `#define TARGET_FPS 30`
- **Buffer:** `#define MJPEG_BUFFER_SIZE (40 * 1024)`
- **Debounce:** `#define DEBOUNCE_DELAY 50`

## 🐛 Troubleshooting

### Display Not Working
- Check 3.3V power supply
- Verify SPI connections
- Check pin configuration in `PINS_ESP32-S3-LCD-ST7735_1_8.h`

### Video Not Loading
- Verify MJPEG format
- Check file size (max 1MB recommended)
- Verify memory space

### Buttons Not Responding
- Check connections (pull-up)
- Verify debounce delay
- Check GPIO pins

## 📊 Technical Specifications

- **Display:** 1.8" TFT ST7735 (128x160)
- **Microcontroller:** ESP32-S3
- **Memory:** 8MB PSRAM
- **Video Format:** MJPEG
- **FPS:** 30 (configurable)
- **Buttons:** 9 + Menu

## 🤝 Contributions

Feel free to fork, customize, and improve this project!

---

**Happy coding! 🚀**
