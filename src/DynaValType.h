#pragma once
#include <string>

enum class DynaValType {
	Error,
	Undefined,
	Null,
	Any,
	Int,
	Uint,
	Float,
	Bool,
	String,
	Array,
	Object,
};

inline std::string dynaValTypeToString(const DynaValType type) {
	switch (type) {
		case DynaValType::Error: return "error";
		case DynaValType::Undefined: return "undefined";
		case DynaValType::Null: return "null";
		case DynaValType::Any: return "any";
		case DynaValType::Int: return "int";
		case DynaValType::Uint: return "u_int";
		case DynaValType::Float: return "float";
		case DynaValType::Bool: return "boolean";
		case DynaValType::String: return "string";
		case DynaValType::Array: return "array";
		case DynaValType::Object: return "object";
		default: return "unknown";
	}
}