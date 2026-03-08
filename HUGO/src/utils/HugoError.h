#pragma once

#include <variant>
#include <assert.h>

#include "HugoTypes.h"

// Generic struct error
struct Error {
  virtual HUGO::String description() = 0;
};

// Default error
struct StringError : Error {
  HUGO::String errorMessage;
  StringError(HUGO::String error) : errorMessage(error) {};
  StringError() {};
  HUGO::String description() override { return errorMessage; }
};

// Error class using variants
template <typename T, typename ErrorT = StringError> 
class ErrorOrType {
  std::variant<ErrorT, T> info;
protected:
  bool error;

public:
  explicit ErrorOrType(ErrorT err) : error(true), info(err) {}
  explicit ErrorOrType(T value) : error(false), info(value) {}

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
