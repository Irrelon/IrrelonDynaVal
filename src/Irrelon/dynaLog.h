// dyna_log.h
#pragma once

// -------- Configuration --------
// Define FMT_HEADER_ONLY before including <fmt/...> to avoid linking a .cpp.
#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "DynaVal.h"

// Change these if you want different defaults.
#ifndef IRRELON_LOG_PREFIX
#define IRRELON_LOG_PREFIX "[LOG] "
#endif

#ifndef IRRELON_LOG_INDENT_CHAR
#define IRRELON_LOG_INDENT_CHAR '\t'
#endif

#include <string>
#include <string_view>
#include <utility>
   // core formatting

namespace Irrelon {
	// ---- State ----
	inline bool dynaLogIsEnabled = true;
	inline int dynaLogIndentLevel = 0;

	// ---- Utilities ----
	inline std::string dynaLogIndentString(char indentChar = IRRELON_LOG_INDENT_CHAR) {
		if (dynaLogIndentLevel <= 0) return {};
		return std::string(static_cast<size_t>(dynaLogIndentLevel), indentChar);
	}

	// Low-level sink: write bytes to console/serial *without* interpreting format tokens.
	inline void dynaLogWriteSink(std::string_view s) {
#ifdef ARDUINO
		// On Arduino cores, Serial may not be initialized automatically.
		// Ensure Serial.begin(...) elsewhere before logging.
		for (char c: s) { Serial.print(c); }
#else
		(void) fwrite(s.data(), 1, s.size(), stdout);
#endif
	}

	// ---- Core output function (everything funnels here) ----
	// Writes a single line (with or without trailing newline), including indent and prefix.
	inline void dynaLogOutputRaw(std::string_view msg, bool add_newline = true, bool annotate = true, bool indent = true) {
		// Build the final line once to minimize I/O calls.
		std::string out;
		out.reserve(dynaLogIndentLevel + sizeof(IRRELON_LOG_PREFIX) + msg.size() + 1);

		if (indent) {
			fmt::format_to(std::back_inserter(out), "{}", dynaLogIndentString());
		}

		if (annotate) {
			fmt::format_to(std::back_inserter(out), "{}", IRRELON_LOG_PREFIX);
		}

		// indent + prefix + msg + optional '\n'
		fmt::format_to(std::back_inserter(out), "{}", msg);

		if (add_newline) out.push_back('\n');

		dynaLogWriteSink(out);
	}

	namespace detail {
		inline void append_piece(std::string& out, const char* s)        { if (s) out.append(s); }
		inline void append_piece(std::string& out, const std::string& s) { out.append(s); }
		inline void append_piece(std::string& out, char c)               { out.push_back(c); }
		inline void append_piece(std::string& out, bool b)               { out.append(b ? "true" : "false"); }
		inline void append_piece(std::string& out, int v)                { out.append(std::to_string(v)); }
		inline void append_piece(std::string& out, unsigned v)           { out.append(std::to_string(v)); }
		inline void append_piece(std::string& out, long v)               { out.append(std::to_string(v)); }
		inline void append_piece(std::string& out, unsigned long v)      { out.append(std::to_string(v)); }
		inline void append_piece(std::string& out, long long v)          { out.append(std::to_string(v)); }
		inline void append_piece(std::string& out, unsigned long long v) { out.append(std::to_string(v)); }
		inline void append_piece(std::string& out, float v)              { out.append(std::to_string(v)); }
		inline void append_piece(std::string& out, double v)             { out.append(std::to_string(v)); }
		inline void append_piece(std::string& out, const DynaVal& v)     { out.append(v.toJson()); }

		template<typename T>
		inline void append_piece(std::string& out, const T& t) { out.append(std::to_string(t)); } // fallback if needed
	}

	template<typename... Args>
	inline void dynaLogJoin(Args&&... args) {
		if (!dynaLogIsEnabled) return;
		std::string msg;
		msg.reserve(256);
		(detail::append_piece(msg, std::forward<Args>(args)), ...);
		dynaLogOutputRaw(msg);
	}

