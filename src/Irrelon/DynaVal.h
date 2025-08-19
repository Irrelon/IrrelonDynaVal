#pragma once

#include <iomanip>  // for std::boolalpha
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <Irrelon/PSRAMAllocator.h>
#include "DynaError.h"
#include "DynaValType.h"

namespace Irrelon {
	struct DynaVal;

	// Use PSRAM-backed containers
	using DynaValArray = std::vector<DynaVal, PSRAMAllocator<DynaVal>>;
	using DynaValObject = std::unordered_map<std::string, DynaVal, std::hash<std::string>, std::equal_to<std::string>,
		PSRAMAllocator<std::pair<const std::string, DynaVal>>>;

	struct DynaVal {
		bool frozen = false;
		bool solid = false;
		DynaValType type = DynaValType::Null;
		float numberFloat = 0.0f;
		double numberDouble = 0.0f;
		int numberInteger = 0;
		u_int numberUnsignedInteger = 0;
		bool boolean = false;
		std::string string;
		std::shared_ptr<DynaValArray> array;
		std::shared_ptr<DynaValObject> object;
		std::shared_ptr<DynaError> errorData;

		DynaVal()
		: frozen(false),
		  solid(false),
		  type(DynaValType::Null),
		  numberFloat(0),
		  numberInteger(0),
		  numberUnsignedInteger(0),
		  boolean(false),
		  string(),
		  array(nullptr),
		  object(nullptr),
		  errorData(nullptr) {}

		DynaVal(const DynaVal &other)
		: frozen(other.frozen),
		  solid(other.solid),
		  type(other.type),
		  numberFloat(other.numberFloat),
		  numberInteger(other.numberInteger),
		  numberUnsignedInteger(other.numberUnsignedInteger),
		  boolean(other.boolean),
		  string(other.string),
		  array(other.array),
		  object(other.object),
		  errorData(other.errorData) {}

		DynaVal &operator= (const DynaVal &other) {
			if (this != &other) {
				type = other.type;
				numberFloat = other.numberFloat;
				numberInteger = other.numberInteger;
				numberUnsignedInteger = other.numberUnsignedInteger;
				boolean = other.boolean;
				string = other.string;
				array = other.array;
				object = other.object;
				errorData = other.errorData;
				frozen = other.frozen;
				solid = other.solid;
			}
			return *this;
		}

		// Support array initialization with a list of Values
		DynaVal (std::initializer_list<DynaVal> initList) : type(DynaValType::Array),
			array(std::make_shared<DynaValArray>(initList)) {}

		// Catch initializer lists of initializer lists (i.e. nested arrays)
		DynaVal (const std::initializer_list<std::initializer_list<DynaVal>> nestedList) : type(DynaValType::Array),
			array(std::make_shared<DynaValArray>()) {
			for (const auto &inner : nestedList) {
				array->emplace_back(DynaVal(inner)); // Convert the inner list into DynaVal
			}
		}

		template <typename T>
		DynaVal (const std::vector<T> &vec) : type(DynaValType::Array), array(std::make_shared<DynaValArray>()) {
			for (const auto &item : vec) {
				array->emplace_back(DynaVal(item));
			}
		}

		// Catch all integral types except the u_ints: int, size_t, int8_t etc.
		template <
		typename T,
		typename = std::enable_if_t<
				std::is_integral_v<T> &&
				!std::is_same_v<T, uint8_t> &&
				!std::is_same_v<T, uint16_t> &&
				!std::is_same_v<T, uint32_t>
			>
		>
		DynaVal (T n) : type(DynaValType::Float), numberFloat(static_cast<float>(n)) {}

		// Float
		DynaVal (const float num) : type(DynaValType::Float), numberFloat(num) {}

		// Double
		DynaVal (const double num) : type(DynaValType::Double), numberDouble(num) {}

		// Int
		DynaVal (const int num) : type(DynaValType::Int), numberInteger(num) {}

		// Uint
		DynaVal (const uint8_t num) : type(DynaValType::Uint), numberUnsignedInteger(num) {}
		DynaVal (const uint16_t num) : type(DynaValType::Uint), numberUnsignedInteger(num) {}
		DynaVal (const uint32_t num) : type(DynaValType::Uint), numberUnsignedInteger(num) {}
		DynaVal (const uint64_t num) : type(DynaValType::Uint), numberUnsignedInteger(num) {}

