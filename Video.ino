/*******************************************************************************
 * MJPEG Player for ESP32 + ST7789V3 (170x320)
 * 
 * Required libraries:
 * - Arduino_GFX: https://github.com/moononournation/Arduino_GFX
 * - JPEGDEC: https://github.com/bitbank2/JPEGDEC
 ******************************************************************************/

#include <Arduino_GFX_Library.h>
#include <JPEGDEC.h>
#include <LittleFS.h>
#include "MjpegClass.h"
#include "PINS_ESP32-S3-LCD-ST7789_1_9.h"

#define MJPEG_FILENAME "/output_short.mjpeg"
#define GFX_SPEED 80000000
#define GFX_BRIGHTNESS 63



// --- MJPEG variables ---
MjpegClass mjpeg;
uint8_t *mjpeg_buf;
uint16_t *output_buf;
long output_buf_size, estimateBufferSize;
unsigned long total_frames, total_read_video, total_decode_video, total_show_video, start_ms, curr_ms;

// --- JPEG rendering callback ---
int jpegDrawCallback(JPEGDRAW *pDraw)
{
  unsigned long s = millis();
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  total_show_video += millis() - s;
  return 1;
}

void setup()
{
  Serial.begin(115200);
  delay(500);
  Serial.println("MJPEG Player Start");

  // --- Display initialization ---
  if (!gfx->begin(GFX_SPEED)) {
    gfx->fillScreen(BLACK);  // Clears the screen from old images
    Serial.println("Display init FAILED!");
    while (1);
  }
  gfx->fillScreen(BLACK);  // Clears the screen from old images


  // --- PWM backlight (GPIO32) ---
#if defined(GFX_BL)
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR < 3)
  ledcSetup(0, 1000, 8);
  ledcAttachPin(GFX_BL, 0);
  ledcWrite(0, GFX_BRIGHTNESS);
#else
  ledcAttachChannel(GFX_BL, 1000, 8, 1);
  ledcWrite(GFX_BL, GFX_BRIGHTNESS);
#endif
#endif

  // --- Filesystem initialization (LittleFS) ---
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount FAILED!");
    gfx->println("Filesystem Error!");
    while (1);
  }

  // --- Buffer allocation ---
  output_buf_size = gfx->width() * 4 * 2;
  output_buf = (uint16_t *)heap_caps_aligned_alloc(16, output_buf_size * sizeof(uint16_t), MALLOC_CAP_DMA);
  if (!output_buf) {
    Serial.println("output_buf alloc failed!");
    while (1);
  }

  estimateBufferSize = gfx->width() * gfx->height() * 2 / 5;
  mjpeg_buf = (uint8_t *)heap_caps_malloc(estimateBufferSize, MALLOC_CAP_8BIT);
  if (!mjpeg_buf) {
    Serial.println("mjpeg_buf alloc failed!");
    while (1);
  }
}

void loop()
{
  File mjpegFile = LittleFS.open(MJPEG_FILENAME, "r");

  if (!mjpegFile || mjpegFile.isDirectory()) {
    Serial.println("File open FAILED: " MJPEG_FILENAME);
    gfx->fillScreen(RED);
    gfx->setCursor(10, 150);
    gfx->setTextSize(2);
    gfx->setTextColor(WHITE);
    gfx->println("File not found!");
    delay(5000);
    return;
  }

  Serial.println("MJPEG playback start");

  start_ms = millis();
  curr_ms = millis();
  total_frames = 0;
  total_read_video = 0;
  total_decode_video = 0;
  total_show_video = 0;

  mjpeg.setup(
    &mjpegFile, mjpeg_buf, jpegDrawCallback, true,
    0, 0, gfx->width(), gfx->height()
  );
  mjpeg.resetScale();  // <--- Forces recalculation of scale and centering


  while (mjpegFile.available() && mjpeg.readMjpegBuf()) {
    total_read_video += millis() - curr_ms;
    curr_ms = millis();

    mjpeg.drawJpg();
    delay(33);
    total_decode_video += millis() - curr_ms;

    curr_ms = millis();
    total_frames++;
  }

  int time_used = millis() - start_ms;
  mjpegFile.close();
  float fps = 1000.0 * total_frames / time_used;

  total_decode_video -= total_show_video;

  Serial.println("MJPEG playback end");
  Serial.printf("Total frames: %lu\n", total_frames);
  Serial.printf("Time used: %d ms\n", time_used);
  Serial.printf("FPS: %.2f\n", fps);
  Serial.printf("Read MJPEG: %lu ms (%.1f %%)\n", total_read_video, 100.0 * total_read_video / time_used);
  Serial.printf("Decode: %lu ms (%.1f %%)\n", total_decode_video, 100.0 * total_decode_video / time_used);
  Serial.printf("Display: %lu ms (%.1f %%)\n", total_show_video, 100.0 * total_show_video / time_used);

  delay(2000); // Delay before repeating
}
