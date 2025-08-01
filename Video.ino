#include "DisplayManager.h"
#include <LittleFS.h>
#include "MjpegClass.h"
#include "DFRobotDFPlayerMini.h"

// ===== GLOBAL DISPLAY MANAGER =====
DisplayManager display;

// ===== DFPLAYER MINI SETUP =====
#include <SoftwareSerial.h>
SoftwareSerial mySerial(9, 10); // RX=9, TX=10
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

// System states
enum SystemState {
  STATE_SPLASH,
  STATE_HOME,
  STATE_MENU,
  STATE_PLAYING_VIDEO,
  STATE_ERROR
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

// Global system variables
SystemState currentState = STATE_SPLASH;
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
void initializeButtons();
void initializeDFPlayer();
void updateButtons();
bool isButtonPressed(ButtonState& button, uint8_t pin);
void handleStateMachine();
void handleSplashState();
void handleMenuState();
void handleVideoState();
void drawSplashScreen();
void drawMenu();
void drawErrorScreen(const char* error);
bool startVideo(const char* filename, int videoIndex = -1);
void stopVideo();
void returnToHome();
void playButtonTone();
void startAudio(uint8_t trackNumber);
void stopAudio();
int jpegDrawCallback(JPEGDRAW* pDraw);

void setup() {
  Serial.begin(115200);
  Serial.println("🚀 Starting MJPEG Player with Audio...");
  
  initializeSystem();
  splashStartTime = millis();
}

void loop() {
  updateButtons();
  handleStateMachine();
  delay(1);
}

void initializeSystem() {
  // Initialize display first
  if (!display.begin()) {
    Serial.println("❌ Display initialization failed");
    while (1) delay(100);
  }
  
  initializeButtons();
  initializeDFPlayer();
  drawSplashScreen();
  initializeFileSystem();
  
  if (!allocateBuffer()) {
    currentState = STATE_ERROR;
    drawErrorScreen("Memory allocation failed");
    return;
  }
  
  Serial.println("✅ System initialized successfully");
}

void initializeDFPlayer() {
  mySerial.begin(9600, SERIAL_8N1, 9, 10); // RX=9, TX=10
  
  if (player.begin(mySerial)) {
    Serial.println("🎵 DFPlayer ready");
    player.volume(25); // Volume from 0 to 30
    // No track plays at startup
  } else {
    Serial.println("❌ DFPlayer not detected");
    // Don't halt execution, just log the error
    // while (true); // Uncomment if you want to halt on DFPlayer failure
  }
}

void initializeFileSystem() {
  if (!LittleFS.begin()) {
    Serial.println("❌ FileSystem initialization failed");
    currentState = STATE_ERROR;
    drawErrorScreen("FileSystem init failed");
    return;
  }
  Serial.println("✅ FileSystem initialized");
}

bool allocateBuffer() {
  mjpegBuffer = (uint8_t*)heap_caps_malloc(MJPEG_BUFFER_SIZE, MALLOC_CAP_8BIT);
  if (!mjpegBuffer) {
    Serial.println("❌ MJPEG buffer allocation failed");
    return false;
  }
  Serial.printf("✅ Allocated %d bytes for MJPEG buffer\n", MJPEG_BUFFER_SIZE);
  return true;
}

void initializeButtons() {
  pinMode(MENU_BUTTON_PIN, INPUT_PULLUP);
  menuButton = {HIGH, HIGH, 0, false};
  
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    videoButtons[i] = {HIGH, HIGH, 0, false};
  }
  Serial.println("✅ Buttons initialized");
}

void updateButtons() {
  // Update menu button
  if (isButtonPressed(menuButton, MENU_BUTTON_PIN)) {
    playButtonTone();
  }
  
  // Update video buttons
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    if (isButtonPressed(videoButtons[i], BUTTON_PINS[i])) {
      playButtonTone();
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
      
    case STATE_ERROR:
      // Stay in error state
      break;
  }
}

void handleSplashState() {
  if (millis() - splashStartTime >= SPLASH_DURATION) {
    currentState = STATE_HOME;
    if (!startVideo(HOME_VIDEO)) {
      currentState = STATE_ERROR;
      drawErrorScreen("Cannot load home video");
    }
  }
}

