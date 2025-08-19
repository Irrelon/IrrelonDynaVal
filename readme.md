# Irrelon DynaVal
A dynamic value container with type safety and PSRAM-backed containers for ESP32.

## Info
This dynamic container uses PSRAM on ESP32. It has not been tested on other platforms or modules without PSRAM. The code in `allocator.h` handles allocating memory in PSRAM. Code paths to handle non-PSRAM modules exists in `allocator.h` but have not been tested.

## Usage
```c++
#include <Irrelon/DynaVal.h>
```

## Assigning Data
```c++
// Define a DynaVal
DynaVal myObj = Irrelon::DynaVal.becomeObject();
myObj["someKey1"] = 13;
myObj["someKey2"] = true;
myObj["someKey3"] = "Hello";
myObj["someKey4"]["someOtherKey"] = true;
```

## Extracting Data
```c++
const std::string json = myObj.toJson();

// Get a uint
const uint32_t val = myObj["someKey1"].toUInt();

// Get an int
const int val = myObj["someKey1"].toInt();

// Get a bool
const bool val = myObj["someKey2"].toBool();
const bool val = myObj["someKey4"]["someOtherKey"].toBool();

// Get a string
const int val = myObj["someKey3"].toString();
```