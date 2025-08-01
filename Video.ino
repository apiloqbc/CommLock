#include "DisplayManager.h"
#include "Logger.h"
#include "LEDConfig.h"
#include <LittleFS.h>
#include "MjpegClass.h"
#include "DFRobotDFPlayerMini.h"

// ===== GLOBAL DISPLAY MANAGER =====
DisplayManager display;

// ===== DFPLAYER MINI SETUP =====
// Use HardwareSerial (Serial2) for ESP32-S3 instead of SoftwareSerial
DFRobotDFPlayerMini player;

// Configuration
#define TOTAL_VIDEOS 9
#define MJPEG_BUFFER_SIZE (40 * 1024)
#define TARGET_FPS 30
#define FRAME_INTERVAL (1000 / TARGET_FPS)
#define DEBOUNCE_DELAY 50
#define SPLASH_DURATION 2000
#define BUTTON_TONE_FREQ 800
#define BUTTON_TONE_DURATION 100
#define MAX_RETRY_ATTEMPTS 3
#define ERROR_RECOVERY_DELAY 5000
#define COMMLOCK_DURATION 15000 // 15 seconds for Commlock display

// Error codes for better debugging
enum ErrorCode {
  ERROR_NONE = 0,
  ERROR_DISPLAY_INIT_FAILED = 1,
  ERROR_FILESYSTEM_INIT_FAILED = 2,
  ERROR_MEMORY_ALLOCATION_FAILED = 3,
  ERROR_DFPLAYER_INIT_FAILED = 4,
  ERROR_VIDEO_FILE_NOT_FOUND = 5,
  ERROR_VIDEO_FILE_CORRUPTED = 6,
  ERROR_AUDIO_TRACK_NOT_FOUND = 7,
  ERROR_SYSTEM_OVERLOAD = 8
};

// System states
enum SystemState {
  STATE_SPLASH,
  STATE_HOME,
  STATE_MENU,
  STATE_PLAYING_VIDEO,
  STATE_COMMLOCK,
  STATE_ERROR,
  STATE_RECOVERY
};

// Structure to manage videos
struct VideoInfo {
  const char* filename;
  const char* displayName;
  uint8_t audioTrack; // MP3 track number (0001.mp3 = 1, 0002.mp3 = 2, etc.)
};

// Video list with audio track mapping
const VideoInfo videoDatabase[TOTAL_VIDEOS] = {
  {"/video_1.mjpeg", "Video 1", 1},
  {"/video_2.mjpeg", "Video 2", 2},
  {"/video_3.mjpeg", "Video 3", 3},
  {"/video_4.mjpeg", "Video 4", 4},
  {"/video_5.mjpeg", "Video 5", 5},
  {"/video_6.mjpeg", "Video 6", 6},
  {"/video_7.mjpeg", "Video 7", 7},
  {"/video_8.mjpeg", "Video 8", 8},
  {"/video_9.mjpeg", "Video 9", 9}
};

const char* HOME_VIDEO = "/home.mjpeg";

// Pin configuration
const uint8_t BUTTON_PINS[TOTAL_VIDEOS] = {
  46, 35, 16, 17, 19, 20, 21, 47, 48
};
const uint8_t MENU_BUTTON_PIN = 5;

// Commlock variables
int commlockHour = 0;
int commlockMinute = 0;
unsigned long commlockStartTime = 0;
unsigned long lastCommlockUpdate = 0;

// LED system variables
LEDConfig ledConfigs[TOTAL_VIDEOS];

bool ledActive = false;
unsigned long ledStartTime = 0;
unsigned long lastLedBlink = 0;
bool ledState = false;

// Global system variables
SystemState currentState = STATE_SPLASH;
ErrorCode lastError = ERROR_NONE;
MjpegClass mjpeg;
File mjpegFile;
uint8_t* mjpegBuffer = nullptr;

// Video state
int currentVideoIndex = -1;
bool videoPlaying = false;
unsigned long lastFrameTime = 0;
unsigned long splashStartTime = 0;

// Audio state
bool audioPlaying = false;
uint8_t currentAudioTrack = 0;

// Performance monitoring
unsigned long frameCount = 0;
unsigned long lastFpsCheck = 0;
float currentFps = 0.0f;
unsigned long loopStartTime = 0;
unsigned long maxLoopTime = 0;

