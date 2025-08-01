#pragma once
#include "PINS_ESP32-S3-LCD-ST7735_1_8.h"
#include <JPEGDEC.h>

// ===== DISPLAY MANAGER CLASS =====
class DisplayManager {
private:
  Arduino_GFX* _gfx;
  bool _initialized;
  
public:
  DisplayManager() : _gfx(gfx), _initialized(false) {}
  
  bool begin() {
    if (!_gfx->begin()) {
      Serial.println("❌ Display initialization failed");
      return false;
    }
    
    // Initialize backlight
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
    
    _initialized = true;
    Serial.println("✅ Display initialized successfully");
    return true;
  }
  
  void clear() {
    if (_initialized) _gfx->fillScreen(BLACK);
  }
  
  void setTextColor(uint16_t color) {
    if (_initialized) _gfx->setTextColor(color);
  }
  
  void setTextSize(uint8_t size) {
    if (_initialized) _gfx->setTextSize(size);
  }
  
  void setCursor(int16_t x, int16_t y) {
    if (_initialized) _gfx->setCursor(x, y);
  }
  
  void print(const char* text) {
    if (_initialized) _gfx->print(text);
  }
  
  void print(int number) {
    if (_initialized) _gfx->print(number);
  }
  
  void println(const char* text) {
    if (_initialized) _gfx->println(text);
  }
  
  void println() {
    if (_initialized) _gfx->println();
  }
  
  void drawCenteredText(const char* text, int16_t y, uint8_t textSize = 1) {
    if (!_initialized) return;
    
    _gfx->setTextSize(textSize);
    int16_t x1, y1;
    uint16_t w, h;
    _gfx->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    _gfx->setCursor((_gfx->width() - w) / 2, y);
    _gfx->println(text);
  }
  
  void drawJpegFrame(JPEGDRAW* pDraw) {
    if (_initialized) {
      _gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
    }
  }
  
  int width() { return _initialized ? _gfx->width() : 0; }
  int height() { return _initialized ? _gfx->height() : 0; }
  bool isInitialized() { return _initialized; }
}; 