	inline void dynaLogLn(fmt::format_string<std::string_view> fmtstr, const Irrelon::DynaVal& v) {
		if (!dynaLogIsEnabled) return;
		const std::string s = v.toJson();
		dynaLogOutputRaw(fmt::format(fmtstr, std::string_view{s}), true);
	}

	inline void dynaLogLn(fmt::format_string<std::string_view> fmtstr, Irrelon::DynaVal& v) {
		if (!dynaLogIsEnabled) return;
		const std::string s = v.toJson();
		dynaLogOutputRaw(fmt::format(fmtstr, std::string_view{s}), true);
	}

	// optional no-newline
	inline void dynaLog(fmt::format_string<std::string_view> fmtstr, const Irrelon::DynaVal& v) {
		if (!dynaLogIsEnabled) return;
		const std::string s = v.toJson();
		dynaLogOutputRaw(fmt::format(fmtstr, std::string_view{s}), false, false, false);
	}

	inline void dynaLog(fmt::format_string<std::string_view> fmtstr, Irrelon::DynaVal& v) {
		if (!dynaLogIsEnabled) return;
		const std::string s = v.toJson();
		dynaLogOutputRaw(fmt::format(fmtstr, std::string_view{s}), false);
	}

	// ---- Public API: println-style ----
	template<class... Args>
	inline void dynaLogLn(fmt::format_string<Args...> fmtstr, Args &&... args) {
		if (!dynaLogIsEnabled) return;
		// Type-safe formatting; compile-time checked against fmtstr.
		const std::string line = fmt::format(fmtstr, std::forward<Args>(args)...);
		dynaLogOutputRaw(line, /*add_newline*/ true);
	}

	// Raw message (no formatting interpretation)
	inline void dynaLogLn(std::string_view msg) {
		if (!dynaLogIsEnabled) return;
		dynaLogOutputRaw(msg, /*add_newline*/ true);
	}

	inline void dynaLogLn(const char *cstr) {
		if (!dynaLogIsEnabled) return;
		dynaLogOutputRaw(cstr ? std::string_view{cstr} : std::string_view{}, /*add_newline*/ true);
	}

	// ---- Public API: print-without-newline ----
	template<class... Args>
	inline void dynaLog(fmt::format_string<Args...> fmtstr, Args &&... args) {
		if (!dynaLogIsEnabled) return;
		const std::string chunk = fmt::format(fmtstr, std::forward<Args>(args)...);
		dynaLogOutputRaw(chunk, /*add_newline*/ false);
	}

	// Raw (no formatting)
	inline void dynaLog(std::string_view msg) {
		if (!dynaLogIsEnabled) return;
		dynaLogOutputRaw(msg, /*add_newline*/ false);
	}

	inline void dynaLog(const char *cstr) {
		if (!dynaLogIsEnabled) return;
		dynaLogOutputRaw(cstr ? std::string_view{cstr} : std::string_view{}, /*add_newline*/ false);
	}

	inline void dynaLogPrintIndent() {
		if (!dynaLogIsEnabled) return;
		dynaLogOutputRaw("", false, false, true);
	}

	inline void dynaLogPrintAnnotate() {
		if (!dynaLogIsEnabled) return;
		dynaLogOutputRaw("", false, true, false);
	}

	template<class... Args>
	inline void dynaLogForce(fmt::format_string<Args...> fmtstr, Args &&... args) {
		// Type-safe formatting; compile-time checked against fmtstr.
		const std::string line = fmt::format(fmtstr, std::forward<Args>(args)...);
		dynaLogOutputRaw(line, /*add_newline*/ true);
	}

	// ---- Controls ----
	inline void dynaLogEnabled(bool enable) { dynaLogIsEnabled = enable; }
	inline void dynaLogOn() { dynaLogIsEnabled = true; }
	inline void dynaLogOff() { dynaLogIsEnabled = false; }
	inline void dynaLogIndent() { ++dynaLogIndentLevel; }
	inline void dynaLogDedent() { if (dynaLogIndentLevel > 0) --dynaLogIndentLevel; }
} // namespace Irrelon