// Error recovery
unsigned long errorStartTime = 0;
uint8_t retryCount = 0;

// Button management with debounce
struct ButtonState {
  bool currentState;
  bool lastState;
  unsigned long lastDebounceTime;
  bool wasPressed;
};

ButtonState videoButtons[TOTAL_VIDEOS];
ButtonState menuButton;

// Function prototypes
void initializeSystem();
void initializeFileSystem();
bool allocateBuffer();
void deallocateBuffer();
void initializeButtons();
void initializeDFPlayer();
void initializeLED();
void updateButtons();
bool isButtonPressed(ButtonState& button, uint8_t pin);
void handleStateMachine();
void handleSplashState();
void handleMenuState();
void handleVideoState();
void handleCommlockState();
void handleErrorState();
void handleRecoveryState();
void drawSplashScreen();
void drawMenu();
void drawCommlock();
void drawErrorScreen(ErrorCode error, const char* details = nullptr);
void drawPerformanceInfo();
bool startVideo(const char* filename, int videoIndex = -1);
void stopVideo();
void returnToHome();
void playButtonTone();
void startAudio(uint8_t trackNumber);
void stopAudio();
int jpegDrawCallback(JPEGDRAW* pDraw);
void reportError(ErrorCode error, const char* details = nullptr);
bool attemptRecovery();
const char* getErrorString(ErrorCode error);
void updatePerformanceMetrics();
void printSystemStatus();
void startCommlock();
void updateCommlock();
void startLED(uint8_t videoIndex);
void updateLED();
void stopLED();
void configureLED(uint8_t videoIndex, uint16_t duration, uint16_t interval, bool enabled);

void setup() {
  Serial.begin(115200);
  Logger::begin();
  Logger::info(LOG_CAT_SYSTEM, "Starting MJPEG Player with Commlock...");
  
  initializeSystem();
  splashStartTime = millis();
}

void loop() {
  loopStartTime = micros();
  
  updateButtons();
  handleStateMachine();
  updatePerformanceMetrics();
  updateLED();
  
  // Adaptive delay based on system load
  unsigned long loopTime = micros() - loopStartTime;
  if (loopTime < 1000) { // Less than 1ms
    delayMicroseconds(1000 - loopTime);
  }
}

void initializeSystem() {
  Logger::info(LOG_CAT_SYSTEM, "Initializing system components...");
  
  // Initialize display first
  if (!display.begin()) {
    reportError(ERROR_DISPLAY_INIT_FAILED, "Display.begin() returned false");
    return;
  }
  
  initializeButtons();
  initializeDFPlayer();
  initializeLED();
  drawSplashScreen();
  initializeFileSystem();
  
  if (!allocateBuffer()) {
    reportError(ERROR_MEMORY_ALLOCATION_FAILED, "Failed to allocate MJPEG buffer");
    return;
  }
  
  Logger::info(LOG_CAT_SYSTEM, "System initialized successfully");
  printSystemStatus();
}

void initializeDFPlayer() {
  Logger::info(LOG_CAT_AUDIO, "Initializing DFPlayer...");
  
  // Initialize Serial2 for ESP32-S3 (pins 9 and 10)
  Serial2.begin(9600, SERIAL_8N1, 9, 10); // RX=9, TX=10
  
  if (player.begin(Serial2)) {
    Logger::info(LOG_CAT_AUDIO, "DFPlayer ready");
    player.volume(25); // Volume from 0 to 30
  } else {
    reportError(ERROR_DFPLAYER_INIT_FAILED, "DFPlayer.begin() failed");
  }
}

void initializeLED() {
  Logger::info(LOG_CAT_SYSTEM, "Initializing LED system...");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize LED configurations from LEDConfig.h
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    ledConfigs[i] = DEFAULT_LED_CONFIGS[i];
    Logger::info(LOG_CAT_SYSTEM, "LED config %d: duration=%d ms, interval=%d ms, enabled=%s", 
                 i + 1, ledConfigs[i].blinkDuration, ledConfigs[i].blinkInterval, 
                 ledConfigs[i].enabled ? "true" : "false");
  }
  
  Logger::info(LOG_CAT_SYSTEM, "LED system initialized on pin %d", LED_PIN);
}

