#include "PINS_ESP32-S3-LCD-ST7735_1_8.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <JPEGDEC.h>
#include "MjpegClass.h"
#include "DFRobotDFPlayerMini.h"

// --- MJPEG File Paths ---
#define MJPEG_VIDEO_NORMAL   "/output_2.mjpeg"
#define MJPEG_VIDEO_SHORT    "/output_short_3.mjpeg"
#define MJPEG_VIDEO_SHORT_2  "/output_short_4.mjpeg"

// --- Shared pin for video and audio trigger ---
#define BUTTON_PIN 45

// --- DFPlayer Mini setup ---
HardwareSerial mySerial(1); // UART1 (TX=10, RX=9)
DFRobotDFPlayerMini player;

// --- Button state ---
bool lastButtonState = HIGH;
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// --- MJPEG player ---
MjpegClass mjpeg;
File mjpegFile;
uint8_t* mjpeg_buf;
bool videoPlaying = false;

// --- List of videos ---
const char* videoList[] = {
  MJPEG_VIDEO_NORMAL,
  MJPEG_VIDEO_SHORT,
  MJPEG_VIDEO_SHORT_2
};

int currentVideoIndex = 0;
int specialIndex = 1;  // Alternates between short video 1 and 2

// --- MJPEG frame rate (25 FPS) ---
unsigned long lastFrameTime = 0;
const unsigned long frameInterval = 1000 / 25;

// --- JPEG draw callback ---
int jpegDrawCallback(JPEGDRAW* pDraw) {
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

// --- Start a video ---
void startVideo(const char* filename) {
  if (mjpegFile) mjpegFile.close();

  mjpegFile = LittleFS.open(filename);
  if (!mjpegFile || mjpegFile.isDirectory() || mjpegFile.size() < 1024) {
    Serial.printf("⚠️ Error opening file: %s\n", filename);
    return;
  }

  mjpeg.setup(&mjpegFile, mjpeg_buf, jpegDrawCallback, true, 0, 0, gfx->width(), gfx->height());
  mjpeg.resetScale();
  videoPlaying = true;
  lastFrameTime = millis();
  Serial.printf("▶️ Starting video: %s\n", filename);
}

// --- Stop the video ---
void stopVideo() {
  if (mjpegFile) mjpegFile.close();
  videoPlaying = false;
}

// --- Initialize the display ---
void initDisplay() {
  if (!gfx->begin()) {
    Serial.println("❌ Display error");
    while (1) delay(100);
  }
  gfx->fillScreen(BLACK);
  Serial.println("✅ Display ready");
}

// --- Initialize LittleFS filesystem ---
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("❌ LittleFS initialization failed");
    while (1) delay(100);
  }
  Serial.println("✅ LittleFS ready");
}

// --- Initialize DFPlayer Mini ---
void initDFPlayer() {
  mySerial.begin(9600, SERIAL_8N1, 9, 10); // RX=9, TX=10
  if (player.begin(mySerial)) {
    Serial.println("🎵 DFPlayer ready");
    player.volume(25); // Volume from 0 to 30
    // No track plays at startup
  } else {
    Serial.println("❌ DFPlayer not detected");
    while (true); // Halt execution
  }
}

// --- Setup function ---
void setup() {
  Serial.begin(115200);
  delay(300);  // Allow power stabilization

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  initDisplay();
  initFS();
  initDFPlayer();

  mjpeg_buf = (uint8_t*)heap_caps_malloc(40 * 1024, MALLOC_CAP_8BIT);
  if (!mjpeg_buf) {
    Serial.println("❌ Failed to allocate MJPEG buffer");
    while (1) delay(100);
  }

  startVideo(videoList[currentVideoIndex]);  // Start the default video WITHOUT audio
}

// --- Main loop ---
void loop() {
  // --- Button handling with debounce ---
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && !buttonPressed) {
      buttonPressed = true;

      // Switch to short video
      currentVideoIndex = specialIndex;
      startVideo(videoList[currentVideoIndex]);
      specialIndex = (specialIndex == 1) ? 2 : 1;

      // Play audio ONLY if it's a short video
      if (currentVideoIndex != 0) {
        player.next();
        Serial.println("🎶 Audio started with short video");
      }
    }

    if (reading == HIGH) {
      buttonPressed = false;
    }
  }

  lastButtonState = reading;

  // --- MJPEG playback ---
  if (videoPlaying) {
    unsigned long now = millis();
    if (now - lastFrameTime >= frameInterval) {
      if (mjpegFile.available() && mjpeg.readMjpegBuf()) {
        mjpeg.drawJpg();
        lastFrameTime = now;
        delay(1);  // Friendly to watchdog
      } else {
        stopVideo();
        if (currentVideoIndex != 0) {
          // After short video, return to main video WITHOUT audio
          currentVideoIndex = 0;
          startVideo(videoList[currentVideoIndex]);
        }
      }
    }
  }

  delay(1);  // Watchdog-friendly idle
}
