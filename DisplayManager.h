#pragma once
#include "PINS_ESP32-S3-LCD-ST7735_1_8.h"
#include <JPEGDEC.h>

// Display error codes
enum DisplayError {
  DISPLAY_ERROR_NONE = 0,
  DISPLAY_ERROR_INIT_FAILED = 1,
  DISPLAY_ERROR_NOT_INITIALIZED = 2,
  DISPLAY_ERROR_MEMORY_FAILED = 3,
  DISPLAY_ERROR_DRAW_FAILED = 4
};

// ===== DISPLAY MANAGER CLASS =====
class DisplayManager {
private:
  Arduino_GFX* _gfx;
  bool _initialized;
  DisplayError _lastError;
  unsigned long _lastDrawTime;
  unsigned long _drawCount;
  unsigned long _errorCount;
  
  // Performance monitoring
  unsigned long _totalDrawTime;
  float _averageDrawTime;
  
public:
  DisplayManager() : _gfx(gfx), _initialized(false), _lastError(DISPLAY_ERROR_NONE),
                     _lastDrawTime(0), _drawCount(0), _errorCount(0),
                     _totalDrawTime(0), _averageDrawTime(0.0f) {}
  
  bool begin() {
    if (_initialized) {
      return true; // Already initialized
    }
    
    if (!_gfx) {
      _lastError = DISPLAY_ERROR_INIT_FAILED;
      Serial.println("❌ Display: GFX object is null");
      return false;
    }
    
    if (!_gfx->begin()) {
      _lastError = DISPLAY_ERROR_INIT_FAILED;
      Serial.println("❌ Display: GFX initialization failed");
      return false;
    }
    
    // Initialize backlight
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
    
    _initialized = true;
    _lastError = DISPLAY_ERROR_NONE;
    Serial.println("✅ Display initialized successfully");
    return true;
  }
  
  void clear() {
    if (!_initialized) {
      _lastError = DISPLAY_ERROR_NOT_INITIALIZED;
      return;
    }
    
    unsigned long startTime = micros();
    _gfx->fillScreen(BLACK);
    _updateDrawMetrics(micros() - startTime);
  }
  
  void setTextColor(uint16_t color) {
    if (_initialized) {
      _gfx->setTextColor(color);
    }
  }
  
  void setTextSize(uint8_t size) {
    if (_initialized) {
      _gfx->setTextSize(size);
    }
  }
  
  void setCursor(int16_t x, int16_t y) {
    if (_initialized) {
      _gfx->setCursor(x, y);
    }
  }
  
  void print(const char* text) {
    if (!_initialized) {
      _lastError = DISPLAY_ERROR_NOT_INITIALIZED;
      return;
    }
    
    unsigned long startTime = micros();
    _gfx->print(text);
    _updateDrawMetrics(micros() - startTime);
  }
  
  void print(int number) {
    if (!_initialized) {
      _lastError = DISPLAY_ERROR_NOT_INITIALIZED;
      return;
    }
    
    unsigned long startTime = micros();
    _gfx->print(number);
    _updateDrawMetrics(micros() - startTime);
  }
  
  void println(const char* text) {
    if (!_initialized) {
      _lastError = DISPLAY_ERROR_NOT_INITIALIZED;
      return;
    }
    
    unsigned long startTime = micros();
    _gfx->println(text);
    _updateDrawMetrics(micros() - startTime);
  }
  
  void println() {
    if (!_initialized) {
      _lastError = DISPLAY_ERROR_NOT_INITIALIZED;
      return;
    }
    
    unsigned long startTime = micros();
    _gfx->println();
    _updateDrawMetrics(micros() - startTime);
  }
  
  void drawCenteredText(const char* text, int16_t y, uint8_t textSize = 1) {
    if (!_initialized) {
      _lastError = DISPLAY_ERROR_NOT_INITIALIZED;
      return;
    }
    
    unsigned long startTime = micros();
    
    _gfx->setTextSize(textSize);
    int16_t x1, y1;
    uint16_t w, h;
    _gfx->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    _gfx->setCursor((_gfx->width() - w) / 2, y);
    _gfx->println(text);
    
    _updateDrawMetrics(micros() - startTime);
  }
  
  void drawJpegFrame(JPEGDRAW* pDraw) {
    if (!_initialized) {
      _lastError = DISPLAY_ERROR_NOT_INITIALIZED;
      return;
    }
    
    if (!pDraw) {
      _lastError = DISPLAY_ERROR_DRAW_FAILED;
      return;
    }
    
    unsigned long startTime = micros();
    
    try {
      _gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
      _updateDrawMetrics(micros() - startTime);
    } catch (...) {
      _lastError = DISPLAY_ERROR_DRAW_FAILED;
      _errorCount++;
      Serial.println("❌ Display: JPEG draw failed");
    }
  }
  
  // Getters
  int width() { return _initialized ? _gfx->width() : 0; }
  int height() { return _initialized ? _gfx->height() : 0; }
  bool isInitialized() { return _initialized; }
  DisplayError getLastError() { return _lastError; }
  
  // Performance monitoring
  float getAverageDrawTime() { return _averageDrawTime; }
  unsigned long getDrawCount() { return _drawCount; }
  unsigned long getErrorCount() { return _errorCount; }
  unsigned long getLastDrawTime() { return _lastDrawTime; }
  
  // Reset error state
  void resetError() {
    _lastError = DISPLAY_ERROR_NONE;
  }
  
  // Get error string
  const char* getErrorString() {
    switch (_lastError) {
      case DISPLAY_ERROR_NONE: return "No Error";
      case DISPLAY_ERROR_INIT_FAILED: return "Init Failed";
      case DISPLAY_ERROR_NOT_INITIALIZED: return "Not Initialized";
      case DISPLAY_ERROR_MEMORY_FAILED: return "Memory Failed";
      case DISPLAY_ERROR_DRAW_FAILED: return "Draw Failed";
      default: return "Unknown Error";
    }
  }
  
  // Performance report
  void printPerformanceReport() {
    Serial.println("=== Display Performance Report ===");
    Serial.printf("Initialized: %s\n", _initialized ? "Yes" : "No");
    Serial.printf("Last Error: %s\n", getErrorString());
    Serial.printf("Draw Count: %lu\n", _drawCount);
    Serial.printf("Error Count: %lu\n", _errorCount);
    Serial.printf("Average Draw Time: %.2f μs\n", _averageDrawTime);
    Serial.printf("Last Draw Time: %lu μs\n", _lastDrawTime);
    Serial.println("==================================");
  }
  
private:
  void _updateDrawMetrics(unsigned long drawTime) {
    _lastDrawTime = drawTime;
    _drawCount++;
    _totalDrawTime += drawTime;
    _averageDrawTime = (float)_totalDrawTime / _drawCount;
  }
}; 