void initializeFileSystem() {
  Logger::info(LOG_CAT_SYSTEM, "Initializing filesystem...");
  if (!LittleFS.begin()) {
    reportError(ERROR_FILESYSTEM_INIT_FAILED, "LittleFS.begin() failed");
    return;
  }
  Logger::info(LOG_CAT_SYSTEM, "Filesystem initialized");
}

bool allocateBuffer() {
  Logger::info(LOG_CAT_MEMORY, "Allocating MJPEG buffer (%d bytes)", MJPEG_BUFFER_SIZE);
  mjpegBuffer = (uint8_t*)heap_caps_malloc(MJPEG_BUFFER_SIZE, MALLOC_CAP_8BIT);
  if (!mjpegBuffer) {
    Logger::error(LOG_CAT_MEMORY, "MJPEG buffer allocation failed. Free heap: %d bytes", ESP.getFreeHeap());
    return false;
  }
  Logger::info(LOG_CAT_MEMORY, "Allocated %d bytes for MJPEG buffer. Free heap: %d bytes", 
               MJPEG_BUFFER_SIZE, ESP.getFreeHeap());
  return true;
}

void deallocateBuffer() {
  if (mjpegBuffer) {
    Logger::info(LOG_CAT_MEMORY, "Deallocating MJPEG buffer");
    heap_caps_free(mjpegBuffer);
    mjpegBuffer = nullptr;
  }
}

void initializeButtons() {
  Logger::info(LOG_CAT_BUTTON, "Initializing buttons...");
  pinMode(MENU_BUTTON_PIN, INPUT_PULLUP);
  menuButton = {HIGH, HIGH, 0, false};
  
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    videoButtons[i] = {HIGH, HIGH, 0, false};
  }
  Logger::info(LOG_CAT_BUTTON, "Buttons initialized (%d video buttons + 1 menu button)", TOTAL_VIDEOS);
}

void updateButtons() {
  // Update menu button
  if (isButtonPressed(menuButton, MENU_BUTTON_PIN)) {
    Logger::debug(LOG_CAT_BUTTON, "Menu button pressed");
    playButtonTone();
  }
  
  // Update video buttons
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    if (isButtonPressed(videoButtons[i], BUTTON_PINS[i])) {
      Logger::debug(LOG_CAT_BUTTON, "Video button %d pressed", i + 1);
      playButtonTone();
      
      // Commlock feature for button 9 (last button)
      if (i == 8) { // Button 9 (index 8)
        Logger::info(LOG_CAT_SYSTEM, "Commlock button pressed - activating Commlock display");
        startCommlock();
      }
    }
  }
}

bool isButtonPressed(ButtonState& button, uint8_t pin) {
  bool reading = digitalRead(pin);
  bool pressed = false;
  
  if (reading != button.lastState) {
    button.lastDebounceTime = millis();
  }
  
  if ((millis() - button.lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != button.currentState) {
      button.currentState = reading;
      if (button.currentState == LOW) {
        pressed = true;
        button.wasPressed = true;
      }
    }
  }
  
  button.lastState = reading;
  return pressed;
}

void playButtonTone() {
  // Play a short tone when button is pressed
  tone(8, BUTTON_TONE_FREQ, BUTTON_TONE_DURATION); // Use pin 8 for buzzer
  Logger::debug(LOG_CAT_AUDIO, "Button tone played (%d Hz, %d ms)", BUTTON_TONE_FREQ, BUTTON_TONE_DURATION);
}

void handleStateMachine() {
  switch (currentState) {
    case STATE_SPLASH:
      handleSplashState();
      break;
      
    case STATE_HOME:
    case STATE_PLAYING_VIDEO:
      handleVideoState();
      break;
      
    case STATE_MENU:
      handleMenuState();
      break;
      
    case STATE_COMMLOCK:
      handleCommlockState();
      break;
      
    case STATE_ERROR:
      handleErrorState();
      break;
      
    case STATE_RECOVERY:
      handleRecoveryState();
      break;
  }
}

void handleSplashState() {
  if (millis() - splashStartTime >= SPLASH_DURATION) {
    Logger::info(LOG_CAT_SYSTEM, "Splash screen finished, transitioning to home");
    currentState = STATE_HOME;
    if (!startVideo(HOME_VIDEO)) {
      reportError(ERROR_VIDEO_FILE_NOT_FOUND, "Cannot load home video");
    }
  }
}

