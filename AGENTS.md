# Agent Guidelines for IrrelonDynaVal

## Build/Test Commands
- **Build**: `pio run -e local` (native debug build) or `pio run -e esp32_s3_r16n8` (ESP32 build)
- **Test**: `npm test` or `pio run -e local -t clean && pio test -e local --without-uploading -vvv`
- **Debug**: `npm run lldb` or `lldb .pio/build/local/program`
- **Single test**: No specific command - modify test/index.cpp and run `npm test`

## Code Style Guidelines
- **Language**: C++20 (gnu++2a standard)
- **Headers**: Use `#pragma once`, include system headers first, then project headers
- **Namespace**: All code in `Irrelon` namespace
- **Naming**: camelCase for variables/functions, PascalCase for types/classes
- **Types**: Use `[[nodiscard]]` for getters, prefer const methods where possible
- **Includes**: Use relative paths for project headers (e.g., `"DynaError.h"`)
- **Memory**: Use PSRAM-backed containers (`PSRAMAllocator`) and `std::shared_ptr`
- **Error handling**: Use exceptions with descriptive messages, DynaError for error values
- **Formatting**: Consistent spacing, braces on same line for classes/structs
- **Dependencies**: fmtlib/fmt@^8.1.1, irrelon/PSRAMAllocator@^1.0.1

## Testing
- Uses Unity test framework
- Test files in `test/` directory
- All tests in single `test/index.cpp` file currently
- Wrap test code in try-catch blocks with `TEST_FAIL_MESSAGE(e.what())`

*Target platform: ESP32 with PSRAM support*