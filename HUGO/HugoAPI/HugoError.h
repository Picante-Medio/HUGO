#pragma once

#include <variant>
#include <assert.h>

#include "HugoTypes.h"

namespace HUGO {

// Generic struct error
struct Error {
	virtual ~Error() = default;
	virtual std::string description() = 0;
};

// Default error
struct StringError : Error {
	std::string errorMessage;
	StringError(const std::string& error) : errorMessage(error) {};
	StringError() {};
	std::string description() override { return errorMessage; }
};

// Error class using variants
template <typename T, typename ErrorT = StringError> 
class ErrorOrType {
	std::variant<ErrorT, T> info;
protected:
	bool error;

public:
	explicit ErrorOrType(ErrorT err) : info(err), error(true) {}
	explicit ErrorOrType(T value) : info(value), error(false) {}

	inline bool isError() { return error; }
	inline ErrorT getError() { return std::get<ErrorT>(info); }
	virtual inline T getValue() { 
		assert(!error && "Called getValue() on an error result");
	return std::get<T>(info); 
	}
};

// Void errors
class ErrorOrVoid : public ErrorOrType<std::monostate> {
public:
	inline ErrorOrVoid() : ErrorOrType<std::monostate> (std::monostate()) {}
	inline ErrorOrVoid(StringError err) : ErrorOrType<std::monostate> (err) {}
};

} // namespace HUGO