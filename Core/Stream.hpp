/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef STREAM_HPP
#define STREAM_HPP
#include <Sys/IODevice.hpp>
#include <Core/Bytes.hpp>
#include <Core/Platform.hpp>
#include <Core/Concepts.hpp>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <system_error>
#include <charconv>
#include <limits>

#if defined(N19_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <vector>
#else //POSIX
#  include <locale.h>
#endif

#ifdef N19_LARGE_OSTREAM_BUFFERS
#  define N19_OSTREAM_BUFFSIZE 2048LU
#else
#  define N19_OSTREAM_BUFFSIZE 1024LU
#endif

BEGIN_NAMESPACE(n19);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Some stream manipulators, their values are not actually used,
// the types themselved as an indicator of what the stream should do.

inline constexpr struct __Flush {
  unsigned char __dummy_value = 0;
  constexpr __Flush() = default;
} Flush;

inline constexpr struct __Endl {
  unsigned char __dummy_value = 0;
  constexpr __Endl() = default;
} Endl;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OStream is a very basic stream class that supports writing compiler
// specific types and strings of different encodings, including UTF16 and UTF8.
// This is the most basic kind of stream, and as such does not use any kind of
// buffering internally. It simply writes directly to whatever file descriptor it's using.

class OStream {
public:
  using __Index  = size_t;
  using __Char   = Byte;
  using __Span   = std::span<const __Char>;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Factories

  static auto from_stdout() -> OStream;
  static auto from_stderr() -> OStream;
  static auto from(const sys::IODevice&) -> OStream;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Begin inline class methods

  N19_FORCEINLINE auto operator<<(const __Flush&) -> OStream& {
    this->flush();   /// Flush but don't add a newline
    return *this;    /// return this stream
  }

  N19_FORCEINLINE auto operator<<(const __Endl&) -> OStream& {
    *this << '\n';   /// add a new line
    this->flush();   /// Flush
    return *this;    /// return this stream
  }

  template<Character T>
  auto operator<<(const T value) -> OStream& {
    T buffer[ 2 ];
    buffer[0] = value;
    buffer[1] = static_cast<T>(0);
    return *this << buffer;
  }

  template<IntOrFloat T> requires(!IsCharacter<T>)
  auto operator<<(const T value) -> OStream& {
    char buff[ 40 ] = { 0 };   /// Conversion buffer
    auto [ptr, ec]  = std::to_chars(buff, buff + sizeof(buff) - 1, value);

    buff[39] = '\0';           /// shizophrenia
    if(ec == std::errc()) *this << std::string_view{buff};
    return *this;
  }

  template<Pointer T> requires(!IsCharacter<RemovePointer<T>>)
  auto operator<<(const T value) -> OStream& {
    const auto conv  = reinterpret_cast<uintptr_t>(value);
    char buff [ 40 ] = { 0 };  /// conversion buffer
    auto [ptr, ec]   = std::to_chars(buff, buff + sizeof(buff) - 1, conv, 16);

    buff[39] = '\0';           /// shizophrenia
    if(ec == std::errc()) *this << std::string_view{buff};
    return *this;
  }

  auto operator<<(const std::string_view& str) -> OStream& {
    if(!str.empty()) this->write(as_bytes(str));
    return *this;
  }

  auto operator<<(const std::u8string_view& str) -> OStream& {
    if(!str.empty()) this->write(as_bytes(str));
    /// For windows, this relies on the fact that
    /// the code page for the console should have been
    /// set to UTF8 ahead of time. If not, this won't work.
    return *this;
  }

#if defined(N19_WIN32)
  auto operator<<(const std::wstring_view& str) -> COStream & {
    if(str.empty()) return *this;

    const int req_size = WideCharToMultiByte(
      CP_UTF8,         /// Code page: UTF-8
      0,               /// Conversion flags
      str.data(),      /// Source UTF-16 string
      str.size(),      /// Number of codepoints
      nullptr,         /// No output buffer yet
      0,               /// Request buffer size
      nullptr, nullptr /// Default char mappings (unused for UTF-8)
    );

    if(req_size == 0) return *this;
    std::vector<char> outbuf((size_t)req_size, '\0');

    const int result = WideCharToMultiByte(
      CP_UTF8,         /// Code page: UTF-8
      0,               /// Conversion flags
      str.data(),      /// Source UTF-16 string
      str.size(),      /// Number of codepoints
      outbuf.data(),   /// Destination buffer
      outbuf.size(),   /// Buffer size
      nullptr, nullptr /// Default char mappings (unused for UTF-8)
    );

    std::string_view the_view{ outbuf.begin(), outbuf.end() };
    return *this << the_view;
  }

#else // POSIX
  auto operator<<(const std::wstring_view& str) -> OStream& {
    if(!str.empty()) this->write(as_bytes(str));
    /// For POSIX systems, who cares.
    /// This will never get called anyways,
    /// since UTF16 is only ever used on Windows.
    return *this;
  }
#endif

