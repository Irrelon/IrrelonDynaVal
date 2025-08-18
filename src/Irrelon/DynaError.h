#pragma once
#include <string>

namespace Irrelon {
	struct DynaVal;

	struct DynaError {
		std::string message;
		int statusCode;
		std::string key;
		std::shared_ptr<DynaVal> stack;

		DynaError() = default;

		DynaError(std::string msg, const int code = 500): message(std::move(msg)), statusCode(code) {}

		[[nodiscard]] std::string toString () const {
			std::ostringstream out;

			if (!message.empty()) {
				out << "Error(" << statusCode << "): " << message;
				// TODO: Figure out what stack trace shapes will be and what they will look like when stringified
				//if (stack) {
				//	out << std::endl << "Stack trace: " << stack->toJson() << std::endl;
				//}
			} else {
				out << "Error(<null>): Unknown error occurred";
			}

			return out.str();
		}
	};
}