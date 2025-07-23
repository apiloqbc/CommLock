#include "PINS_ESP32-S3-LCD-ST7789_1_9.h"

#include <LittleFS.h>
#include <JPEGDEC.h>
#include "MjpegClass.h"

// --- MJPEG player ---
MjpegClass mjpeg;
uint8_t *mjpeg_buf;
File mjpegFile;
bool videoPlaying = false;

// --- Video list ---
#define MJPEG_VIDEO_NORMAL   "/output.mjpeg"
#define MJPEG_VIDEO_SHORT    "/output_short.mjpeg"
#define MJPEG_VIDEO_SHORT_2  "/output_short_2.mjpeg"

const char *videoList[] = {
  MJPEG_VIDEO_NORMAL,
  MJPEG_VIDEO_SHORT,
  MJPEG_VIDEO_SHORT_2
};

int currentVideoIndex = 0;
int specialIndex = 1;

// --- FPS control ---
unsigned long lastFrameTime = 0;
const unsigned long frameInterval = 1000 / 30;

// --- Debounce button ---
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
bool lastButtonState = HIGH;

// --- JPEG draw callback ---
int jpegDrawCallback(JPEGDRAW *pDraw) {
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

// --- Avvia il video ---
void startVideo(const char *filename) {
  if (mjpegFile) mjpegFile.close();

  mjpegFile = LittleFS.open(filename);
  if (!mjpegFile || mjpegFile.isDirectory() || mjpegFile.size() < 1024) {
    Serial.printf("⚠️ Cannot open %s\n", filename);
    return;
  }

  mjpeg.setup(&mjpegFile, mjpeg_buf, jpegDrawCallback, true, 0, 0, gfx->width(), gfx->height());
  mjpeg.resetScale();
  videoPlaying = true;
  lastFrameTime = millis();
  Serial.printf("▶️ Playing video: %s\n", filename);
}

// --- Ferma la riproduzione ---
void stopVideo() {
  if (mjpegFile) mjpegFile.close();
  videoPlaying = false;
}

// --- Inizializza display ---
void initDisplay() {
  if (!gfx->begin()) {
    Serial.println("❌ Display init failed");
    while (1);
  }
  gfx->fillScreen(BLACK);
}

// --- Inizializza filesystem ---
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("❌ LittleFS init failed");
    while (1);
  }
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  initDisplay();
  initFS();

  mjpeg_buf = (uint8_t *)heap_caps_malloc(40 * 1024, MALLOC_CAP_8BIT);
  if (!mjpeg_buf) {
    Serial.println("❌ Failed to allocate MJPEG buffer");
    while (1);
  }

  currentVideoIndex = 0;
  startVideo(videoList[currentVideoIndex]);
}

// --- Loop principale ---
void loop() {
  bool reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && lastButtonState == HIGH) {
      currentVideoIndex = specialIndex;
      startVideo(videoList[currentVideoIndex]);
      specialIndex = (specialIndex == 1) ? 2 : 1;
    }
  }

  lastButtonState = reading;

  if (videoPlaying) {
    unsigned long now = millis();
    if (now - lastFrameTime >= frameInterval) {
      if (mjpegFile.available() && mjpeg.readMjpegBuf()) {
        mjpeg.drawJpg();
        lastFrameTime = now;
      } else {
        stopVideo();
        if (currentVideoIndex != 0) {
          currentVideoIndex = 0;
          startVideo(videoList[currentVideoIndex]);
        }
      }
    }
  }

  delay(1);
}
