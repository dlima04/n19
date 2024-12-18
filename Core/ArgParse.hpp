/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP
#include <Sys/String.hpp>
#include <Core/Concepts.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
#include <Core/Result.hpp>
#include <string>
#include <cstdint>
#include <memory>
#include <vector>
BEGIN_NAMESPACE(n19::argp);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This header file contains definitions for n19's command line argument parsing
// library. An example usecase is as follows:
//
// struct MyArgs : public n19::argp::Parser {
//   int64_t& num_jobs  = arg("--num-jobs", "-j", 6);
//   std::string& input = arg("--input, "-i");
// };
//
// arguments should be passed to the "arg" method in the following order:
// long form flag name, short form, default value (optional).
// Afterwards, arguments can be parsed using the parse() method. If no errors
// occurred, the values can be retrieved directly using the object's members.

class ValueBase {
public:
  virtual auto convert(const sys::String&) -> Result<void> = 0;
  virtual ~ValueBase() = default;
};

template<class T>
class Value final : public ValueBase {
public:
  auto convert(const sys::String&) -> Result<void> override = delete;
  auto value() -> void = delete;
private:
  ~Value() override = delete;
  Value()  = delete;
};

template<>
class Value<int64_t> final : public ValueBase {
N19_MAKE_COMPARABLE_ON(int64_t, value_);
public:
  auto convert(const sys::String&) -> Result<void> override;
  N19_FORCEINLINE auto value() -> int64_t& { return value_; }

  Value() = default;
 ~Value() override = default;
private:
  int64_t value_ = 0;
};

template<>
class Value<bool> final : public ValueBase {
N19_MAKE_COMPARABLE_ON(bool, value_);
public:
  auto convert(const sys::String&)  -> Result<void> override;
  N19_FORCEINLINE auto value() -> bool& { return value_; }

  Value() = default;
 ~Value() override = default;
private:
  bool value_ = false;
};

template<>
class Value<double> final : public ValueBase {
N19_MAKE_COMPARABLE_ON(double, value_);
public:
  auto convert(const sys::String&) -> Result<void> override;
  N19_FORCEINLINE auto value() -> double& { return value_; }

  Value() = default;
 ~Value() override = default;
private:
  double value_ = false;
};

template<>
class Value<sys::String> final : public ValueBase {
N19_MAKE_COMPARABLE_ON(sys::String, value_);
public:
  auto convert(const std::string &) -> Result<void> override;
  N19_FORCEINLINE auto value() -> sys::String& { return value_; }

  Value() = default;
 ~Value() override = default;
private:
  sys::String value_{};
};

struct Parameter {
  std::string_view long_;
  std::string_view short_;
  std::string_view desc_;
  std::unique_ptr<ValueBase> value_;
  bool is_required_ = false;
};

class Parser {
public:


private:
  std::vector<Parameter> params;
};

END_NAMESPACE(n19::argp);
#endif //ARGPARSE_HPP
