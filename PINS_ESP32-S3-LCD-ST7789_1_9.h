#define GFX_BL 32           // GPIO for backlight (BLK/BL)
#define TFT_WIDTH 220       // Display width
#define TFT_HEIGHT 320      // Display height

#include <Arduino_GFX_Library.h>

// --- Display SPI config (DC, CS, SCK, MOSI, optional MISO) ---
Arduino_DataBus *bus = new Arduino_HWSPI(
  2,    // DC (GPIO2)
  15,   // CS (GPIO15)
  18,   // SCK (GPIO18)
  23,   // MOSI (GPIO23)
  -1    // MISO not used
);

Arduino_GFX *gfx = new Arduino_ST7789(
  bus,
  4,
  1,            // <-- Horizontal rotation
  true,
  TFT_WIDTH,
  TFT_HEIGHT
);