void handleMenuState() {
  // Handle menu button to exit
  if (menuButton.wasPressed) {
    menuButton.wasPressed = false;
    Logger::info(LOG_CAT_SYSTEM, "Exiting menu, returning to home");
    currentState = STATE_HOME;
    returnToHome();
    return;
  }
  
  // Handle video selection
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    if (videoButtons[i].wasPressed) {
      videoButtons[i].wasPressed = false;
      Logger::info(LOG_CAT_VIDEO, "Video %d selected from menu", i + 1);
      currentState = STATE_PLAYING_VIDEO;
      if (!startVideo(videoDatabase[i].filename, i)) {
        Logger::error(LOG_CAT_VIDEO, "Failed to start video %d", i + 1);
        currentState = STATE_HOME;
        returnToHome();
      }
      return;
    }
  }
}

void handleVideoState() {
  // Handle menu button
  if (menuButton.wasPressed) {
    menuButton.wasPressed = false;
    Logger::info(LOG_CAT_SYSTEM, "Menu button pressed during video playback");
    stopVideo();
    currentState = STATE_MENU;
    drawMenu();
    return;
  }
  
  // Handle direct video selection (only if not in menu)
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    if (videoButtons[i].wasPressed) {
      videoButtons[i].wasPressed = false;
      if (currentVideoIndex != i) {
        Logger::info(LOG_CAT_VIDEO, "Direct video selection: %d", i + 1);
        currentState = STATE_PLAYING_VIDEO;
        if (!startVideo(videoDatabase[i].filename, i)) {
          Logger::error(LOG_CAT_VIDEO, "Failed to start video %d", i + 1);
          returnToHome();
        }
      }
      return;
    }
  }
  
  // Handle video playback
  if (videoPlaying) {
    unsigned long now = millis();
    if (now - lastFrameTime >= FRAME_INTERVAL) {
      if (mjpegFile.available() && mjpeg.readMjpegBuf()) {
        mjpeg.drawJpg();
        lastFrameTime = now;
        frameCount++;
      } else {
        // Video finished, return to home video
        Logger::info(LOG_CAT_VIDEO, "Video finished, returning to home");
        returnToHome();
      }
    }
  }
}

void handleCommlockState() {
  // Check if Commlock display should end
  if (millis() - commlockStartTime >= COMMLOCK_DURATION) {
    Logger::info(LOG_CAT_SYSTEM, "Commlock display finished, returning to previous state");
    returnToHome();
    return;
  }
  
  // Update Commlock time every minute
  updateCommlock();
  
  // Handle any button press to exit early
  if (menuButton.wasPressed) {
    menuButton.wasPressed = false;
    Logger::info(LOG_CAT_SYSTEM, "Menu button pressed during Commlock, returning to home");
    returnToHome();
    return;
  }
  
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    if (videoButtons[i].wasPressed) {
      videoButtons[i].wasPressed = false;
      Logger::info(LOG_CAT_SYSTEM, "Video button pressed during Commlock, returning to home");
      returnToHome();
      return;
    }
  }
}

void handleErrorState() {
  // Check for recovery attempts
  if (millis() - errorStartTime > ERROR_RECOVERY_DELAY) {
    if (retryCount < MAX_RETRY_ATTEMPTS) {
      currentState = STATE_RECOVERY;
      retryCount++;
      Logger::warn(LOG_CAT_SYSTEM, "Attempting recovery #%d...", retryCount);
    } else {
      // Final error state - only manual reset can recover
      Logger::fatal(LOG_CAT_SYSTEM, "System in unrecoverable error state. Manual reset required.");
    }
  }
}

void handleRecoveryState() {
  // Attempt to recover from error
  if (attemptRecovery()) {
    currentState = STATE_HOME;
    retryCount = 0;
    lastError = ERROR_NONE;
    Logger::info(LOG_CAT_SYSTEM, "Recovery successful");
    returnToHome();
  } else {
    currentState = STATE_ERROR;
    errorStartTime = millis();
    Logger::error(LOG_CAT_SYSTEM, "Recovery failed");
  }
}

void drawSplashScreen() {
  Logger::info(LOG_CAT_DISPLAY, "Drawing splash screen");
  display.clear();
  display.setTextColor(WHITE);
  
  // Main title
  display.drawCenteredText("MJPEG", display.height() / 2 - 20, 2);
  display.drawCenteredText("Player", display.height() / 2, 2);
  
  // Loading
  display.drawCenteredText("Loading...", display.height() / 2 + 30, 1);
}