  virtual auto write(const __Span& buff) -> OStream& {
    fd_.write(buff);     /// Write to the underlying IODevice object.
    return *this;        /// return this stream
  }

  virtual auto flush() -> OStream& {
    fd_.flush_handle();  /// sync handle
    return *this;
  }

  virtual ~OStream() = default;
  OStream() = default;
protected:
  sys::IODevice fd_;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BufferedOStream<size_> is the buffered version of OStream.
// It uses a fixed-size buffer internally, of which the size is determined by
// the template parameter. The size should not be 0.

template<size_t size_>
class BufferedOStream : public OStream {
public:
  constexpr static size_t __size  = size_;
  constexpr static size_t __begin = 0;
  constexpr static size_t __end   = __size - 1;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Factories

  static auto from_stdout() -> BufferedOStream {
    auto the_stream = BufferedOStream<size_>();
    the_stream.fd_  = sys::IODevice::from_stdout();
    return the_stream;
  }

  static auto from_stderr() -> BufferedOStream {
    auto the_stream = BufferedOStream<size_>();
    the_stream.fd_  = sys::IODevice::from_stderr();
    return the_stream;
  }

  static auto from(const sys::IODevice& dev) -> BufferedOStream {
    auto the_stream = BufferedOStream<size_>();
    the_stream.fd_  = dev;
    return the_stream;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Type aliases

  using __Buffer = OStream::__Char[ __size ];
  using __Index  = OStream::__Index;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Begin inline class methods

  N19_FORCEINLINE auto to_buffer(const __Span& buff) -> OStream& {
    ASSERT(curr_ <= __size, "Invalid current buffer index.");
    ASSERT(buff.size_bytes() <= (__size - curr_), "Buffer overrun!");
    ASSERT(!buff.empty(), "to_buffer: empty span!");

    std::memcpy(&buff_[curr_], buff.data(), buff.size_bytes());
    curr_ += buff.size_bytes();
    return *this;
  }

  N19_FORCEINLINE auto flush() -> OStream& override {
    ASSERT(curr_ <= __size, "Buffer overrun!");
    if(curr_ > __begin) {
      fd_.write(Bytes{&buff_[__begin], curr_});
      curr_ = __begin;        /// Reset the buffer index.
    }                         ///

    fd_.flush_handle();       /// FIXME: this is retarded.
    return *this;             /// return instance
  }

  auto write(const __Span& buff) -> OStream& override {
    const size_t remaining = __size - curr_;
    const size_t size_new  = buff.size_bytes();
    const bool   no_space  = remaining < size_new;

    if(buff.empty()) {        /// Disallow empty buffers.
      return *this;           /// Early return.
    } if(size_new > __size) { /// buffer is too large.
      flush();                ///
      fd_.write(buff);        /// write contents directly.
    } else if(no_space) {     /// remaining size is too small,
      flush();                /// flush the buffer.
      to_buffer(buff);        /// copy the new contents over.
    } else {                  ///
      to_buffer(buff);        /// copy the new contents over.
    }                         ///

    return *this;
  }

 ~BufferedOStream() override = default;
  BufferedOStream() = default;
protected:
  __Buffer buff_{};
  __Index curr_ {__begin};
};

class NullOStream final : public OStream {
public:
  auto write(const __Span&) -> OStream & override { return *this; }
  auto flush() -> OStream & override { return *this; }

 ~NullOStream() override = default;
  NullOStream() = default;
};

class IStream {
public:
  static auto from_stdin() -> IStream;
  static auto from(const sys::IODevice&) -> IStream;

  N19_FORCEINLINE ~IStream() { fd_.flush_handle(); }
  N19_FORCEINLINE IStream()  { /* ... */ }
private:
  sys::IODevice fd_;
};

END_NAMESPACE(n19);
#endif //STREAM_HPP
