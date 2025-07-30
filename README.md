# CommandLock – MJPEG Video on SPI Display with ESP32

This project allows you to play a short MJPEG video on a 1.8" SPI TFT display (128x160 pixels) using an ESP32 microcontroller. It's ideal for visual effects, interactive demos, or embedded prototypes.

---

## 🧰 Required Hardware

- ESP32 (tested with ESP32-S3)
- 1.8" SPI TFT Display (ST7735 controller, 128x160 resolution)
- Wiring based on this guide:  
  https://zonnepanelen.wouterlood.com/31-1-8-inch-128160-pixel-spi-tft-wiring-to-an-esp32-microcontroller/

---

## 🧪 Software Dependencies

- Arduino IDE (or a compatible environment)
- Arduino libraries:
  - Adafruit_GFX
  - Adafruit_ST7735
  - SPI
  - Any others required by `MjpegClass.h`

---

## 🎞️ Video Conversion with ffmpeg

You can convert any `.mp4` video into an `.mjpeg` file compatible with the display using this command:

```
ffmpeg -i VID-20250717-WA0019.mp4 -t 5 -vcodec mjpeg -an -s 320x220 -aspect 1:1 -q:v 5 data/output_short_2.mjpeg
```

### Parameter Breakdown

- `-i`: input video file
- `-t 5`: set duration to 5 seconds
- `-vcodec mjpeg`: encode using MJPEG
- `-an`: disable audio
- `-s 320x220`: output resolution (can be resized later)
- `-aspect 1:1`: force square aspect ratio
- `-q:v 5`: quality setting (lower means better; 2 = high, 31 = low)

> You can later resize the output to 128x160 to match the screen exactly.

---

## 📤 Upload & Playback

1. Upload the `.mjpeg` file to SPIFFS or an SD card (depending on your sketch).
2. Open and upload `Video.ino` to your ESP32 using the Arduino IDE.
3. The video will play automatically on boot.

---

## 📁 Project Structure

```
CommandLock/
├── MjpegClass.h                      # MJPEG decoder class
├── PINS_ESP32-S3-LCD-ST7735_1_8.h    # Display pin configuration
├── Video.ino                         # Main Arduino sketch
├── README.md                         # This documentation
└── .gitignore                        # Git ignore rules
```

---

## 🧠 Notes

- Performance depends on the ESP32 model and storage method.
- Lowering resolution or video length can improve playback.
- Ensure the display is correctly wired and initialized in code.

---

## 🛠️ Credits

Wiring reference:  
https://zonnepanelen.wouterlood.com/31-1-8-inch-128160-pixel-spi-tft-wiring-to-an-esp32-microcontroller/

---

Feel free to fork, modify, and contribute!