void drawMenu() {
  Logger::debug(LOG_CAT_DISPLAY, "Drawing menu screen");
  display.clear();
  display.setTextColor(CYAN);
  display.setCursor(10, 5);
  display.println("=== VIDEO MENU ===");
  
  // Video list
  display.setTextColor(WHITE);
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    display.setCursor(10, 20 + i * 12);
    display.print("[");
    display.print(i + 1);
    display.print("] ");
    display.println(videoDatabase[i].displayName);
  }
  
  // Special feature hint
  display.setCursor(10, 20 + TOTAL_VIDEOS * 12 + 5);
  display.setTextColor(YELLOW);
  display.println("Button 9 = Commlock");
  
  // Instructions
  display.setCursor(10, 20 + TOTAL_VIDEOS * 12 + 20);
  display.setTextColor(YELLOW);
  display.println("Menu = Exit");
  
  // Performance info
  drawPerformanceInfo();
}

void drawCommlock() {
  Logger::debug(LOG_CAT_DISPLAY, "Drawing Commlock display");
  display.clear();
  
  // Create a deep blue color for Commlock atmosphere
  uint16_t commlockBlue = display._gfx->color565(100, 200, 255);
  display.setTextColor(commlockBlue);
  
  // "COMMLOCK" title
  display.setTextSize(1);
  display.drawCenteredText("COMMLOCK", 30);
  
  // Large time display
  display.setTextSize(2);
  char timeStr[10];
  sprintf(timeStr, "%02d %02d", commlockHour, commlockMinute);
  display.drawCenteredText(timeStr, 80);
  
  // Subtitle
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.drawCenteredText("1999: A Space Odyssey", 120);
  
  // Status
  display.setCursor(10, display.height() - 20);
  display.setTextColor(CYAN);
  display.print("Press any button to exit");
}

void drawErrorScreen(ErrorCode error, const char* details) {
  Logger::error(LOG_CAT_DISPLAY, "Drawing error screen: %s", getErrorString(error));
  display.clear();
  display.setTextColor(RED);
  display.setCursor(10, 20);
  display.println("SYSTEM ERROR:");
  
  display.setTextColor(WHITE);
  display.setCursor(10, 40);
  display.print("Code: ");
  display.println((int)error);
  
  display.setCursor(10, 55);
  display.print("Error: ");
  display.println(getErrorString(error));
  
  if (details) {
    display.setCursor(10, 85);
    display.setTextColor(YELLOW);
    display.println("Details:");
    display.setTextColor(WHITE);
    display.setCursor(10, 100);
    display.println(details);
  }
  
  display.setCursor(10, 130);
  display.setTextColor(CYAN);
  display.print("Retry: ");
  display.print(retryCount);
  display.print("/");
  display.println(MAX_RETRY_ATTEMPTS);
}

void drawPerformanceInfo() {
  display.setTextColor(GREEN);
  display.setCursor(10, display.height() - 20);
  display.print("FPS: ");
  display.print(currentFps, 1);
  display.print(" | Mem: ");
  display.print(ESP.getFreeHeap());
  display.print("B");
}

bool startVideo(const char* filename, int videoIndex) {
  Logger::info(LOG_CAT_VIDEO, "Starting video: %s (index: %d)", filename, videoIndex);
  stopVideo(); // Make sure previous video is stopped
  
  mjpegFile = LittleFS.open(filename);
  if (!mjpegFile || mjpegFile.isDirectory() || mjpegFile.size() < 1024) {
    Logger::error(LOG_CAT_VIDEO, "Cannot open file: %s", filename);
    reportError(ERROR_VIDEO_FILE_NOT_FOUND, filename);
    return false;
  }
  
  mjpeg.setup(&mjpegFile, mjpegBuffer, jpegDrawCallback, true, 0, 0, display.width(), display.height());
  mjpeg.resetScale();
  
  currentVideoIndex = videoIndex;
  videoPlaying = true;
  lastFrameTime = millis();
  frameCount = 0;
  
  // Start audio if it's a numbered video (not home or menu)
  if (videoIndex >= 0) {
    startAudio(videoDatabase[videoIndex].audioTrack);
  }
  
  // Start LED if configured for this video
  if (videoIndex >= 0 && videoIndex < TOTAL_VIDEOS) {
    startLED(videoIndex);
  }
  
  Logger::info(LOG_CAT_VIDEO, "Playing: %s", filename);
  if (videoIndex >= 0) {
    Logger::info(LOG_CAT_VIDEO, "Video: %s with audio track %d", videoDatabase[videoIndex].displayName, videoDatabase[videoIndex].audioTrack);
  }
  
  return true;
}

