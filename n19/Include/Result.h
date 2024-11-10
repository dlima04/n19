#ifndef RESULT_H
#define RESULT_H
#include <expected>
#include <optional>
#include <cstdint>
#include <format>
#include <string>

namespace n19 {
  // A 16-bit error code.
  // Specifies the error's type, and is
  // usually found inside of a bithat::ErrorDescriptor.
  enum class ErrC : uint16_t {
    None,
    InvalidArg,
    FileIO,
    Platform,
    Internal,
    NotFound,
    BadToken
  };

  // A struct that represents a runtime error.
  // Contains an error code (n19::ErrC) as well
  // as a message (in the form of a std::string) for
  // some additional context on what went wrong.
  struct ErrorDescriptor;

  // A type that represents "nothing". Used in
  // situations where a function or method returns either
  // nothing, or an error (i.e. Result<None>).
  struct None;

  // The result and error types used by the compiler.
  // So that we can propagate errors as values without using
  // exceptions.
  template<typename T>
  using Result = std::expected<T, ErrorDescriptor>;
  using Error = std::unexpected<ErrorDescriptor>;

  // For consistency.
  template<typename T>
  using Maybe = std::optional<T>;

  template<typename T, typename ...Args>
  auto make_result(Args&&... args) -> Result<T>;

  template<typename ...Args>
  auto make_error(ErrC code, std::format_string<Args...>, Args... args) -> Error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct n19::ErrorDescriptor {
  std::string msg;
  ErrC code = ErrC::None;
};

struct n19::None {
  // Not zero sized to avoid aids cancer.
  unsigned char _ = 0;
  None() = default;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename ...Args>
auto n19::make_result(Args&&... args) -> Result<T> {
  return Result<T>{ T(std::forward<Args>(args)...) };
}

template<typename... Args>
auto n19::make_error(const ErrC code, std::format_string<Args...> fmt, Args... args)
-> Error {
  std::string formatted;
  ErrorDescriptor desc;
  try {
    formatted = std::vformat(fmt.get(), std::make_format_args(args...));
  } catch(const std::format_error& e) {
    formatted = std::string("!! std::vformat: ") + e.what();
  } catch(...) {
    formatted = "!! format error";
  }

  desc.code = code;
  desc.msg  = formatted;
  return Error{ desc };
}

#endif //RESULT_H
