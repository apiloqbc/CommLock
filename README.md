# CommandLock – MJPEG Video on SPI Display with ESP32

This project allows you to play a short MJPEG video on a 1.8" SPI TFT display (128x160 pixels) using an ESP32 microcontroller. It's ideal for visual effects, interactive demos, or embedded prototypes.

---

## 🧰 Required Hardware

- ESP32 (tested with ESP32-S3)
- 1.8" SPI TFT Display (ST7735 controller, 128x160 resolution)
- Wiring based on this guide:  
  🔗 https://zonnepanelen.wouterlood.com/31-1-8-inch-128160-pixel-spi-tft-wiring-to-an-esp32-microcontroller/

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

```bash
ffmpeg -i VID-20250717-WA0019.mp4 -t 5 -vcodec mjpeg -an -s 320x220 -aspect 1:1 -q:v 5 data/output_short_2.mjpeg```