void stopVideo() {
  if (mjpegFile) {
    Logger::debug(LOG_CAT_VIDEO, "Stopping video playback");
    mjpegFile.close();
  }
  videoPlaying = false;
  stopAudio();
  stopLED();
}

void returnToHome() {
  Logger::info(LOG_CAT_SYSTEM, "Returning to home video");
  currentVideoIndex = -1;
  currentState = STATE_HOME;
  if (!startVideo(HOME_VIDEO)) {
    reportError(ERROR_VIDEO_FILE_NOT_FOUND, "Cannot load home video");
  }
}

void startAudio(uint8_t trackNumber) {
  if (player.available()) {
    player.play(trackNumber);
    audioPlaying = true;
    currentAudioTrack = trackNumber;
    Logger::info(LOG_CAT_AUDIO, "Playing audio track: %04d.mp3", trackNumber);
  } else {
    Logger::error(LOG_CAT_AUDIO, "DFPlayer not available for audio");
    reportError(ERROR_AUDIO_TRACK_NOT_FOUND, "DFPlayer not available");
  }
}

void stopAudio() {
  if (player.available() && audioPlaying) {
    player.stop();
    audioPlaying = false;
    currentAudioTrack = 0;
    Logger::debug(LOG_CAT_AUDIO, "Audio stopped");
  }
}

void startCommlock() {
  Logger::info(LOG_CAT_SYSTEM, "Starting Commlock display");
  
  // Stop current video if playing
  stopVideo();
  
  // Initialize Commlock time with random values
  commlockHour = random(0, 24);
  commlockMinute = random(0, 60);
  commlockStartTime = millis();
  lastCommlockUpdate = millis();
  
  // Change state and draw
  currentState = STATE_COMMLOCK;
  drawCommlock();
  
  Logger::info(LOG_CAT_SYSTEM, "Commlock initialized: %02d:%02d", commlockHour, commlockMinute);
}

void updateCommlock() {
  unsigned long now = millis();
  
  // Update time every minute (60000ms)
  if (now - lastCommlockUpdate >= 60000) {
    lastCommlockUpdate = now;
    
    commlockMinute++;
    if (commlockMinute >= 60) {
      commlockMinute = 0;
      commlockHour++;
      if (commlockHour >= 24) {
        commlockHour = 0;
      }
    }
    
    Logger::debug(LOG_CAT_SYSTEM, "Commlock time updated: %02d:%02d", commlockHour, commlockMinute);
    drawCommlock();
  }
}

void startLED(uint8_t videoIndex) {
  if (videoIndex >= TOTAL_VIDEOS) return;
  
  LEDConfig& config = ledConfigs[videoIndex];
  if (!config.enabled) return;
  
  Logger::info(LOG_CAT_SYSTEM, "Starting LED for video %d (duration: %d ms, interval: %d ms)", 
               videoIndex + 1, config.blinkDuration, config.blinkInterval);
  
  ledActive = true;
  ledStartTime = millis();
  lastLedBlink = millis();
  ledState = true;
  digitalWrite(LED_PIN, HIGH);
}

void updateLED() {
  if (!ledActive) return;
  
  unsigned long now = millis();
  
  // Check if LED should stop
  if (now - ledStartTime >= ledConfigs[currentVideoIndex].blinkDuration) {
    stopLED();
    return;
  }
  
  // Toggle LED based on interval
  if (now - lastLedBlink >= ledConfigs[currentVideoIndex].blinkInterval) {
    lastLedBlink = now;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  }
}

void stopLED() {
  if (ledActive) {
    Logger::debug(LOG_CAT_SYSTEM, "Stopping LED");
    ledActive = false;
    digitalWrite(LED_PIN, LOW);
  }
}

void configureLED(uint8_t videoIndex, uint16_t duration, uint16_t interval, bool enabled) {
  if (videoIndex >= TOTAL_VIDEOS) return;
  
  LEDConfig& config = ledConfigs[videoIndex];
  config.videoIndex = videoIndex;
  config.blinkDuration = duration;
  config.blinkInterval = interval;
  config.enabled = enabled;
  
  Logger::info(LOG_CAT_SYSTEM, "LED configured for video %d: duration=%d ms, interval=%d ms, enabled=%s", 
               videoIndex + 1, duration, interval, enabled ? "true" : "false");
}

