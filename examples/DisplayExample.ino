/*
 * DisplayManager Class Usage Example
 * 
 * This sketch shows how to use the DisplayManager class
 * to easily manage the TFT ST7735 display.
 */

#include "DisplayManager.h"

// Display manager instance
DisplayManager display;

void setup() {
  Serial.begin(115200);
  Serial.println("🚀 Display Manager Example");
  
  // Initialize display
  if (!display.begin()) {
    Serial.println("❌ Display initialization failed!");
    while (1) delay(100);
  }
  
  Serial.println("✅ Display ready!");
  
  // Show usage examples
  showDisplayExamples();
}

void loop() {
  // Empty loop - static example
  delay(1000);
}

void showDisplayExamples() {
  // 1. Clear screen
  display.clear();
  
  // 2. Simple text
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(10, 10);
  display.println("Display Manager");
  
  // 3. Centered text
  display.drawCenteredText("Centered", 30, 2);
  
  // 4. Different colors
  display.setTextColor(RED);
  display.setCursor(10, 50);
  display.println("Red");
  
  display.setTextColor(GREEN);
  display.setCursor(10, 70);
  display.println("Green");
  
  display.setTextColor(BLUE);
  display.setCursor(10, 90);
  display.println("Blue");
  
  // 5. Display information
  display.setTextColor(YELLOW);
  display.setCursor(10, 120);
  display.print("Display: ");
  display.print(display.width());
  display.print("x");
  display.println(display.height());
  
  // 6. Initialization status
  display.setCursor(10, 140);
  display.setTextColor(display.isInitialized() ? GREEN : RED);
  display.print("Status: ");
  display.println(display.isInitialized() ? "OK" : "ERROR");
} 