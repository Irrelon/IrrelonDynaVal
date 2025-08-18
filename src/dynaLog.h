#pragma once
#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdio>
#endif

#include "DynaVal.h"

// Buffer size for serializing JSON values
#define JSON_LOG_BUFFER_SIZE 2048

inline bool dynaLogIsEnabled = true;
inline int dynaLogIndentLevel = 0;

inline std::string dynaValLogGetIndentString(const char indentChar = '\t') {
    return std::string(dynaLogIndentLevel, indentChar);
}

// Internal helper
inline void dynaLogImplementation(const char* format, va_list args) {
#ifdef ARDUINO
	Serial.print(dynaValLogGetIndentString().c_str());
	Serial.print("[LOG] ");
	char buffer[128];
	vsnprintf(buffer, sizeof(buffer), format, args);
	Serial.printf(buffer);
	Serial.println();
#else
	printf("%s[LOG] ", dynaValLogGetIndentString().c_str());
	vprintf(format, args);
	printf("\n");
#endif
}

inline void dynaLogForce(const char* format, ...) {
	va_list args;
	va_start(args, format);
	dynaLogImplementation(format, args);
	va_end(args);
}

inline void dynaLogLn(const char* format, const DynaVal& value) {
	if (!dynaLogIsEnabled) return;
#ifdef ARDUINO
	Serial.print(dynaValLogGetIndentString().c_str());
	Serial.print("[LOG] ");
	Serial.print(format);
	Serial.println(value.toJson().c_str());
#else
	printf("%s[LOG] %s %s\n", dynaValLogGetIndentString().c_str(), format, value.toJson().c_str());
#endif
}

// Variadic wrapper
inline void dynaLogLn(const char* format, ...) {
	if (!dynaLogIsEnabled) return;
	va_list args;
	va_start(args, format);
	dynaLogImplementation(format, args);
	va_end(args);
}

// Variadic template for standard logging
template<typename... Args>
void dynaLog(const char* format, Args... args) {
    if (!dynaLogIsEnabled) return;
#ifdef ARDUINO
	Serial.printf(format, args...);
#else
	printf(format, args...);
#endif
}

// Variadic template for standard logging
template<typename... Args>
void dynaLogNoBreak(const char* format, Args... args) {
    if (!dynaLogIsEnabled) return;
#ifdef ARDUINO
	Serial.print(dynaValLogGetIndentString().c_str());
	Serial.printf("[LOG] ");
	Serial.printf(format, args...);
#else
	printf("%s[LOG] ", dynaValLogGetIndentString().c_str());
	printf(format, args...);
#endif
}

inline void dynaLogBreak() {
if (!dynaLogIsEnabled) return;
#ifdef ARDUINO
	Serial.println();
#else
	printf("\n");
#endif
}

inline void dynaLogEnabled(const bool enable) {
    dynaLogIsEnabled = enable;
}

inline void dynaLogOn () {
    dynaLogIsEnabled = true;
}

inline void dynaLogOff () {
    dynaLogIsEnabled = false;
}

inline void dynaLogIndent () {
    dynaLogIndentLevel++;
}

inline void dynaLogDedent () {
    if (dynaLogIndentLevel == 0) return;
    dynaLogIndentLevel--;
}