void reportError(ErrorCode error, const char* details) {
  lastError = error;
  currentState = STATE_ERROR;
  errorStartTime = millis();
  Logger::error(LOG_CAT_SYSTEM, "Error %d: %s - %s", (int)error, getErrorString(error), details ? details : "No details");
  drawErrorScreen(error, details);
}

bool attemptRecovery() {
  Logger::info(LOG_CAT_SYSTEM, "Attempting recovery for error: %s", getErrorString(lastError));
  
  // Try to recover based on error type
  switch (lastError) {
    case ERROR_DISPLAY_INIT_FAILED:
      return display.begin();
      
    case ERROR_FILESYSTEM_INIT_FAILED:
      return LittleFS.begin();
      
    case ERROR_MEMORY_ALLOCATION_FAILED:
      deallocateBuffer();
      return allocateBuffer();
      
    case ERROR_DFPLAYER_INIT_FAILED:
      return player.begin(Serial2);
      
    case ERROR_VIDEO_FILE_NOT_FOUND:
    case ERROR_VIDEO_FILE_CORRUPTED:
      // Try to return to home
      returnToHome();
      return true;
      
    default:
      return false;
  }
}

const char* getErrorString(ErrorCode error) {
  switch (error) {
    case ERROR_NONE: return "No Error";
    case ERROR_DISPLAY_INIT_FAILED: return "Display Init Failed";
    case ERROR_FILESYSTEM_INIT_FAILED: return "Filesystem Init Failed";
    case ERROR_MEMORY_ALLOCATION_FAILED: return "Memory Allocation Failed";
    case ERROR_DFPLAYER_INIT_FAILED: return "DFPlayer Init Failed";
    case ERROR_VIDEO_FILE_NOT_FOUND: return "Video File Not Found";
    case ERROR_VIDEO_FILE_CORRUPTED: return "Video File Corrupted";
    case ERROR_AUDIO_TRACK_NOT_FOUND: return "Audio Track Not Found";
    case ERROR_SYSTEM_OVERLOAD: return "System Overload";
    default: return "Unknown Error";
  }
}

void updatePerformanceMetrics() {
  unsigned long now = millis();
  
  // Update FPS every second
  if (now - lastFpsCheck >= 1000) {
    currentFps = (float)frameCount * 1000.0f / (now - lastFpsCheck);
    frameCount = 0;
    lastFpsCheck = now;
    
    // Log performance metrics
    Logger::debug(LOG_CAT_PERFORMANCE, "FPS: %.1f, Free Memory: %d bytes", currentFps, ESP.getFreeHeap());
  }
  
  // Track maximum loop time
  unsigned long loopTime = micros() - loopStartTime;
  if (loopTime > maxLoopTime) {
    maxLoopTime = loopTime;
  }
  
  // Check for system overload
  if (loopTime > 5000) { // More than 5ms
    Logger::warn(LOG_CAT_PERFORMANCE, "Loop time exceeded 5ms: %lu μs", loopTime);
    reportError(ERROR_SYSTEM_OVERLOAD, "Loop time exceeded 5ms");
  }
}

void printSystemStatus() {
  Logger::info(LOG_CAT_SYSTEM, "=== System Status ===");
  Logger::info(LOG_CAT_SYSTEM, "Free Memory: %d bytes", ESP.getFreeHeap());
  Logger::info(LOG_CAT_SYSTEM, "Display: %dx%d", display.width(), display.height());
  Logger::info(LOG_CAT_SYSTEM, "Total Videos: %d", TOTAL_VIDEOS);
  Logger::info(LOG_CAT_SYSTEM, "Target FPS: %d", TARGET_FPS);
  Logger::info(LOG_CAT_SYSTEM, "MJPEG Buffer: %d bytes", MJPEG_BUFFER_SIZE);
  Logger::info(LOG_CAT_SYSTEM, "LED Pin: %d", LED_PIN);
  Logger::info(LOG_CAT_SYSTEM, "====================");
}

int jpegDrawCallback(JPEGDRAW* pDraw) {
  display.drawJpegFrame(pDraw);
  return 1;
}