/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/FrontendContext.hpp>
#include <bitset>

#ifdef N19_WIN32
#include <System/Win32.hpp>
#else
#include <sys/utsname.h>
#endif

namespace {
  constinit
  n19::InputFile::ID  g_inputfile_id_  = N19_INVALID_INFILE_ID + 1;

  constinit
  n19::OutputFile::ID g_outputfile_id_ = N19_INVALID_OUTFILE_ID + 1;
}

BEGIN_NAMESPACE(n19);
auto Context::get_version_info() -> VersionInfo {
  VersionInfo ver;
  ver.major = 0;
  ver.minor = 0;
  ver.patch = 0;

#ifdef N19_WIN32
  ::SYSTEM_INFO sysinfo{};
  ::GetNativeSystemInfo(&sysinfo);

  switch(sysinfo.wProcessorArchitecture) {
  case PROCESSOR_ARCHITECTURE_AMD64:
    ver.arch = "x64";
    break;
  case PROCESSOR_ARCHITECTURE_ARM:
    ver.arch = "ARM";
    break;
  case PROCESSOR_ARCHITECTURE_ARM64:
    ver.arch = "ARM64";
    break;
  case PROCESSOR_ARCHITECTURE_INTEL:
    ver.arch = "legacy x86";
    break;
  default:
    ver.arch = "<unknown architecture>";
    break;
  }

#else // POSIX
  struct ::utsname buff{};
  constexpr size_t i
    = (sizeof(buff.machine) / sizeof(buff.machine[0])) - 1;

  if(::uname(&buff) == 0) {
    buff.machine[ i ] = '\0';
    ver.arch = buff.machine;
  }
#endif

#if defined(N19_WIN32)
  ver.os = "microsoft-windows";
#elif defined(N19_DARWIN)
  ver.os = "apple-darwin";
#elif defined(N19_LINUX)
  ver.os = "linux";
#else
  ver.os = "<unknown operating system>";
#endif

  return ver;
}

/*
 * These retrieve a "locally unique" ID for input/output files.
 * This is a simple increasing integer value that is non-unique
 * across compilation units and is ONLY used to avoid
 * storing an excessive amount of strings to refer to different
 * source files by name.
 */
auto Context::get_next_input_id() -> InputFile::ID {
  InputFile::ID retval = g_inputfile_id_++;
  return retval;
}

auto Context::get_next_output_id() -> OutputFile::ID {
  OutputFile::ID retval = g_outputfile_id_++;
  return retval;
}

/*
 * Constructors for input/output files
 */
InputFile::InputFile(sys::String&& n) : name(n) {
  this->id = Context::get_next_input_id();
}

OutputFile::OutputFile(sys::String&& n) : name(n) {
  this->id = Context::get_next_output_id();
}

auto Context::dump(OStream& stream) -> void {
  stream
    << Con::MagentaFG
    << "Flags :: "
    << Con::Reset;
  std::string buff;

  #define X(NAME, UNUSED)         \
    if(flags_ & Context::NAME){   \
      buff += #NAME " | ";        \
    }

  N19_FRONTEND_CONTEXT_FLAG_LIST
  #undef X

  if(!buff.empty()) {
    buff.erase(buff.size() - 3);
  }

  stream
    << buff
    << Con::MagentaFG
    << "\nInputs ::\n"
    << Con::Reset;

  for(const InputFile& input : inputs_) {
    stream
      << " - ID="
      << Con::BlueFG
      << input.id
      << Con::Reset
      << "\n - Name=\""
      << Con::GreenFG
      << input.name
      << Con::Reset
      << "\"\n - State="
      << static_cast<size_t>(input.state)
      << "\n - Kind="
      << static_cast<size_t>(input.kind)
      << "\n";
  }

  stream
    << Con::MagentaFG
    << "Outputs ::\n"
    << Con::Reset;

  for(const OutputFile& output : outputs_) {
    stream
      << " - ID="
      << Con::BlueFG
      << output.id
      << Con::Reset
      << "\n - Name=\""
      << Con::GreenFG
      << output.name
      << Con::Reset
      << "\"\n";
  }

  stream
    << Con::MagentaFG
    << "RawFlags :: "
    << std::bitset<sizeof(flags_)>(flags_).to_string()
    << Con::Reset
    << "\n\n";
}

END_NAMESPACE(n19);