		// Bool
		DynaVal (const bool b) : type(DynaValType::Bool), boolean(b) {}

		// String
		DynaVal (std::string s) : type(DynaValType::String), string(std::move(s)) {}

		DynaVal (const char *s) : type(DynaValType::String), string(s) {}

		// ValueArray / ValueObject
		DynaVal (DynaValArray arr) : type(DynaValType::Array), array(std::make_shared<DynaValArray>(std::move(arr))) {}

		DynaVal (DynaValObject obj) : type(DynaValType::Object), object(std::make_shared<DynaValObject>(std::move(obj))) {}

		// Helper accessors
		[[nodiscard]] const DynaError &toError () const { return *errorData; }

		[[nodiscard]] float toFloat (const bool looseType = false) const {
			if (looseType) {
				if (type == DynaValType::Float) {
					return numberFloat;
				}

				if (type == DynaValType::Int) {
					return static_cast<float>(numberInteger);
				}

				if (type == DynaValType::Uint) {
					return static_cast<float>(numberUnsignedInteger);
				}

				if (type == DynaValType::Bool) {
					return boolean ? 1 : 0;
				}
			}

			return numberFloat;
		}

		[[nodiscard]] int toInt (const bool looseType = false) const {
			if (looseType) {
				if (type == DynaValType::Int) {
					return numberInteger;
				}

				if (type == DynaValType::Uint) {
					return static_cast<int>(numberUnsignedInteger);
				}

				if (type == DynaValType::Float) {
					return static_cast<int>(numberFloat);
				}

				if (type == DynaValType::Bool) {
					return boolean ? 1 : 0;
				}
			}

			return numberInteger;
		}

		[[nodiscard]] uint toUInt (const bool looseType = false) const {
			if (looseType) {
				if (type == DynaValType::Uint) {
					return numberUnsignedInteger;
				}

				if (type == DynaValType::Int) {
					return static_cast<u_int>(numberInteger);
				}

				if (type == DynaValType::Float) {
					return static_cast<u_int>(numberFloat);
				}

				if (type == DynaValType::Bool) {
					return boolean ? 1 : 0;
				}
			}

			return numberUnsignedInteger;
		}

		[[nodiscard]] bool toBool (const bool looseType = false) const {
			if (looseType) {
				return !isFalsy();
			}

			return boolean;
		}

		[[nodiscard]] std::string toString (const bool interpretArrayData = false) const {
			switch (type) {
				case DynaValType::String:
					return string;
				case DynaValType::Int:
					return std::to_string(numberInteger);
				case DynaValType::Uint:
					return std::to_string(numberUnsignedInteger);
				case DynaValType::Float:
					return std::to_string(numberFloat);
				case DynaValType::Bool:
					return boolean
						? "true"
						: "false";
				case DynaValType::Error:
					return errorData->toString();
				case DynaValType::Null:
					return "null";
				case DynaValType::Array:
					if (interpretArrayData) {
						return arrayToString();
					}

					return "[Array]";
				case DynaValType::Object:
					return "[Object]";
				default:
					return "[Unknown]";
			}
		}

		[[nodiscard]] const DynaValArray &toArray () const {
			if (!isArray()) {
				throw std::runtime_error("Tried to access non-array DynaVal as array");
			}

			return *array;
		}

		[[nodiscard]] const DynaValObject &toObject () const {
			if (!isObject()) {
				throw std::runtime_error("Tried to access non-object DynaVal as object");
			}

			return *object;
		}

		bool isFalsy () const {
			switch (type) {
				case DynaValType::Bool:
					return !boolean;
				case DynaValType::Null:
				case DynaValType::Undefined:
					return true;
				case DynaValType::Int:
					return numberInteger == 0;
				case DynaValType::Uint:
					return numberUnsignedInteger == 0;
				case DynaValType::Float:
					return numberFloat == 0.0f;
				case DynaValType::String:
					return string.empty();
				case DynaValType::Array:
					return !array || array->empty();
				case DynaValType::Object:
					return !object || object->empty();
				case DynaValType::Error:
					return false; // Errors are not falsy
				default:
					return false;
			}
		}

