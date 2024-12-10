/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP
#include <Core/Result.hpp>
#include <Core/ClassTraits.hpp>
#include <Sys/String.hpp>
#include <cstdint>
#include <vector>

BEGIN_NAMESPACE(n19::argp);

// A value that is extracted from
// the input stream, in raw text form.
struct Value {
  auto to_i64()  const -> Result<int64_t>;
  auto to_cdvs() const -> Result<std::vector<Value>>;
  auto to_bool() const -> Result<bool>;
  auto to_f64()  const -> Result<double>;

  sys::String value_;
  explicit Value(const sys::String& str)
    : value_(str) {}
};

// This class represents a value
// that we expect to receive from the
// command line. Stores the long form,
// short form, and the value itself.
struct Parameter {
  bool required_         = false;
  Maybe<Value> default_  = std::nullopt;
  Maybe<Value> val_      = std::nullopt;

  // The longform (i.e. --param),
  // the shortform (i.e. -p),
  // and the description of what this is.
  sys::StringView lf_;
  sys::StringView sf_;
  std::string_view desc_;

  // A helpful factory for this type,
  // so we can get default arguments.
  static auto create(
    const sys::StringView& lf,
    const sys::StringView& sf,
    const std::string_view& desc,
    bool required        = false,
    Maybe<Value>&& deflt = std::nullopt
  ) -> Parameter;
  Parameter() = default;
};

// The argp::Parser class is used to
// parse arguments passed from the command line,
// and subsequently retrieve them as argp::Values.
class Parser {
N19_MAKE_NONCOPYABLE(Parser);
N19_MAKE_NONMOVABLE(Parser);
public:
  auto add_param(Parameter&& param) -> Parser&;
  auto print() const                -> void;
  auto debug_print() const          -> void;

  auto parse(const std::vector<sys::StringView>& chunks) -> Result<None>;
  auto get_arg(const sys::StringView& str ) const        -> Result<Value>;

  ~Parser() = default;
  Parser()  = default;
private:
  auto _is_valid_argument(const sys::String& str) const -> bool;
  auto _check_required_params() const -> bool;

  static auto _already_passed(
    const size_t index,
    const std::vector<sys::StringView>& strings
  ) -> bool;

  static auto _print_chunk_error(
    const std::string& msg,
    const size_t at,
    const std::vector<sys::StringView>& strings
  ) -> void;

  std::vector<Parameter> params_;
};

END_NAMESPACE(n19::argp);
#endif //ARGPARSE_HPP
