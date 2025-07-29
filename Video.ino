#include "PINS_ESP32-S3-LCD-ST7789_1_9.h"
#include <Arduino_GFX_Library.h>
#include <LittleFS.h>
#include <JPEGDEC.h>
#include "MjpegClass.h"

#define TOTAL_VIDEOS 9
const char *homeVideo = "/home.mjpeg";
MjpegClass mjpeg;
uint8_t *mjpeg_buf;
File mjpegFile;
bool videoPlaying = false;

const char *videoList[TOTAL_VIDEOS] = {
  "/video_1.mjpeg", "/video_2.mjpeg", "/video_3.mjpeg",
  "/video_4.mjpeg", "/video_5.mjpeg", "/video_6.mjpeg",
  "/video_7.mjpeg", "/video_8.mjpeg", "/video_9.mjpeg"
};
const char *videoNames[TOTAL_VIDEOS] = {
  "Video 1", "Video 2", "Video 3",
  "Video 4", "Video 5", "Video 6",
  "Video 7", "Video 8", "Video 9"
};

int currentVideoIndex = -1;
const uint8_t buttonPins[TOTAL_VIDEOS] = {14,15,16,17,19,20,21,22,48};
bool buttonStates[TOTAL_VIDEOS] = { HIGH };

const uint8_t MENU_BUTTON_PIN = 5;     // Tasto menu
bool menuState = HIGH;
bool menuActive = false;

unsigned long lastFrameTime = 0;
const unsigned long frameInterval = 1000 / 30;

void showSplash() {
  gfx->fillScreen(BLACK);
  gfx->setCursor(30, 60);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->println("Loading...");
  delay(1000);
}

int jpegDrawCallback(JPEGDRAW *pDraw) {
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

void drawMenu() {
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(10, 10);
  gfx->println("Select Video:");
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    gfx->setCursor(10, 25 + i*12);
    gfx->print(i+1);
    gfx->print(": ");
    gfx->println(videoNames[i]);
  }
  gfx->setCursor(10, 25 + TOTAL_VIDEOS*12);
  gfx->println("Press menu again to close");
}

void startVideo(const char *filename) {
  mjpegFile.close();
  mjpegFile = LittleFS.open(filename);
  if (!mjpegFile || mjpegFile.isDirectory() || mjpegFile.size() < 1024) {
    Serial.printf("⚠️ Cannot open file: %s\n", filename);
    return;
  }
  mjpeg.setup(&mjpegFile, mjpeg_buf, jpegDrawCallback, true, 0, 0, gfx->width(), gfx->height());
  mjpeg.resetScale();
  videoPlaying = true;
  lastFrameTime = millis();
  Serial.printf("▶️ Playing: %s\n", filename);
}

void stopVideo() {
  mjpegFile.close();
  videoPlaying = false;
}

void initDisplay() {
  if (!gfx->begin()) { Serial.println("❌ Display init failed"); while (1); }
  gfx->fillScreen(BLACK);
}

void initFS() {
  if (!LittleFS.begin()) { Serial.println("❌ FS init failed"); while (1); }
}

void setup() {
  Serial.begin(115200);
  pinMode(GFX_BL, OUTPUT); digitalWrite(GFX_BL, HIGH);

  pinMode(MENU_BUTTON_PIN, INPUT_PULLUP);
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  initDisplay();
  showSplash();
  initFS();

  mjpeg_buf = (uint8_t*)heap_caps_malloc(40*1024, MALLOC_CAP_8BIT);
  if (!mjpeg_buf) {
    Serial.println("❌ Buffer alloc failed");
    gfx->fillScreen(BLACK);
    gfx->setCursor(10,40);
    gfx->setTextColor(RED);
    gfx->setTextSize(2);
    gfx->println("Error:");
    gfx->println("MJPEG buffer");
    while (1);
  }

  currentVideoIndex = -1;
  startVideo(homeVideo);
}

void loop() {
  bool menuPressed = digitalRead(MENU_BUTTON_PIN);
  if (menuPressed == LOW && menuState == HIGH) {
    menuActive = !menuActive;
    if (menuActive) {
      drawMenu();
      videoPlaying = false;
    } else {
      // exit menu: resume home or last video
      currentVideoIndex = -1;
      startVideo(homeVideo);
    }
  }
  menuState = menuPressed;

  if (menuActive) {
    for (int i = 0; i < TOTAL_VIDEOS; i++) {
      bool st = digitalRead(buttonPins[i]);
      if (st == LOW && buttonStates[i] == HIGH) {
        currentVideoIndex = i;
        menuActive = false;
        startVideo(videoList[i]);
      }
      buttonStates[i] = st;
    }
  } else {
    for (int i = 0; i < TOTAL_VIDEOS; i++) {
      bool st = digitalRead(buttonPins[i]);
      if (st == LOW && buttonStates[i] == HIGH) {
        if (currentVideoIndex != i) {
          currentVideoIndex = i;
          startVideo(videoList[i]);
        }
      }
      buttonStates[i] = st;
    }
  }

  if (videoPlaying) {
    unsigned long now = millis();
    if (now - lastFrameTime >= frameInterval) {
      if (mjpegFile.available() && mjpeg.readMjpegBuf()) {
        mjpeg.drawJpg();
        lastFrameTime = now;
      } else {
        stopVideo();
        if (currentVideoIndex != -1) {
          currentVideoIndex = -1;
          startVideo(homeVideo);
        }
      }
    }
  }
  delay(1);
}
