#pragma once
#include <string>

namespace Irrelon {
	enum class DynaValType {
		// Special types
		Error,
		Undefined,
		Null,
		Any,
		// Numeric types
		Int,
		UInt,
		Float,
		Double,
		Long,
		// Other types
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
			case DynaValType::UInt: return "u_int";
			case DynaValType::Float: return "float";
			case DynaValType::Double: return "double";
			case DynaValType::Long: return "long";
			case DynaValType::Bool: return "boolean";
			case DynaValType::String: return "string";
			case DynaValType::Array: return "array";
			case DynaValType::Object: return "object";
			default: return "unknown";
		}
	}
}