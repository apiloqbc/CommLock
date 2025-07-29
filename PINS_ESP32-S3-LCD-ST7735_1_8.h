#pragma once
#include <Arduino_GFX_Library.h>

// --- Display backlight pin (LED on display) ---
// -1 means backlight is not controlled by software
#define GFX_BL     -1       

// --- Display resolution (ST7735 standard size) ---
#define TFT_WIDTH  128
#define TFT_HEIGHT 160

// --- SPI bus configuration for ESP32 ---
// Display pin mapping:
// SDA  (MOSI)  → GPIO 13
// SCK  (CLK)   → GPIO 14
// A0   (DC)    → GPIO 2
// CS   (CS)    → GPIO 15
// RESET        → GPIO 4
Arduino_DataBus *bus = new Arduino_HWSPI(
  2,    // A0 (DC - Data/Command pin)
  15,   // CS (Chip Select pin)
  14,   // SCK (SPI Clock)
  13,   // SDA (MOSI - SPI Data)
  -1    // MISO not used
);

// --- ST7735 Display initialization ---
// Rotation: 0 to 3 (adjust if text is upside down or sideways)
// Color inversion: false = normal colors (true if colors look wrong)
Arduino_GFX *gfx = new Arduino_ST7735(
  bus,
  4,     // RESET (Reset pin)
  3,     // Rotation (0–3)
  false, // Color inversion (set to true if needed)
  TFT_WIDTH,
  TFT_HEIGHT
);