void handleMenuState() {
  // Handle menu button to exit
  if (menuButton.wasPressed) {
    menuButton.wasPressed = false;
    currentState = STATE_HOME;
    returnToHome();
    return;
  }
  
  // Handle video selection
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    if (videoButtons[i].wasPressed) {
      videoButtons[i].wasPressed = false;
      currentState = STATE_PLAYING_VIDEO;
      if (!startVideo(videoDatabase[i].filename, i)) {
        Serial.printf("❌ Failed to start video %d\n", i + 1);
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
        currentState = STATE_PLAYING_VIDEO;
        if (!startVideo(videoDatabase[i].filename, i)) {
          Serial.printf("❌ Failed to start video %d\n", i + 1);
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
      } else {
        // Video finished, return to home video
        Serial.println("📺 Video finished, returning to home");
        returnToHome();
      }
    }
  }
}

void drawSplashScreen() {
  display.clear();
  display.setTextColor(WHITE);
  
  // Main title
  display.drawCenteredText("MJPEG", display.height() / 2 - 20, 2);
  display.drawCenteredText("Player", display.height() / 2, 2);
  
  // Loading
  display.drawCenteredText("Loading...", display.height() / 2 + 30, 1);
}

void drawMenu() {
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
  
  // Instructions
  display.setCursor(10, 20 + TOTAL_VIDEOS * 12 + 10);
  display.setTextColor(YELLOW);
  display.println("Menu = Exit");
}

void drawErrorScreen(const char* error) {
  display.clear();
  display.setTextColor(RED);
  display.setCursor(10, 40);
  display.println("ERROR:");
  
  display.setTextSize(1);
  display.setCursor(10, 70);
  display.setTextColor(WHITE);
  display.println(error);
  
  Serial.printf("❌ Error: %s\n", error);
}

bool startVideo(const char* filename, int videoIndex) {
  stopVideo(); // Make sure previous video is stopped
  
  mjpegFile = LittleFS.open(filename);
  if (!mjpegFile || mjpegFile.isDirectory() || mjpegFile.size() < 1024) {
    Serial.printf("⚠️ Cannot open file: %s\n", filename);
    return false;
  }
  
  mjpeg.setup(&mjpegFile, mjpegBuffer, jpegDrawCallback, true, 0, 0, display.width(), display.height());
  mjpeg.resetScale();
  
  currentVideoIndex = videoIndex;
  videoPlaying = true;
  lastFrameTime = millis();
  
  // Start audio if it's a numbered video (not home or menu)
  if (videoIndex >= 0) {
    startAudio(videoDatabase[videoIndex].audioTrack);
  }
  
  Serial.printf("▶️ Playing: %s", filename);
  if (videoIndex >= 0) {
    Serial.printf(" (%s) with audio track %d", videoDatabase[videoIndex].displayName, videoDatabase[videoIndex].audioTrack);
  }
  Serial.println();
  
  return true;
}

void stopVideo() {
  if (mjpegFile) {
    mjpegFile.close();
  }
  videoPlaying = false;
  stopAudio();
}

void returnToHome() {
  currentVideoIndex = -1;
  currentState = STATE_HOME;
  if (!startVideo(HOME_VIDEO)) {
    currentState = STATE_ERROR;
    drawErrorScreen("Cannot load home video");
  }
}

void startAudio(uint8_t trackNumber) {
  if (player.available()) {
    player.play(trackNumber);
    audioPlaying = true;
    currentAudioTrack = trackNumber;
    Serial.printf("🎵 Playing audio track: %04d.mp3\n", trackNumber);
  } else {
    Serial.println("⚠️ DFPlayer not available for audio");
  }
}

void stopAudio() {
  if (player.available() && audioPlaying) {
    player.stop();
    audioPlaying = false;
    currentAudioTrack = 0;
    Serial.println("🔇 Audio stopped");
  }
}

int jpegDrawCallback(JPEGDRAW* pDraw) {
  display.drawJpegFrame(pDraw);
  return 1;
}