		// Type checkers
		[[nodiscard]] bool isError () const { return type == DynaValType::Error; }

		[[nodiscard]] bool isUndefined () const { return type == DynaValType::Undefined; }

		[[nodiscard]] bool isNull () const { return type == DynaValType::Null; }

		[[nodiscard]] bool isFloat () const { return type == DynaValType::Float; }

		[[nodiscard]] bool isInt (const bool looseType = false) const {
			if (looseType) {
				return type == DynaValType::Int || type == DynaValType::Uint;
			}

			return type == DynaValType::Int;
		}
		[[nodiscard]] bool isUInt (const bool looseType = false) const {
			if (looseType) {
				return type == DynaValType::Uint || type == DynaValType::Int;
			}

			return type == DynaValType::Uint;
		}

		[[nodiscard]] bool isBool () const { return type == DynaValType::Bool; }

		[[nodiscard]] bool isString () const { return type == DynaValType::String; }

		[[nodiscard]] bool isArray () const { return type == DynaValType::Array; }

		[[nodiscard]] bool isObject () const { return type == DynaValType::Object; }

		void freeze () { frozen = true; }

		void unfreeze () { frozen = false; }

		[[nodiscard]] bool isFrozen () const { return frozen; }

		void ensureMutable () const {
			if (frozen) throw std::runtime_error("Attempted to modify a frozen DynaVal");
		}

		[[nodiscard]] std::string getType () const {
			return dynaValTypeToString(type);
		}

		DynaVal &fromBytesAsArray(const uint8_t* data, const size_t length) {
			becomeArray();
			for (size_t i = 0; i < length; ++i) {
				const auto val = DynaVal(data[i]);
				if (val.getType() != "u_int") {
					throw std::runtime_error("fromBytesAsArray(): all elements must be unsigned integers");
				}
				push(val);
			}
			return *this;
		}

		/**
		 * Copies the contents of a DynaVal array into a caller-provided byte buffer.
		 * Only integer values in the range [0, 255] are allowed.
		 *
		 * @param out     A pointer to a buffer where the byte data will be written.
		 * @param maxLen  The maximum number of bytes to write to the buffer.
		 * @return        The number of bytes successfully written, or -1 on error.
		 */
		int arrayToBytes(uint8_t* out, const size_t maxLen = 2048) const {
			if (!out || !isArray()) return -1;

			const size_t count = std::min(size(), maxLen);

			for (size_t i = 0; i < count; ++i) {
				const auto& item = (*this)[i];
				if (!item.isInt(true)) return -1;

				const uint8_t n = item.toUInt(true);
				if (n < 0 || n > 255) return -1;

				out[i] = n;
			}

			return static_cast<int>(count);
		}

		[[nodiscard]] std::string toHexString() const {
			std::ostringstream oss;

			if (!isArray()) return "[not an array]";
			const auto& arr = toArray();

			for (size_t i = 0; i < arr.size(); ++i) {
				if (!arr[i].isUInt(true)) return "[non-uint in array]";
				oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(arr[i].toUInt(true));
				if (i < arr.size() - 1) oss << ":";
			}

			return oss.str();
		}

		[[nodiscard]] std::string arrayToString() const {
			if (!isArray()) {
				throw std::runtime_error("arrayToString() called on non-array DynaVal");
			}

			const auto& arr = toArray();
			std::string result;
			result.reserve(arr.size()); // reserve memory up front

			for (const auto& item : arr) {
				if (!item.isUInt()) {
					throw std::runtime_error("arrayToString(): all elements must be unsigned integers");
				}

				const uint8_t val = item.toUInt(true);
				if (val < 0 || val > 255) {
					throw std::runtime_error("arrayToString(): element out of byte range (0–255)");
				}

				//Serial.printf("ATS: Val of %d is %c ", val, static_cast<char>(val));

				result.push_back(static_cast<char>(val));
			}

			return result;
		}

		DynaVal &becomeError () {
			if (type != DynaValType::Error) {
				type = DynaValType::Error;
				errorData = std::make_shared<DynaError>();
			}
			return *this;
		}

