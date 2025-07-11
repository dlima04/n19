/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <catch2/catch_test_macros.hpp>
#include <Core/ArgParse.hpp>
#include <Core/Console.hpp>

using namespace n19;

/// UNIX style arguments
struct DummyParser1 : public argp::Parser {
  int64_t& i64_param = arg<int64_t>(
    _nstr("--i64-arg"), _nstr("-i64"),
    _nstr(""), 343);
  
  bool& bool_param = arg<bool>(
    _nstr("--bool-arg"), _nstr("-bool"),
    _nstr(""),  true);

  sys::String& str_param = arg<sys::String>(
    _nstr("--string-arg"), _nstr("-string"), 
    _nstr(""), _nstr("default value"));
  
  double& double_param = arg<double>(
    _nstr("--double-arg"), _nstr("-double"),
    _nstr(""), 3.14);

  argp::PackType& pack_param = arg<argp::PackType>(
    _nstr("--pack-arg"), 
    _nstr("-pack"),
    _nstr(""), 
    argp::PackType{_nstr("foo"), _nstr("bar"), _nstr("baz")});
};

/// DOS style arguments
struct DummyParser2 : public argp::Parser {
  int64_t& i64_param = arg<int64_t>(
    _nstr("/i64-arg"), _nstr("/i64"),
    _nstr(""), 343);

  bool& bool_param = arg<bool>(
    _nstr("/bool-arg"), _nstr("/bool"),
    _nstr(""), true);

  sys::String& str_param = arg<sys::String>(
    _nstr("/string-arg"), _nstr("/string"),
    _nstr(""), _nstr("default value"));

  double& double_param = arg<double>(
    _nstr("/double-arg"), _nstr("/double"),
    _nstr(""), 3.14);

  argp::PackType& pack_param = arg<argp::PackType>(
    _nstr("/pack-arg"), _nstr("/pack"),
    _nstr(""));
};

/// idk
struct DummyParser3 : public argp::Parser {
  int64_t& i64_param = arg<int64_t>(
    _nstr(":i64-arg"), _nstr(":i64"),
    _nstr(""), 343);

  bool& bool_param = arg<bool>(
    _nstr(":bool-arg"), _nstr(":bool"),
    _nstr(""), true);

  sys::String& str_param = arg<sys::String>(
    _nstr(":string-arg"), _nstr(":string"),
    _nstr(""), _nstr("default value"));

  double& double_param = arg<double>(
    _nstr(":double-arg"), _nstr(":double"),
    _nstr(""), 3.14);

  argp::PackType& pack_param = arg<argp::PackType>(
    _nstr(":pack-arg"), _nstr(":pack"),
    _nstr(""));
};

TEST_CASE("Defaults", "[Core.ArgParse]") {
  DummyParser1 parser1;

  /// Check that default arguments were added correctly
  REQUIRE(parser1.i64_param == 343);
  REQUIRE(parser1.bool_param);
  REQUIRE(parser1.str_param == _nstr("default value"));
  REQUIRE(parser1.double_param == 3.14);

  /// pack type
  REQUIRE(parser1.pack_param.size() == 3);
  REQUIRE(parser1.pack_param[0] == _nstr("foo"));
  REQUIRE(parser1.pack_param[1] == _nstr("bar"));
  REQUIRE(parser1.pack_param[2] == _nstr("baz"));
}

