/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef ARGPARSE_H
#define ARGPARSE_H
#include <Core/Result.h>
#include <Native/String.h>
#include <cstdint>
#include <functional>
#include <vector>

namespace n19::argp {
  class Parser;
  struct Value;
  struct Parameter;
}

// A value that is extracted from
// the input stream, in raw text form.
struct n19::argp::Value {
  auto to_i64()  const -> Result<int64_t>;
  auto to_cdvs() const -> Result<std::vector<Value>>;
  auto to_bool() const -> Result<bool>;
  auto to_f64()  const -> Result<double>;

  native::String value_;
  explicit Value(const native::String& str)
    : value_(str) {}
};

// This class represents a value
// that we expect to receive from the
// command line. Stores the long form,
// short form, and the value itself.
struct n19::argp::Parameter {
  bool required_         = false;
  Maybe<Value> default_  = std::nullopt;
  Maybe<Value> val_      = std::nullopt;

  // The longform (i.e. --param),
  // the shortform (i.e. -p),
  // and the description of what this is.
  native::StringView lf_;
  native::StringView sf_;
  std::string_view desc_;

  // A helpful factory for this type,
  // so we can get default arguments.
  static auto create(
    const native::StringView& lf,
    const native::StringView& sf,
    const std::string_view& desc,
    bool required        = false,
    Maybe<Value>&& deflt = std::nullopt
  ) -> Parameter;

  // Leave this constructor open
  // for convenience purposes. Parameter::create()
  // should be called most of the time though.
  Parameter() = default;
};

// The argp::Parser class is used to
// parse arguments passed from the command line,
// and subsequently retrieve them as argp::Values.
class n19::argp::Parser {
private:
  auto _is_valid_argument(
    const native::String& str
  ) const -> bool;

  static auto _already_passed(
    const size_t index,
    const std::vector<native::String>& strings
  ) -> bool;

  static auto _print_chunk_error(
    const std::string& msg,
    const size_t at,
    const std::vector<native::String>& strings
  ) -> void;

  auto _check_required_params()
    const -> bool;
public:
  Parser(const Parser&)             = delete;
  Parser& operator=(const Parser&)  = delete;

  auto get_arg(const native::StringView& str) const     -> Result<Value>;
  auto add_param(Parameter&& param)                     -> Parser&;
  auto parse(const std::vector<native::String>& chunks) -> Result<None>;
  auto print() const                                    -> void;
  auto debug_print() const                              -> void;

  ~Parser() = default;
  Parser()  = default;
private:
  std::vector<Parameter> params_;
};

#endif //ARGPARSE_H
