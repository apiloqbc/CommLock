#pragma once
#include <Arduino_GFX_Library.h>

// --- Display backlight pin ---
// -1 means backlight is not controlled by any GPIO
#define GFX_BL     -1       
   
// --- Display resolution ---
#define TFT_WIDTH  128
#define TFT_HEIGHT 160

// --- SPI bus configuration ---
Arduino_DataBus *bus = new Arduino_HWSPI(
  2,    // DC pin
  15,   // CS pin
  14,   // SCK (clock) — matches known working setup
  13,   // MOSI (data)
  -1    // MISO not used
);

// --- ST7735 Display configuration ---
Arduino_GFX *gfx = new Arduino_ST7735(
  bus,
  4,    // RST pin (reset)
  3,    // Rotation (0–3) — matches Adafruit example code
  false,// Color inversion (❗️ Adafruit_ST7735 uses non-inverted colors for BLACKTAB)
  TFT_WIDTH,
  TFT_HEIGHT
);