TEST_CASE("UnixStyle", "[Core.ArgParse]") {
  std::vector<sys::String> longform_unix =
  { _nstr("--i64-arg"),    _nstr("341"),
    _nstr("--bool-arg"),   _nstr("false"),
    _nstr("--double-arg"), _nstr("69.69"),
    _nstr("--string-arg"), _nstr("idk"),
    _nstr("--pack-arg"),   _nstr("bim,bam,pow,bar")
  };

  std::vector<sys::String> shortform_unix =
  { _nstr("-i64=341"), /// use the "=" syntax as well.
    _nstr("-bool"),   _nstr("false"),
    _nstr("-double=69.69"),
    _nstr("-string"), _nstr("idk"),
    _nstr("-pack=bim,bam,pow,bar")
  };

  SECTION("unix_longform") {
    DummyParser1 parser1;
    auto res = parser1
      .style(argp::ArgStyle::UNIX)
      .take_argv(std::move(longform_unix))
      .parse(nulls());

    REQUIRE(res.has_value());
    REQUIRE(!parser1.bool_param);
    REQUIRE(parser1.double_param == 69.69);
    REQUIRE(parser1.i64_param == 341);
    REQUIRE(parser1.str_param == _nstr("idk"));

    REQUIRE(parser1.pack_param.size() == 4);
    REQUIRE(parser1.pack_param[0] == _nstr("bim"));
    REQUIRE(parser1.pack_param[1] == _nstr("bam"));
    REQUIRE(parser1.pack_param[2] == _nstr("pow"));
    REQUIRE(parser1.pack_param[3] == _nstr("bar"));
  }

  SECTION("unix_shortform") {
    DummyParser1 parser1;
    auto res = parser1
      .style(argp::ArgStyle::UNIX)
      .take_argv(std::move(shortform_unix))
      .parse(nulls());

    REQUIRE(res.has_value());
    REQUIRE(!parser1.bool_param);
    REQUIRE(parser1.double_param == 69.69);
    REQUIRE(parser1.i64_param == 341);
    REQUIRE(parser1.str_param == _nstr("idk"));

    REQUIRE(parser1.pack_param.size() == 4);
    if(parser1.pack_param.size() == 4) {
      REQUIRE(parser1.pack_param[0] == _nstr("bim"));
      REQUIRE(parser1.pack_param[1] == _nstr("bam"));
      REQUIRE(parser1.pack_param[2] == _nstr("pow"));
      REQUIRE(parser1.pack_param[3] == _nstr("bar"));
    }
  }
}

TEST_CASE("MasqStyle", "[Core.ArgParse]") {
  std::vector<sys::String> masq =
  { _nstr(":i64-arg"),    _nstr("341"),
    _nstr(":bool-arg"),   /// This should be implicitly true once parsed.
    _nstr(":double-arg=69.69"),
    _nstr(":string-arg"), _nstr("string thingy"),
    _nstr(":pack-arg"),   _nstr("single")
  };

  DummyParser3 parser1;
  parser1.bool_param = false;

  auto res = parser1
    .style(argp::ArgStyle::Masq)
    .take_argv(std::move(masq))
    .parse(nulls());

  REQUIRE(res.has_value());
  REQUIRE(parser1.bool_param);
  REQUIRE(parser1.double_param == 69.69);
  REQUIRE(parser1.i64_param == 341);
  REQUIRE(parser1.str_param == _nstr("string thingy"));

  REQUIRE(parser1.pack_param.size() == 1);
  if(parser1.pack_param.size() == 1) {
    REQUIRE(parser1.pack_param[0] == _nstr("single"));
  }
}

TEST_CASE("DosStyle", "[Core.ArgParse]") {
  /// Let's use a vector this time
  std::vector<sys::String> dos =
  { _nstr("/i64"),    _nstr("341"),
    _nstr("/bool"),   _nstr("false"),
    _nstr("/double"), _nstr("69.69"),
    _nstr("/string"), _nstr("idk"),
    _nstr("/pack"),   _nstr("bim,bam,pow,bar")
  };

  DummyParser2 parser1;
  auto res = parser1
    .style(argp::ArgStyle::DOS)
    .take_argv(std::move(dos))
    .parse(nulls());

  REQUIRE(res.has_value());
  REQUIRE(!parser1.bool_param);
  REQUIRE(parser1.double_param == 69.69);
  REQUIRE(parser1.i64_param == 341);
  REQUIRE(parser1.str_param == _nstr("idk"));

  REQUIRE(parser1.pack_param.size() == 4);
  REQUIRE(parser1.pack_param[0] == _nstr("bim"));
  REQUIRE(parser1.pack_param[1] == _nstr("bam"));
  REQUIRE(parser1.pack_param[2] == _nstr("pow"));
  REQUIRE(parser1.pack_param[3] == _nstr("bar"));
}