		DynaVal &becomeObject () {
			if (type != DynaValType::Object) {
				type = DynaValType::Object;
				object = std::make_shared<DynaValObject>();
			}
			return *this;
		}

		DynaVal &becomeArray () {
			if (type != DynaValType::Array) {
				type = DynaValType::Array;
				array = std::make_shared<DynaValArray>();
				array->clear();
			}
			return *this;
		}

		DynaVal &becomeString () {
			if (type != DynaValType::String) {
				type = DynaValType::String;
				string.clear();
			}
			return *this;
		}

		DynaVal &becomeFloat () {
			if (type != DynaValType::Float) {
				type = DynaValType::Float;
				numberFloat = 0.0f;
			}
			return *this;
		}

		DynaVal &becomeInt () {
			if (type != DynaValType::Int) {
				type = DynaValType::Int;
				numberFloat = 0;
			}
			return *this;
		}

		DynaVal &becomeBool () {
			if (type != DynaValType::Bool) {
				type = DynaValType::Bool;
				boolean = false;
			}
			return *this;
		}

		DynaVal &becomeNull () {
			type = DynaValType::Null;
			return *this;
		}

		DynaVal &becomeUndefined () {
			type = DynaValType::Undefined;
			return *this;
		}

		std::string toJson () const {
			std::ostringstream out;
			_toJson(out);
			return out.str();
		}

		size_t size () const {
			if (type == DynaValType::Array) {
				return array
					? array->size()
					: 0;
			}
			if (type == DynaValType::Object) {
				return object
					? object->size()
					: 0;
			}
			return 0;
		}

		void reset () {
			clear();
			type = DynaValType::Null;
		}

		void clear () {
			switch (type) {
				case DynaValType::String:
					string.clear();
					break;
				case DynaValType::Array:
					array.reset();
					break;
				case DynaValType::Object:
					object.reset();
					break;
				case DynaValType::Error:
					errorData.reset();
					break;
				default:
					break;
			}
		}

		void remove (const size_t index) const {
			if (type != DynaValType::Array || !array) return;
			if (index >= array->size()) return;
			array->erase(array->begin() + index);
		}

		[[nodiscard]] bool containsKey (const std::string &key) const {
			if (type != DynaValType::Object || !object) return false;
			return object->find(key) != object->end();
		}

		DynaVal(std::shared_ptr<DynaValArray> arr) {
			type = DynaValType::Array;
			array = std::move(arr);
		}

		DynaVal (const DynaError &err) {
			type = DynaValType::Error;
			errorData = std::make_shared<DynaError>(err);
		}

		DynaVal (DynaError &&err) {
			type = DynaValType::Error;
			errorData = std::make_shared<DynaError>(std::move(err));
		}

		DynaVal &set (float val) {
			type = DynaValType::Float;
			numberFloat = val;
			return *this;
		}

		DynaVal &set (int val) {
			type = DynaValType::Int;
			numberInteger = val;
			return *this;
		}

		DynaVal &set (uint8_t val) {
			type = DynaValType::Uint;
			numberUnsignedInteger = static_cast<uint>(val);
			return *this;
		}

		DynaVal &set (uint16_t val) {
			type = DynaValType::Uint;
			numberUnsignedInteger = static_cast<uint>(val);
			return *this;
		}

		DynaVal &set (uint32_t val) {
			type = DynaValType::Uint;
			numberUnsignedInteger = val;
			return *this;
		}

		DynaVal &set (bool val) {
			type = DynaValType::Bool;
			boolean = val;
			return *this;
		}

		DynaVal &set (const std::string &val) {
			reset();
			type = DynaValType::String;
			string = val;
			return *this;
		}

		DynaVal &set (const char *val) {
			reset();
			type = DynaValType::String;
			string = val;
			return *this;
		}

		DynaVal &set (const DynaValArray &arr) {
			reset();
			type = DynaValType::Array;
			array = std::make_shared<DynaValArray>(arr);
			return *this;
		}

		DynaVal &set (DynaValArray &&arr) {
			reset();
			type = DynaValType::Array;
			array = std::make_shared<DynaValArray>(std::move(arr));
			return *this;
		}

		DynaVal &set (const DynaValObject &obj) {
			reset();
			type = DynaValType::Object;
			object = std::make_shared<DynaValObject>(obj);
			return *this;
		}

