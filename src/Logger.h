// src\Logger.h -- logger configuration
#pragma once // Copyright 2025 Alex0vSky (https://github.com/Alex0vSky)

/**
 * @file Logger.h
 * @brief Logging macro and configuration for debug and release builds
 */

#ifdef DEBUG
// Global logger instance for serial output
Device::LogToMonitor Serial;
/**
 * @def LOG
 * @brief Macro for logging messages using printf-style formatting
 */
#define LOG(...) Serial.printf(  __VA_ARGS__ )
#else // DEBUG
// Clean macro for release builds (no logging)
#define LOG(...) do{} while( false )
#endif // DEBUG