///
/// For testing incorrect types
struct JustI64 : public argp::Parser {
  int64_t& arg_ = arg<int64_t>(_nstr("--arg"), _nstr("-a"));
};

struct JustDouble : public argp::Parser {
  double& arg_ = arg<double>(_nstr("--arg"), _nstr("-a"));
};

struct JustBool : public argp::Parser {
  bool& arg_ = arg<bool>(_nstr("--arg"), _nstr("-a"), _nstr(""), false);
};

TEST_CASE("BadTypes", "[Core.ArgParse]") {
  JustI64 parser_i64;
  JustDouble parser_double;
  JustBool parser_bool;

  /// Test parser for integer values
  REQUIRE(!parser_i64
    .style(argp::ArgStyle::UNIX)
    .take_argv(std::vector<sys::String>{_nstr("--arg=true")})
    .parse(nulls())
    .has_value());

  REQUIRE(!parser_i64
    .style(argp::ArgStyle::UNIX)
    .take_argv(std::vector<sys::String>{_nstr("--arg=foobar")})
    .parse(nulls())
    .has_value());

  REQUIRE(!parser_i64
    .style(argp::ArgStyle::UNIX)
    .take_argv(std::vector<sys::String>{_nstr("--arg=\t\b\n")})
    .parse(nulls())
    .has_value());

  /// Test parser for doubles
  REQUIRE(!parser_double
    .style(argp::ArgStyle::UNIX)
    .take_argv(std::vector<sys::String>{_nstr("--arg=foobar")})
    .parse(nulls())
    .has_value());

  REQUIRE(!parser_double
    .style(argp::ArgStyle::UNIX)
    .take_argv(std::vector<sys::String>{_nstr("--arg=\t~~\b\n")})
    .parse(nulls())
    .has_value());

  /// Test parser for bools
  REQUIRE(!parser_bool
    .style(argp::ArgStyle::UNIX)
    .take_argv(std::vector<sys::String>{_nstr("--arg=12331false")})
    .parse(nulls())
    .has_value());

  REQUIRE(!parser_bool
    .style(argp::ArgStyle::UNIX)
    .take_argv(std::vector<sys::String>{_nstr("--arg=foobartrue")})
    .parse(nulls())
    .has_value());

  REQUIRE(!parser_bool
    .style(argp::ArgStyle::UNIX)
    .take_argv(std::vector<sys::String>{_nstr("--arg=\t\n\bfalse\t")})
    .parse(nulls())
    .has_value());
}

TEST_CASE("BadNames", "[Core.ArgParse]") {
  DummyParser1 parser1;
  DummyParser1 parser2;
  DummyParser1 parser3;

  std::vector<sys::String> name_doesnt_exist =
  { _nstr("--foobar"), _nstr("3123"),
    _nstr("\t\nbarbaz")
  };

  std::vector<sys::String> name_doesnt_exist2 =
  { _nstr("--i64-arg"),    _nstr("341"),
    _nstr("--bool-arg"),   _nstr("false"),
    _nstr("--double-arg"), _nstr("69.69"),
    _nstr("--string-arg"), _nstr("idk"),
    _nstr("--pack-arg"),   _nstr("bim,bam,pow,bar"),
    _nstr("--fake-arg") /// not real
  };

  std::vector<sys::String> style_mismatch =
  { _nstr("/i64-arg"),    _nstr("341"),
    _nstr("/bool-arg"),   _nstr("false"),
    _nstr(":double-arg"), _nstr("69.69"),
    _nstr(":string-arg"), _nstr("idk"),
    _nstr("/pack-arg"),   _nstr("bim,bam,pow,bar")
  };

  REQUIRE(!parser1
    .style(argp::ArgStyle::UNIX)
    .take_argv(std::move(name_doesnt_exist))
    .parse(nulls())
    .has_value());

  REQUIRE(!parser2
    .style(argp::ArgStyle::UNIX)
    .take_argv(std::move(style_mismatch))
    .parse(nulls())
    .has_value());

  REQUIRE(!parser3
    .style(argp::ArgStyle::UNIX)
    .take_argv(std::move(name_doesnt_exist2))
    .parse(nulls())
    .has_value());
}
