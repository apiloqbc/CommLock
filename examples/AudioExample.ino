/*
 * DFPlayer Mini Audio Example
 * 
 * This sketch shows how to use the DFPlayer Mini module
 * for MP3 audio playback with ESP32-S3.
 */

#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

// DFPlayer Mini setup
SoftwareSerial mySerial(9, 10); // RX=9, TX=10
DFRobotDFPlayerMini player;

// Button setup
const uint8_t BUTTON_PIN = 5;
const uint8_t BUZZER_PIN = 8;

// Audio configuration
#define BUTTON_TONE_FREQ 800
#define BUTTON_TONE_DURATION 100

void setup() {
  Serial.begin(115200);
  Serial.println("🎵 DFPlayer Mini Audio Example");
  
  // Initialize DFPlayer
  if (!initializeDFPlayer()) {
    Serial.println("❌ DFPlayer initialization failed!");
    while (1) delay(100);
  }
  
  // Initialize button and buzzer
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  
  Serial.println("✅ System ready!");
  Serial.println("Press button to play audio tracks");
}

void loop() {
  // Check button press
  if (digitalRead(BUTTON_PIN) == LOW) {
    playButtonTone();
    delay(50); // Debounce
    
    // Play a random track (1-9)
    uint8_t track = random(1, 10);
    playAudioTrack(track);
    
    // Wait for button release
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(10);
    }
  }
  
  // Check if audio finished
  if (player.available()) {
    if (player.readType() == DFPlayerPlayFinished) {
      Serial.println("🎵 Audio track finished");
    }
  }
  
  delay(10);
}

bool initializeDFPlayer() {
  mySerial.begin(9600, SERIAL_8N1, 9, 10);
  
  if (player.begin(mySerial)) {
    Serial.println("🎵 DFPlayer ready");
    player.volume(25); // Volume from 0 to 30
    return true;
  } else {
    Serial.println("❌ DFPlayer not detected");
    return false;
  }
}

void playAudioTrack(uint8_t trackNumber) {
  if (player.available()) {
    player.play(trackNumber);
    Serial.printf("🎵 Playing track: %04d.mp3\n", trackNumber);
  } else {
    Serial.println("⚠️ DFPlayer not available");
  }
}

void playButtonTone() {
  tone(BUZZER_PIN, BUTTON_TONE_FREQ, BUTTON_TONE_DURATION);
  Serial.println("🔊 Button tone");
}

void stopAudio() {
  if (player.available()) {
    player.stop();
    Serial.println("🔇 Audio stopped");
  }
}

// Audio control functions
void setVolume(uint8_t volume) {
  if (player.available()) {
    player.volume(volume); // 0-30
    Serial.printf("🔊 Volume set to: %d\n", volume);
  }
}

void pauseAudio() {
  if (player.available()) {
    player.pause();
    Serial.println("⏸️ Audio paused");
  }
}

void resumeAudio() {
  if (player.available()) {
    player.start();
    Serial.println("▶️ Audio resumed");
  }
} 