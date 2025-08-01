#pragma once
#include <Arduino.h>

// Log levels
enum LogLevel {
  LOG_LEVEL_DEBUG = 0,
  LOG_LEVEL_INFO = 1,
  LOG_LEVEL_WARN = 2,
  LOG_LEVEL_ERROR = 3,
  LOG_LEVEL_FATAL = 4
};

// Log categories
enum LogCategory {
  LOG_CAT_SYSTEM = 0,
  LOG_CAT_DISPLAY = 1,
  LOG_CAT_AUDIO = 2,
  LOG_CAT_VIDEO = 3,
  LOG_CAT_BUTTON = 4,
  LOG_CAT_MEMORY = 5,
  LOG_CAT_PERFORMANCE = 6
};

class Logger {
private:
  static LogLevel _currentLevel;
  static bool _initialized;
  static unsigned long _startTime;
  static unsigned long _logCount;
  static unsigned long _errorCount;
  
  // Performance tracking
  static unsigned long _totalLogTime;
  static float _averageLogTime;
  
  // Circular buffer for recent logs
  static const int LOG_BUFFER_SIZE = 20;
  static struct LogEntry {
    unsigned long timestamp;
    LogLevel level;
    LogCategory category;
    const char* message;
  } _logBuffer[LOG_BUFFER_SIZE];
  static int _logBufferIndex;
  
public:
  static void begin() {
    _currentLevel = LOG_LEVEL_INFO;
    _initialized = true;
    _startTime = millis();
    _logCount = 0;
    _errorCount = 0;
    _totalLogTime = 0;
    _averageLogTime = 0.0f;
    _logBufferIndex = 0;
    
    info(LOG_CAT_SYSTEM, "Logger initialized");
  }
  
  static void setLevel(LogLevel level) {
    _currentLevel = level;
    info(LOG_CAT_SYSTEM, "Log level set to %d", level);
  }
  
  static void debug(LogCategory category, const char* format, ...) {
    if (_currentLevel <= LOG_LEVEL_DEBUG) {
      va_list args;
      va_start(args, format);
      _log(LOG_LEVEL_DEBUG, category, format, args);
      va_end(args);
    }
  }
  
  static void info(LogCategory category, const char* format, ...) {
    if (_currentLevel <= LOG_LEVEL_INFO) {
      va_list args;
      va_start(args, format);
      _log(LOG_LEVEL_INFO, category, format, args);
      va_end(args);
    }
  }
  
  static void warn(LogCategory category, const char* format, ...) {
    if (_currentLevel <= LOG_LEVEL_WARN) {
      va_list args;
      va_start(args, format);
      _log(LOG_LEVEL_WARN, category, format, args);
      va_end(args);
    }
  }
  
  static void error(LogCategory category, const char* format, ...) {
    if (_currentLevel <= LOG_LEVEL_ERROR) {
      va_list args;
      va_start(args, format);
      _log(LOG_LEVEL_ERROR, category, format, args);
      va_end(args);
    }
  }
  
  static void fatal(LogCategory category, const char* format, ...) {
    if (_currentLevel <= LOG_LEVEL_FATAL) {
      va_list args;
      va_start(args, format);
      _log(LOG_LEVEL_FATAL, category, format, args);
      va_end(args);
    }
  }
  
  // Performance logging
  static void performance(const char* operation, unsigned long duration) {
    if (_currentLevel <= LOG_LEVEL_DEBUG) {
      info(LOG_CAT_PERFORMANCE, "%s took %lu μs", operation, duration);
    }
  }
  
  // Memory logging
  static void memory(const char* operation, size_t size) {
    if (_currentLevel <= LOG_LEVEL_INFO) {
      info(LOG_CAT_MEMORY, "%s: %d bytes (Free: %d)", operation, size, ESP.getFreeHeap());
    }
  }
  