		DynaVal &set (const DynaError &err) {
			reset();
			type = DynaValType::Error;
			errorData = std::make_shared<DynaError>(err); // copies the error
			return *this;
		}

		DynaVal &set (DynaError &&err) {
			reset();
			type = DynaValType::Error;
			errorData = std::make_shared<DynaError>(std::move(err));
			return *this;
		}

		DynaVal &set (const DynaVal &other) {
			reset();
			type = other.type;
			numberFloat = other.numberFloat;
			numberInteger = other.numberInteger;
			numberUnsignedInteger = other.numberUnsignedInteger;
			boolean = other.boolean;
			string = other.string;
			array = other.array;
			object = other.object;
			errorData = other.errorData;
			return *this;
		}

		explicit operator bool () const {
			return type != DynaValType::Null && type != DynaValType::Undefined;
		}

		bool operator!= (const DynaVal &other) const {
			return !(*this == other);
		}

		bool operator== (const std::string &other) const {
			return type == DynaValType::String && string == other;
		}

		bool operator== (const char *other) const {
			return type == DynaValType::String && string == std::string(other);
		}

		bool operator== (const int other) const {
			if (type == DynaValType::Int) return numberInteger == other;
			if (type == DynaValType::Uint) return numberUnsignedInteger == static_cast<u_int>(other);
			if (type == DynaValType::Float) return numberFloat == static_cast<float>(other);
			return false;
		}

		bool operator== (const u_int other) const {
			if (type == DynaValType::Uint) return numberUnsignedInteger == other;
			if (type == DynaValType::Int) return numberInteger == static_cast<int>(other);
			if (type == DynaValType::Float) return numberFloat == static_cast<float>(other);
			return false;
		}

		bool operator== (const float other) const {
			if (type == DynaValType::Float) return numberFloat == other;
			if (type == DynaValType::Int) return static_cast<float>(numberInteger) == other;
			return false;
		}

		bool operator== (const bool other) const {
			return type == DynaValType::Bool && boolean == other;
		}

		bool operator== (const DynaVal &other) const {
			switch (type) {
				case DynaValType::Null:
					return other.type == DynaValType::Null;
				case DynaValType::Undefined:
					return other.type == DynaValType::Undefined;
				case DynaValType::Int:
					if (other.type == DynaValType::Int) return numberInteger == other.numberInteger;
					if (other.type == DynaValType::Uint) return numberInteger == static_cast<int>(other.numberUnsignedInteger);
				case DynaValType::Uint:
					if (other.type == DynaValType::Uint) return numberUnsignedInteger == other.numberUnsignedInteger;
					if (other.type == DynaValType::Int) return numberUnsignedInteger == static_cast<u_int>(other.numberInteger);
				case DynaValType::Float:
					return numberFloat == other.numberFloat;
				case DynaValType::Bool:
					return boolean == other.boolean;
				case DynaValType::String:
					return string == other.string;
				case DynaValType::Array:
					return array == other.array; // pointer equality (or implement deep comparison)
				case DynaValType::Object:
					return object == other.object; // same here

				default:
					return false;
			}
		}

		// Non-const: allows modifying or creating array elements
		[[nodiscard]] DynaVal &operator[] (const size_t index) {
			if (type != DynaValType::Array) {
				type = DynaValType::Array;
				array = std::make_shared<DynaValArray>();
			}

			if (index >= array->size()) {
				// Expand the array with new DynaVals if needed
				array->resize(index + 1);
			}

			return (*array)[index];
		}

		// Const: safe read-only access
		[[nodiscard]] const DynaVal &operator[] (const size_t index) const {
			static const DynaVal nullValue;

			if (type == DynaValType::Array && index < array->size()) {
				return (*array)[index];
			}

			return nullValue;
		}

		[[nodiscard]] DynaVal &operator[] (const int index) {
			if (index < 0) { return DynaVal().becomeNull(); }

			if (type != DynaValType::Array) {
				type = DynaValType::Array;
				array = std::make_shared<DynaValArray>();
			}

			if (index >= array->size()) {
				// Expand the array with nulls if needed
				array->resize(index + 1);
			}

			return (*array)[index];
		}

