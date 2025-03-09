/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Sys/File.hpp>
#include <Sys/Error.hpp>
#include <Core/Try.hpp>
#include <utility>

namespace stdfs = std::filesystem;

#if defined(N19_POSIX)
#include <sys/stat.h>
#include <sys/conf.h>
BEGIN_NAMESPACE(n19::sys);

NODISCARD_ auto File::create_or_open(
  const String& name,
  const bool append,
  const uint8_t perms ) -> Result<File>
{
  int the_perms = O_RDWR;
  ERROR_IF(perms == NoAccess, ErrC::InvalidArg);
  if((perms & Read) && !(perms & Write)) the_perms = O_RDONLY;
  if(append) the_perms |= O_APPEND;

  the_perms |= O_CREAT | O_CLOEXEC;
  File the_device;
  the_device.perms_ = perms;
  the_device.value_ = ::open(name.c_str(), the_perms, 0644);
  the_device.name_  = name;

  if(the_device.value_ == -1) return Error::from_native();
  return the_device;
}

NODISCARD_ auto File::open(
  const String& name,
  const bool append,
  const uint8_t perms ) -> Result<File>
{
  int the_perms = O_RDWR;
  ERROR_IF(perms == NoAccess, ErrC::InvalidArg);
  if((perms & Read) && !(perms & Write)) the_perms = O_RDONLY;
  if(append) the_perms |= O_APPEND;

  File the_device;
  the_device.perms_ = perms;
  the_device.value_ = ::open(name.c_str(), the_perms | O_CLOEXEC, 0644);
  the_device.name_  = name;

  if(the_device.value_ == -1) return Error::from_native();
  return the_device;
}

NODISCARD_ auto File::create_trunc(
  const String& name,
  const uint8_t perms ) -> Result<File>
{
  int the_perms = O_RDWR;
  ERROR_IF(perms == NoAccess, ErrC::InvalidArg);
  if((perms & Read) && !(perms & Write)) the_perms = O_RDONLY;

  the_perms |= O_CREAT | O_CLOEXEC | O_TRUNC;
  File the_device;
  the_device.perms_ = perms;
  the_device.value_ = ::open(name.c_str(), the_perms, 0644);
  the_device.name_  = name;

  if(the_device.value_ == -1) return Error::from_native();
  return the_device;
}

NODISCARD_ auto File::size() const -> Result<size_t> {
  struct ::stat info{};
  if(::fstat(value_, &info) == -1) return Error::from_native();
  return Result<size_t>::create((size_t)info.st_size);
}

NODISCARD_ auto File::path() const -> stdfs::path {
  return stdfs::path( this->name_ );
}

Result<SeekDist> File::seek(SeekDist dist, FSeek method) const {
  int whence;
  switch(method) {     /// Determine posix positioning method.
  case FSeek::Beg:
    whence = SEEK_SET; /// Start from the beginning of the file.
    break;             ///
  case FSeek::End:
    whence = SEEK_END; /// Start from the end of the file.
    break;             ///
  case FSeek::Cur:
    whence = SEEK_CUR; /// Maintain the current position.
    break;             ///
  default:
    UNREACHABLE_ASSERTION;
  }

  SeekDist newpos = ::lseek(value_, dist, whence);
  if(newpos == -1) return Error::from_native();
  return newpos;
}

END_NAMESPACE(n19::sys);
#else // IF WINDOWS
BEGIN_NAMESPACE(n19::sys);

Result<SeekDist> File::seek(SeekDist dist, FSeek method) const {
  DWORD whence;
  switch(method) {
  case FSeek::Beg:
    whence = FILE_BEGIN;
    break;
  case FSeek::End:
    whence = FILE_END;
    break;
  case FSeek::Cur:
    whence = FILE_CURRENT;
    break;
  default:
    UNREACHABLE_ASSERTION;
  }

  LARGE_INTEGER in_dist{ 0 };
  LARGE_INTEGER out_dist{ 0 };

  in_dist.QuadPart = dist;
  if(!SetFilePointerEx(value_, in_dist, &out_dist, whence)) {
    return Error::from_native();
  }

  return out_dist.QuadPart;
}

NODISCARD_ auto File::create_or_open(
  const String& name,
  const bool append,
  const uint8_t perms ) -> Result<File>
{
  DWORD the_perms = 0, shareperm = 0;
  if(perms & Read) {
    the_perms |= FILE_GENERIC_READ;  ///
    shareperm |= FILE_SHARE_READ;    /// Note: not sure if this is necessary.
  } if(perms & Write) {
    the_perms |= FILE_GENERIC_WRITE;
    shareperm |= FILE_SHARE_WRITE;
  } if(append) {
    the_perms |= FILE_APPEND_DATA;
  }

  File the_device;
  the_device.perms_ = perms;
  the_device.name_  = name;
  the_device.value_ = ::CreateFileW(
    name.c_str(), the_perms,
    shareperm, nullptr,
    OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
    nullptr
  );

  if(the_device.value_ == INVALID_HANDLE_VALUE)
    return Error::from_native();

  return the_device;
}

NODISCARD_ auto File::open(
  const String& name,
  const bool append,
  const uint8_t perms ) -> Result<File>
{
  DWORD the_perms = 0, shareperm = 0;
  if(perms & Read) {
    the_perms |= FILE_GENERIC_READ;
    shareperm |= FILE_SHARE_READ;
  } if(perms & Write) {
    the_perms |= FILE_GENERIC_WRITE;
    shareperm |= FILE_SHARE_WRITE;
  } if(append) {
    the_perms |= FILE_APPEND_DATA;
  }

  File the_device;
  the_device.perms_ = perms;
  the_device.name_  = name;
  the_device.value_ = ::CreateFileW(
    name.c_str(), the_perms,
    shareperm, nullptr,         ///
    OPEN_EXISTING, 0, nullptr   /// Open only if the file exists.
  );

  if(the_device.value_ == INVALID_HANDLE_VALUE)
    return Error::from_native();

  return the_device;
}

NODISCARD_ auto File::create_trunc(
  const String& name,
  const uint8_t perms ) -> Result<File>
{
  DWORD the_perms = 0, shareperm = 0;
  if(perms & Read) {
    the_perms |= FILE_GENERIC_READ;
    shareperm |= FILE_SHARE_READ;
  } if(perms & Write) {
    the_perms |= FILE_GENERIC_WRITE;
    shareperm |= FILE_SHARE_WRITE;
  } if(append) {
    the_perms |= FILE_APPEND_DATA;
  }

  File the_device;
  the_device.perms_ = perms;
  the_device.name_  = name;
  the_device.value_ = ::CreateFileW(
    name.c_str(), the_perms,
    shareperm, nullptr,         /// CREATE_ALWAYS will truncate the file if it
    CREATE_ALWAYS, 0, nullptr   /// already exists, erasing all contents.
  );

  if(the_device.value_ == INVALID_HANDLE_VALUE)
    return Error::from_native();

  return the_device;
}

NODISCARD_ auto File::size() const -> Result<size_t> {
  LARGE_INTEGER file_size{0};
  if(!GetFileSizeEx(value_, &file_size)) return Error::from_native();
  return Result<size_t>::create(file_size.QuadPart);
}

NODISCARD_ auto File::path() const -> stdfs::path {
  return stdfs::path( this->name_ );
}

END_NAMESPACE(n19::sys);
#endif //N19_POSIX
