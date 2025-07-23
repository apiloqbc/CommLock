#include "PINS_ESP32-S3-LCD-ST7789_1_9.h"

#define MJPEG_VIDEO_NORMAL   "/output.mjpeg"
#define MJPEG_VIDEO_SHORT    "/output_short.mjpeg"
#define MJPEG_VIDEO_SHORT_2  "/output_short_2.mjpeg"

#include <Arduino_GFX_Library.h>
#include <LittleFS.h>
#include <JPEGDEC.h>
#include "MjpegClass.h"

// --- MJPEG player ---
MjpegClass mjpeg;
uint8_t *mjpeg_buf;
File mjpegFile;
bool videoPlaying = false;

// --- Video list ---
const char *videoList[] = {
  MJPEG_VIDEO_NORMAL,
  MJPEG_VIDEO_SHORT,
  MJPEG_VIDEO_SHORT_2
};

int currentVideoIndex = 0;  // 0 = normale, 1 = short, 2 = short_2
int specialIndex = 1;       // alterna tra 1 e 2

// --- FPS control ---
unsigned long lastFrameTime = 0;
const unsigned long frameInterval = 1000 / 30;

// --- Debounce ---
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
bool lastButtonState = HIGH;

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

int jpegDrawCallback(JPEGDRAW *pDraw) {
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

void startVideo(const char *filename) {
  if (mjpegFile) mjpegFile.close();

  mjpegFile = LittleFS.open(filename);
  if (!mjpegFile || mjpegFile.isDirectory() || mjpegFile.size() < 1024) {
    Serial.printf("⚠️ Impossibile aprire %s\n", filename);
    return;
  }

  mjpeg.setup(&mjpegFile, mjpeg_buf, jpegDrawCallback, true, 0, 0, gfx->width(), gfx->height());
  mjpeg.resetScale();
  videoPlaying = true;
  lastFrameTime = millis();
  Serial.printf("▶️ Video avviato: %s\n", filename);
}

void stopVideo() {
  if (mjpegFile) mjpegFile.close();
  videoPlaying = false;
}

void initDisplay() {
  if (!gfx->begin()) {
    Serial.println("❌ Errore display");
    while (1);
  }
  gfx->fillScreen(BLACK);
}

void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("❌ Errore LittleFS");
    while (1);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  initDisplay();
  initFS();

  mjpeg_buf = (uint8_t *)heap_caps_malloc(40 * 1024, MALLOC_CAP_8BIT);
  if (!mjpeg_buf) {
    Serial.println("❌ MJPEG buffer non allocato");
    while (1);
  }

  currentVideoIndex = 0;
  startVideo(videoList[currentVideoIndex]);
}

void loop() {
  // --- Pulsante con debounce ---
  bool reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

if (reading == LOW && lastButtonState == HIGH) {
      // Cambia a video speciale
      currentVideoIndex = specialIndex;
      startVideo(videoList[currentVideoIndex]);

      // Alterna il prossimo video speciale
      specialIndex = (specialIndex == 1) ? 2 : 1;
    }
  

  lastButtonState = reading;

  // --- Riproduzione MJPEG ---
  if (videoPlaying) {
    unsigned long now = millis();
    if (now - lastFrameTime >= frameInterval) {
      if (mjpegFile.available() && mjpeg.readMjpegBuf()) {
        mjpeg.drawJpg();
        lastFrameTime = now;
      } else {
        stopVideo();

        // Se era un video speciale, torna al normale
        if (currentVideoIndex != 0) {
          currentVideoIndex = 0;
          startVideo(videoList[currentVideoIndex]);
        }
      }
    }
  }

  delay(1);
}