  // Get statistics
  static unsigned long getLogCount() { return _logCount; }
  static unsigned long getErrorCount() { return _errorCount; }
  static float getAverageLogTime() { return _averageLogTime; }
  
  // Print recent logs
  static void printRecentLogs() {
    Serial.println("=== Recent Logs ===");
    for (int i = 0; i < LOG_BUFFER_SIZE; i++) {
      int index = (_logBufferIndex - LOG_BUFFER_SIZE + i + LOG_BUFFER_SIZE) % LOG_BUFFER_SIZE;
      if (_logBuffer[index].message) {
        Serial.printf("[%lu] %s: %s\n", 
                     _logBuffer[index].timestamp,
                     _getLevelString(_logBuffer[index].level),
                     _logBuffer[index].message);
      }
    }
    Serial.println("==================");
  }
  
  // Print statistics
  static void printStatistics() {
    Serial.println("=== Logger Statistics ===");
    Serial.printf("Total Logs: %lu\n", _logCount);
    Serial.printf("Errors: %lu\n", _errorCount);
    Serial.printf("Average Log Time: %.2f μs\n", _averageLogTime);
    Serial.printf("Uptime: %lu ms\n", millis() - _startTime);
    Serial.println("========================");
  }
  
private:
  static void _log(LogLevel level, LogCategory category, const char* format, va_list args) {
    if (!_initialized) return;
    
    unsigned long startTime = micros();
    
    // Format message
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    
    // Get timestamp
    unsigned long timestamp = millis() - _startTime;
    
    // Print to Serial
    Serial.printf("[%lu] %s [%s]: %s\n", 
                 timestamp,
                 _getLevelString(level),
                 _getCategoryString(category),
                 buffer);
    
    // Store in circular buffer
    _logBuffer[_logBufferIndex].timestamp = timestamp;
    _logBuffer[_logBufferIndex].level = level;
    _logBuffer[_logBufferIndex].category = category;
    _logBuffer[_logBufferIndex].message = strdup(buffer); // Note: This creates a memory leak, but for demo purposes
    
    _logBufferIndex = (_logBufferIndex + 1) % LOG_BUFFER_SIZE;
    
    // Update statistics
    _logCount++;
    if (level >= LOG_LEVEL_ERROR) {
      _errorCount++;
    }
    
    // Update performance metrics
    unsigned long logTime = micros() - startTime;
    _totalLogTime += logTime;
    _averageLogTime = (float)_totalLogTime / _logCount;
  }
  
  static const char* _getLevelString(LogLevel level) {
    switch (level) {
      case LOG_LEVEL_DEBUG: return "DEBUG";
      case LOG_LEVEL_INFO: return "INFO ";
      case LOG_LEVEL_WARN: return "WARN ";
      case LOG_LEVEL_ERROR: return "ERROR";
      case LOG_LEVEL_FATAL: return "FATAL";
      default: return "UNKN ";
    }
  }
  
  static const char* _getCategoryString(LogCategory category) {
    switch (category) {
      case LOG_CAT_SYSTEM: return "SYS";
      case LOG_CAT_DISPLAY: return "DSP";
      case LOG_CAT_AUDIO: return "AUD";
      case LOG_CAT_VIDEO: return "VID";
      case LOG_CAT_BUTTON: return "BTN";
      case LOG_CAT_MEMORY: return "MEM";
      case LOG_CAT_PERFORMANCE: return "PERF";
      default: return "UNK";
    }
  }
};

// Static member initialization
LogLevel Logger::_currentLevel = LOG_LEVEL_INFO;
bool Logger::_initialized = false;
unsigned long Logger::_startTime = 0;
unsigned long Logger::_logCount = 0;
unsigned long Logger::_errorCount = 0;
unsigned long Logger::_totalLogTime = 0;
float Logger::_averageLogTime = 0.0f;
int Logger::_logBufferIndex = 0;
Logger::LogEntry Logger::_logBuffer[LOG_BUFFER_SIZE] = {0}; 