#include <string>
#include <variant>

#include "HugoTypes.h"

// Struct genérico de error
struct Error {
  virtual HUGO::String description() { return ""; }
};

// Clase error o tipo usando variants
template <typename T> 
class ErrorOrType {
  bool error;
  std::variant<Error, T> info;

public:
  explicit ErrorOrType(Error err) : error(true), info(err) {}
  explicit ErrorOrType(T value) : error(false), info(value) {}

  inline bool isError() { return isError; }
  inline Error getError() { return std::get<Error>(info); }
  inline Error getValue() { return std::get<T>(info); }
};

// Errores pre-implementados

struct StringError : Error {
  HUGO::String errorMessage;
  HUGO::String description() override { return errorMessage; }
};
