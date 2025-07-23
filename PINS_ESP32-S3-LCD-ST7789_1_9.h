#pragma once
#include <Arduino_GFX_Library.h>

// Scegli il display:
#define USE_ST7789
// #define USE_ST7789

#define GFX_BL 32
#define BUTTON_PIN 22


// 🔄 Bus SPI comune
static Arduino_DataBus *bus = new Arduino_HWSPI(
  2,    // DC
  15,   // CS
  18,   // SCK
  23,   // MOSI
  -1    // MISO
);

// ✅ Dichiara gfx come `extern` per gli altri file
extern Arduino_GFX *gfx;



#ifdef USE_ST7735
Arduino_GFX *gfx = new Arduino_ST7735(bus, 4, 1, true, 220, 320);
#endif

#ifdef USE_ST7789
Arduino_GFX *gfx = new Arduino_ST7789(bus, 4, 1, true, 240, 320);
#endif

