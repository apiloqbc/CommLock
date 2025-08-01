# CommandLock – MJPEG Video on SPI Display with ESP32

This project allows you to play short MJPEG videos on a 1.8" SPI TFT display (128x160 pixels) using an ESP32 microcontroller. It's ideal for creating visual effects, interactive installations, or low-cost media displays.

---

## 🧰 Required Hardware

- ESP32 (tested with ESP32-S3)
- 1.8" SPI TFT Display with ST7735 controller (128x160 resolution)
- Pushbuttons (up to 9)
- Power source (USB or regulated 3.3V supply)
- Optional: microSD card (if not using SPIFFS)

Wiring guide reference:  
https://zonnepanelen.wouterlood.com/31-1-8-inch-128160-pixel-spi-tft-wiring-to-an-esp32-microcontroller/

---

## 🔌 ESP32S3 Pinout (Dev Module)

| Left Side     | Right Side  |
|---------------|-------------|
| 3V3           | GND         |
| 3V3           | TX          |
| RST           | RX          |
| 4             | 1           |
| 5             | 2           |
| 6             | 42          |
| 7             | 41          |
| 15            | 40          |
| 16            | 39          |
| 17            | 38          |
| 18            | 37          |
| 8             | 36          |
| 3             | 35          |
| 46            | 0           |
| 9             | 45          |
| 10            | 48          |
| 11            | 47          |
| 12            | 21          |
| 13            | 20          |
| 14            | 19          |
| 5Vin          | GND         |
| GND           | GND         |

<img src="images/pins.jpg" alt="Pinout" width="400"/>
<img src="images/board.jpg" alt="Board" width="300"/>


## 🔌 Display Pinout (ESP32 ↔ TFT ST7735)

| TFT Pin     | ESP32 Pin | Function           |
|-------------|------------|--------------------|
| VCC         | 3.3V       | Power              |
| GND         | GND        | Ground             |
| CS          | GPIO 5     | Chip Select        |
| RESET       | GPIO 18    | Reset              |
| DC / RS     | GPIO 16    | Data/Command Select|
| MOSI (SDA)  | GPIO 11    | SPI Data           |
| SCK         | GPIO 12    | SPI Clock          |
| LED (BL)    | 3.3V       | Backlight (via resistor or directly) |

> You can modify these pin assignments in the file `PINS_ESP32-S3-LCD-ST7735_1_8.h`.

---

## 🎞️ Video Conversion with ffmpeg

To prepare videos for playback, convert any `.mp4` file to `.mjpeg` format using this command:

```
ffmpeg -i input.mp4 -t 5 -vcodec mjpeg -an -s 320x220 -aspect 1:1 -q:v 5 output.mjpeg
```

### Explanation of Parameters

- `-i`: Input video file
- `-t 5`: Duration of 5 seconds
- `-vcodec mjpeg`: Use MJPEG codec
- `-an`: Remove audio
- `-s 320x220`: Output resolution
- `-aspect 1:1`: Force 1:1 aspect ratio
- `-q:v 5`: Quality (2 = high, 31 = low)

> Resize output to 128x160 to match display resolution if needed.

---

## 🎬 Video Selection via Buttons

This project supports up to 9 buttons to trigger different video files.

Each button is wired to a specific GPIO pin on the ESP32 and mapped to a corresponding video file.

| Button | GPIO Pin | Plays Video File    |
|--------|----------|---------------------|
| 1      | 14       | `/video1.mjpeg`     |
| 2      | 15       | `/video2.mjpeg`     |
| 3      | 16       | `/video3.mjpeg`     |
| 4      | 17       | `/video4.mjpeg`     |
| 5      | 19       | `/video5.mjpeg`     |
| 6      | 20       | `/video6.mjpeg`     |
| 7      | 21       | `/video7.mjpeg`     |
| 8      | 22       | `/video8.mjpeg`     |
| 9      | 48       | `/video9.mjpeg`     |

> Buttons must pull the GPIO pin LOW when pressed. Use internal pull-ups or external resistors as needed.

---

## 📤 Upload & Playback

1. Store your `.mjpeg` files in the correct location (SPIFFS or SD card).
2. Open `Video.ino` in Arduino IDE.
3. Ensure libraries like Adafruit_ST7735, Adafruit_GFX, and SPI are installed.
4. Compile and upload the sketch to your ESP32.
5. On startup, the display initializes and waits for button input to play videos.

---

## 📁 Project Structure

```
CommandLock/
├── MjpegClass.h                      # MJPEG decoding class
├── PINS_ESP32-S3-LCD-ST7735_1_8.h    # Pin mapping for ESP32 and display
├── Video.ino                         # Main Arduino sketch
├── README.md                         # This documentation
└── .gitignore                        # Git ignore rules
```

---

## 🧠 Notes

- Video files should match the filenames expected in the sketch.
- Keep video length short and resolution low for smooth playback.
- SPIFFS has size limitations; use SD for longer videos.
- You can customize the sketch to add more controls (serial, web, etc.).

---

## 🛠️ Credits

Display wiring guide by Wouterlood:  
https://zonnepanelen.wouterlood.com/31-1-8-inch-128160-pixel-spi-tft-wiring-to-an-esp32-microcontroller/

---

Feel free to fork, customize, and build upon this project!