		[[nodiscard]] const DynaVal &operator[] (const int index) const {
			static const DynaVal nullValue;

			if (index < 0) { return nullValue; }

			if (type == DynaValType::Array && index < array->size()) {
				return (*array)[index];
			}

			return nullValue;
		}

		// Non-const version: allows mutation or creation of new keys
		[[nodiscard]] DynaVal &operator[] (const std::string &key) {
			if (type == DynaValType::Error) {
				throw std::runtime_error("Cannot use operator[] on DynaVal of type Error");
			}

			if (type != DynaValType::Object) {
				type = DynaValType::Object;
				object = std::make_shared<DynaValObject>();
			}

			return (*object)[key];
		}

		// Const version: safe lookup only
		[[nodiscard]] const DynaVal &operator[] (const std::string &key) const {
			static const DynaVal nullValue;

			if (type == DynaValType::Object) {
				auto it = object->find(key);
				if (it != object->end()) {
					return it->second;
				}
			}

			return nullValue;
		}

		DynaVal &push (const DynaVal &val) {
			if (type != DynaValType::Array) {
				becomeArray(); // ensure array initialized
			}
			array->push_back(val);

			return array->back();
		}

		DynaVal &push (const float n) { return push(DynaVal(n)); }

		DynaVal &push (const int n) { return push(DynaVal(n)); }

		DynaVal &push (const u_int n) { return push(DynaVal(n)); }
		DynaVal &push (const uint8_t n) { return push(DynaVal(n)); }

		DynaVal &push (const std::string &s) { return push(DynaVal(s)); }

		DynaVal &push (const char *s) { return push(DynaVal(s)); }

		DynaVal &push (const bool b) { return push(DynaVal(b)); }

		[[nodiscard]] DynaVal deepCopy () const {
			switch (type) {
				case DynaValType::Error:
					return DynaVal().becomeError();
				case DynaValType::Any:
				case DynaValType::Undefined:
					return DynaVal().becomeUndefined();
				case DynaValType::Null:
					return DynaVal().becomeNull();
				case DynaValType::Int:
					return DynaVal(numberInteger);
				case DynaValType::Uint:
					return DynaVal(numberUnsignedInteger);
				case DynaValType::Float:
					return DynaVal(numberFloat);
				case DynaValType::Double:
					return DynaVal(numberDouble);
				case DynaValType::Bool:
					return DynaVal(boolean);
				case DynaValType::String:
					return DynaVal(string);
				case DynaValType::Array: {
					DynaValArray newArray;
					if (array) {
						for (const auto &item : *array) {
							newArray.push_back(item.deepCopy());
						}
					}
					return {std::move(newArray)};
				}
				case DynaValType::Object: {
					DynaValObject newObject;
					if (object) {
						for (const auto &[k, v] : *object) {
							newObject[k] = v.deepCopy();
						}
					}
					return {std::move(newObject)};
				}
			}

			// Fallback shouldn't happen
			return {};
		}

		static DynaVal error (const DynaError err) {
			auto val = DynaVal();
			val.type = DynaValType::Error;
			val.errorData = std::make_shared<DynaError>(err); // copy into shared_ptr
			return val;
		}

		static DynaVal error (
			const std::string &message,
			const int statusCode = 0,
			const std::vector<std::string> &stack = {}
		) {
			DynaVal val;
			val.type = DynaValType::Error;
			val.errorData = std::make_shared<DynaError>();
			val.errorData->message = message;
			val.errorData->statusCode = statusCode;
			val.errorData->stack = std::make_shared<DynaVal>();
			val.errorData->stack->becomeArray().clear();

			for (const auto &frame : stack) {
				val.errorData->stack->push(DynaVal(frame));
			}
			return val;
		}

