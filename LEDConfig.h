#pragma once

// ===== LED CONFIGURATION =====
// This file allows you to configure LED behavior for each video

// LED Pin Configuration
#define LED_PIN 8  // Change this to your LED pin

// LED Configuration for each video
// Format: {videoIndex, duration_ms, interval_ms, enabled}
// - videoIndex: 0-8 (for videos 1-9)
// - duration_ms: How long the LED blinks (in milliseconds)
// - interval_ms: Time between LED on/off states (in milliseconds)
// - enabled: true/false to enable/disable LED for this video

struct LEDConfig {
  uint8_t videoIndex;     // Which video triggers the LED
  uint16_t blinkDuration; // Duration in milliseconds
  uint16_t blinkInterval; // Interval between blinks in milliseconds
  bool enabled;           // Whether this LED config is active
};

// Default LED configurations
// You can modify these values to customize LED behavior
const LEDConfig DEFAULT_LED_CONFIGS[] = {
  {0, 5000, 500, true},   // Video 1: 5s duration, 500ms interval
  {1, 3000, 300, true},   // Video 2: 3s duration, 300ms interval
  {2, 4000, 400, true},   // Video 3: 4s duration, 400ms interval
  {3, 6000, 600, true},   // Video 4: 6s duration, 600ms interval
  {4, 2000, 200, true},   // Video 5: 2s duration, 200ms interval
  {5, 7000, 700, true},   // Video 6: 7s duration, 700ms interval
  {6, 3500, 350, true},   // Video 7: 3.5s duration, 350ms interval
  {7, 4500, 450, true},   // Video 8: 4.5s duration, 450ms interval
  {8, 8000, 800, true}    // Video 9: 8s duration, 800ms interval
};

// LED Behavior Examples:
// 
// Fast blinking (alert):
// {0, 3000, 100, true}    // 3s duration, 100ms interval
//
// Slow blinking (ambient):
// {1, 10000, 1000, true}  // 10s duration, 1s interval
//
// Disabled LED:
// {2, 5000, 500, false}   // LED disabled for video 3
//
// Continuous blink:
// {3, 15000, 200, true}   // 15s duration, 200ms interval 