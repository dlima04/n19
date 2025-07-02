/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Reporting.hpp>
#include <Bulwark/BulwarkContext.hpp>
#include <Core/Fmt.hpp>
#include <Core/Panic.hpp>
BEGIN_NAMESPACE(n19::test);

constinit TallyType g_total_passed  = 0;
constinit TallyType g_total_failed  = 0;
constinit TallyType g_total_exc     = 0;
constinit TallyType g_total_skipped = 0;
constinit TallyType g_total_suites  = 0;

auto report(const std::string_view& e, Result r, OStream& stream, size_t indent) -> void {
  auto should_use_colours = Context::the().flags_ & Context::Colours;
  auto expr = std::string{ e };        /// Copy into new string object so we can modify
  for(size_t i = 0; i < indent; i++)   /// Add indentation.
    expr.insert(0, "  ");              ///

  expr += " ";                         ///
  stream << fmt("{:.<75} ", expr);     /// Display expression with padding
  if(should_use_colours) stream << r.to_colour();
  stream << r.to_string() << '\n';
  if(should_use_colours) stream << Con::Reset;
}

auto diagnostic(const std::string_view& m, Diagnostic diag, OStream& stream, size_t indent) -> void {
  auto should_use_colours = Context::the().flags_ & Context::Colours;
  auto message = std::string{ m };     /// Same thing for a diagnostic...
  for(size_t i = 0; i < indent; i++)   ///
    message.insert(0, "  ");

  message += " ";                      ///
  stream << fmt("{:.<75} ", message);  /// Pad diagnostic, check if we need colours.
  if(should_use_colours) stream << diag.to_colour();
  stream << diag.to_string() << '\n';
  if(should_use_colours) stream << Con::Reset;
}

auto report(const Case &c, Result r, OStream &stream, size_t indent) -> void {
  report(fmt("  Case \"{}\"", c.name_), r, stream, indent);
}

auto report(const Suite& suite, OStream& stream) -> void {
  auto should_use_colours = Context::the().flags_ & Context::Colours;
  if(should_use_colours) stream << Con::Bold;
  stream << "Suite \"" << suite.name_ << "\":\n";
  if(should_use_colours) stream << Con::Reset;
}

auto report(const std::string_view& s, OStream& stream, size_t indent) -> void {
  auto section = std::string{ s };
  for(size_t i = 0; i < indent; i++) section.insert(0, "  ");
  section += " ";
  stream << fmt("{:.<75} SECTION\n", section);
}

auto Diagnostic::to_string() const -> std::string {
  switch(val_) {
  case Warn:  return "WARN";
  case Info:  return "INFO";
  case Fatal: return "FATAL";
  case Debug: return "DEBUG";
  default: break;
  }

  UNREACHABLE_ASSERTION;
}

auto Diagnostic::to_colour() const -> Con {
  switch(val_) {
  case Info:  return Con::CyanFG;
  case Fatal: return Con::RedFG;
  case Warn:  return Con::YellowFG;
  case Debug: return Con::BlueFG;
  default: break;
  }

  UNREACHABLE_ASSERTION;
}

END_NAMESPACE(n19::test);