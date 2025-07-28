#pragma once
#include <Arduino_GFX_Library.h>  // 🔥 Importante: deve venire prima

#define GFX_BL 32
#define BUTTON_PIN 22  // Sostituisci con il pin corretto per il tuo pulsante
#define TFT_WIDTH 220
#define TFT_HEIGHT 320

// Display SPI config
Arduino_DataBus *bus = new Arduino_HWSPI(
  2,    // DC
  15,   // CS
  18,   // SCK
  23,   // MOSI
  -1    // MISO non usato
);

Arduino_GFX *gfx = new Arduino_ST7789(
  bus,
  4,    // RST
  1,    // Rotazione
  true, // Colori invertiti
  TFT_WIDTH,
  TFT_HEIGHT
);