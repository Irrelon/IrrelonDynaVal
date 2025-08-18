#pragma once
#include "../DynaVal.h"

inline DynaVal& dynaPathGet(DynaVal& obj, const std::string& path) {
	DynaVal* current = &obj;
	size_t dotPos;
	std::string segment = path;

	while ((dotPos = segment.find('.')) != std::string::npos) {
		std::string key = segment.substr(0, dotPos);
		segment = segment.substr(dotPos + 1);

		if (!current->containsKey(key)) {
			static DynaVal nullValue;  // safe dummy
			return nullValue;
		}

		current = &(*current)[key];
	}

	if (!current->containsKey(segment)) {
		static DynaVal nullValue;
		return nullValue;
	}

	return (*current)[segment];
}
