#pragma once
#ifdef ARDUINO
  #include <Arduino.h>
#else
  #include <cstdio>
#endif

#include <cstdarg>
#include <string>
#include <utility>   // std::forward

#include "DynaVal.h"

namespace Irrelon {

inline bool dynaLogIsEnabled = true;
inline int  dynaLogIndentLevel = 0;

inline std::string dynaValLogGetIndentString(const char indentChar = '\t') {
    return std::string(dynaLogIndentLevel, indentChar);
}

// ---------- internal helpers ----------

// Format using va_list onto heap-backed std::string (no stack buffers).
inline std::string vformatString(const char* fmt, va_list ap_in) {
    if (!fmt) return {};

    va_list ap1;
    va_list ap2;
    va_copy(ap1, ap_in);
    va_copy(ap2, ap_in);

    // Measure
    int needed = vsnprintf(nullptr, 0, fmt, ap1);
    va_end(ap1);
    if (needed <= 0) {
        va_end(ap2);
        return {};
    }

    // Render
    std::string out;
    out.resize(static_cast<size_t>(needed)); // no +1; we'll write N bytes
    vsnprintf(out.data(), static_cast<size_t>(needed) + 1, fmt, ap2);
    va_end(ap2);

    return out;
}

// Variadic formatter (non-va_list) – also heap-backed.
template <typename... Args>
inline std::string formatString(const char* fmt, Args&&... args) {
    if (!fmt) return {};

    // First pass: measure
    int needed = snprintf(nullptr, 0, fmt, std::forward<Args>(args)...);
    if (needed <= 0) return {};

    // Second pass: render
    std::string out;
    out.resize(static_cast<size_t>(needed));
    snprintf(out.data(), static_cast<size_t>(needed) + 1, fmt, std::forward<Args>(args)...);
    return out;
}

// Write a whole line with prefix safely (no format interpretation of message).
inline void writeLogLine(const std::string& msg) {
#ifdef ARDUINO
    Serial.print(dynaValLogGetIndentString().c_str());
    Serial.print("[LOG] ");
    Serial.print(msg.c_str());
    Serial.print('\n');
#else
    fputs(dynaValLogGetIndentString().c_str(), stdout);
    fputs("[LOG] ", stdout);
    fputs(msg.c_str(), stdout);
    fputc('\n', stdout);
#endif
}

// Write log without trailing newline (still with prefix).
inline void writeLogNoBreak(const std::string& msg) {
#ifdef ARDUINO
    Serial.print(dynaValLogGetIndentString().c_str());
    Serial.print("[LOG] ");
    Serial.print(msg.c_str());
#else
    fputs(dynaValLogGetIndentString().c_str(), stdout);
    fputs("[LOG] ", stdout);
    fputs(msg.c_str(), stdout);
#endif
}

// ---------- public API (same names as yours) ----------

inline void dynaLogImplementation(const char* format, va_list args) {
    // NOTE: no stack buffers, no printf(format-as-message)
    std::string rendered = vformatString(format, args);
    writeLogLine(rendered);
}

inline void dynaLogForce(const char* format, ...) {
    va_list args;
    va_start(args, format);
    std::string rendered = vformatString(format, args);
    va_end(args);
    writeLogLine(rendered);
}

inline void dynaLogLn(const char* format, const DynaVal& value) {
    if (!dynaLogIsEnabled) return;

    // Avoid large stack temporaries; build combined line on heap
    std::string left = formatString("%s", format ? format : "");
    std::string right = value.toJson(); // assume returns std::string
    writeLogLine(left + " " + right);
}

inline void dynaLogLn(const char* format, ...) {
    if (!dynaLogIsEnabled) return;
    va_list args;
    va_start(args, format);
    std::string rendered = vformatString(format, args);
    va_end(args);
    writeLogLine(rendered);
}

// Standard logging with newline.
template<typename... Args>
inline void dynaLog(const char* format, Args&&... args) {
    if (!dynaLogIsEnabled) return;
    std::string rendered = formatString(format, std::forward<Args>(args)...);
    writeLogLine(rendered);
}

// Standard logging without newline (keeps [LOG] prefix).
template<typename... Args>
inline void dynaLogNoBreak(const char* format, Args&&... args) {
    if (!dynaLogIsEnabled) return;
    std::string rendered = formatString(format, std::forward<Args>(args)...);
    writeLogNoBreak(rendered);
}

inline void dynaLogBreak() {
    if (!dynaLogIsEnabled) return;
#ifdef ARDUINO
    Serial.print('\n');
#else
    fputc('\n', stdout);
#endif
}

inline void dynaLogEnabled(bool enable)   { dynaLogIsEnabled = enable; }
inline void dynaLogOn()                   { dynaLogIsEnabled = true; }
inline void dynaLogOff()                  { dynaLogIsEnabled = false; }
inline void dynaLogIndent()               { ++dynaLogIndentLevel; }
inline void dynaLogDedent()               { if (dynaLogIndentLevel > 0) --dynaLogIndentLevel; }

} // namespace Irrelon