	private:
		void _toJson (std::ostringstream &out) const {
			switch (type) {
				case DynaValType::Error:
					out << errorData->toString();
					break;

				case DynaValType::Any:
				case DynaValType::Undefined:
					out << "undefined";
					break;
				case DynaValType::Null:
					out << "null";
					break;
				case DynaValType::Float:
					out << numberFloat;
					break;
				case DynaValType::Int:
					out << numberInteger;
					break;
				case DynaValType::Uint:
					out << numberUnsignedInteger;
					break;
				case DynaValType::Bool:
					out << std::boolalpha << boolean;
					break;
				case DynaValType::String:
					out << '"';
					for (const char stringChar : string) {
						switch (stringChar) {
							case '"':
								out << "\\\"";
								break;
							case '\\':
								out << "\\\\";
								break;
							case '\b':
								out << "\\b";
								break;
							case '\f':
								out << "\\f";
								break;
							case '\n':
								out << "\\n";
								break;
							case '\r':
								out << "\\r";
								break;
							case '\t':
								out << "\\t";
								break;
							default:
								if (static_cast<unsigned char>(stringChar) < 0x20) out << "\\u" << std::hex <<
									std::setw(4) << std::setfill('0') << static_cast<int>(stringChar) << std::dec;
								else out << stringChar;
						}
					}
					out << '"';
					break;
				case DynaValType::Array:
					out << '[';
					for (size_t i = 0; i < array->size(); ++i) {
						if (i > 0) out << ',';
						if ((*array)[i]) {
							(*array)[i]._toJson(out);
						} else {
							out << "nullptr";
							//throw std::runtime_error("WARNING: Null pointer value detected as an array entry!");
						}
					}
					out << "]";
					break;
				case DynaValType::Object:
					out << '{';
					bool first = true;
					for (const auto &[key, val] : *object) {
						if (!first) out << ',';
						first = false;
						out << '"' << key << "\":";
						val._toJson(out);
					}
					out << '}';
					break;
			}
		}
	};

	// inline DynaVal dynaValFromJson (const JsonVariant src) {
	// 	if (src.isNull()) {
	// 		return DynaVal().becomeNull(); // Null
	// 	}
	//
	// 	if (src.is<bool>()) {
	// 		return DynaVal(src.as<bool>());
	// 	}
	//
	// 	if (src.is<int>()) {
	// 		return DynaVal(src.as<int>());
	// 	}
	//
	// 	if (src.is<u_int>()) {
	// 		return DynaVal(src.as<u_int>());
	// 	}
	//
	// 	if (src.is<float>()) {
	// 		return DynaVal(src.as<float>());
	// 	}
	//
	// 	if (src.is<const char*>() || src.is<std::string>()) {
	// 		return DynaVal(std::string(src.as<const char*>()));
	// 	}
	//
	// 	if (src.is<JsonArray>()) {
	// 		DynaValArray arr;
	// 		for (const JsonVariant v : src.as<JsonArray>()) {
	// 			arr.push_back(dynaValFromJson(v));
	// 		}
	//
	// 		return DynaVal(std::move(arr));
	// 	}
	//
	// 	if (src.is<JsonObject>()) {
	// 		DynaValObject obj;
	// 		for (JsonPair kv : src.as<JsonObject>()) {
	// 			obj[kv.key().c_str()] = dynaValFromJson(kv.value());
	// 		}
	// 		return DynaVal(std::move(obj));
	// 	}
	//
	// 	// Unknown type
	// 	return DynaVal().becomeUndefined();
	// }

	inline DynaVal makeType (const DynaValType type, const DynaVal &subType = DynaVal()) {
		DynaVal node;
		node["kind"] = "DATA_TYPE";
		node["value"] = dynaValTypeToString(type);

		if (!subType.isNull()) {
			node["subType"] = subType;
		}

		return node;
	}

	inline DynaVal makeParam (
		const std::string &name,
		const DynaVal &typeNode,
		const DynaVal &defaultValue = DynaVal(),
		const bool hasDefault = false
	) {
		if (hasDefault) {
			DynaVal node;
			node["kind"] = "ASSIGNMENT_PATTERN";
			node["left"]["kind"] = "IDENTIFIER";
			node["left"]["value"] = name;
			node["left"]["type"] = typeNode;
			node["operator"] = "=";

			DynaVal defaultValueNode;
			defaultValueNode["kind"] = "LITERAL";
			defaultValueNode["value"] = defaultValue;
			node["right"] = defaultValueNode;

			return node;
		}

		DynaVal node;
		node["kind"] = "IDENTIFIER";
		node["value"] = name;
		node["type"] = typeNode;

		return node;
	}
}