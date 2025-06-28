/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <System/SharedRegion.hpp>
#include <Core/Console.hpp>

#ifdef N19_WIN32
#include <System/Win32.hpp>
#else // POSIX
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

BEGIN_NAMESPACE(n19::sys);

#ifdef N19_WIN32

auto SharedRegion::create_impl_(
  const String &name,
  const size_t max_length,
  const bool   open_if_exists,
  const size_t length ) -> Result<SharedRegion>
{
  SharedRegion sr;
  sr.name_ = sys::String(L"Local\\") + name;
  sr.size_ = length == 0 ? max_length : length;

  if(max_length == 0) {
    return Error(ErrC::InvalidArg);
  }

  /// Likely already 64 bits but... eh.
  const uint64_t len64 = static_cast<uint64_t>(max_length);

  /// Create shared memory object.
  sr.value_ = ::CreateFileMappingW(
    INVALID_HANDLE_VALUE,
    nullptr,
    PAGE_READWRITE,
    static_cast<::DWORD>((len64 >> 32) & 0xFFFFFFFFU),
    static_cast<::DWORD>(len64 & 0xFFFFFFFFU),
    sr.name_.c_str()
  );

  if(sr.value_ == (::HANDLE)nullptr
    || (::GetLastError() == ERROR_ALREADY_EXISTS && !open_if_exists))
  {
    return Error::from_native();
  }

  if (::GetLastError() == ERROR_ALREADY_EXISTS) {
    ::SetLastError(ERROR_SUCCESS);
  }

  sr.addr_ = ::MapViewOfFile(sr.value_, FILE_MAP_ALL_ACCESS, 0, 0, length);
  if(sr.addr_ == nullptr) {
    ::CloseHandle(sr.value_);
    return Error::from_native();
  }

  return sr;
}

auto SharedRegion::open(const String& name, const size_t size) -> Result<SharedRegion> {
  SharedRegion sr;
  sr.name_ = sys::String(L"Local\\") + name;
  sr.size_ = size;
  sr.value_ = ::OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, sr.name_.c_str());

  if(sr.value_ == (::HANDLE)nullptr) {
    return Error::from_native();
  }

  sr.addr_ = ::MapViewOfFile(sr.value_, FILE_MAP_ALL_ACCESS, 0, 0, sr.size_);
  if(sr.addr_ == nullptr) {
    ::CloseHandle(sr.value_);
    return Error::from_native();
  }

  return sr;
}

auto SharedRegion::create_or_open(
  const String &name,
  const size_t max_size,
  const size_t size ) -> Result<SharedRegion>
{
  return create_impl_(name, max_size, true, size);
}

auto SharedRegion::create(
  const String &name,
  const size_t max_size,
  const size_t size ) -> Result<SharedRegion>
{
  return create_impl_(name, max_size, false, size);
}

auto SharedRegion::is_invalid() -> bool {
  return value_ == (::HANDLE)nullptr
    || addr_ == nullptr
    || size_ == 0;
}

auto SharedRegion::invalidate() -> void {
  this->addr_ = nullptr;
  this->size_ = 0;
  this->value_ = (::HANDLE)nullptr;
}

auto SharedRegion::close() -> void {
  if(addr_  != nullptr) ::UnmapViewOfFile(addr_);
  if(value_ != (::HANDLE)nullptr) ::CloseHandle(value_);
  invalidate();
}

auto SharedRegion::destroy() -> void {
  /* On Windows, there is no way to
   * manually destroy a file mapping object.
   * These objects are destroyed once their reference count
   * reaches zero. That is, when the last process
   * that has opened a handle to that object closes it.
  */
  this->close();
}

#else // POSIX

auto SharedRegion::create_impl_(
  const String &name,
  const size_t max_length,
  const bool   open_if_exists,
  const size_t length ) -> Result<SharedRegion>
{
  constexpr int oflags = O_CREAT | O_RDWR;
  struct ::stat statbuff = { 0 };
  SharedRegion sr;

  sr.name_ = name;
  sr.size_ = length;

  if(max_length == 0) {
    return Error::from_error_code(EINVAL);
  }

  /// Open a new or existing shared memory object.
  sr.value_ = ::shm_open(
    name.c_str(),
    (!open_if_exists) ? oflags | O_EXCL : oflags,
    S_IRUSR | S_IWUSR
  );

  if(sr.value_ == -1) {
    return Error::from_native();
  }

  if(::fstat(sr.value_, &statbuff) == -1) {
    ::close(sr.value_);
    return Error::from_native();
  }

  /// If the object is newly created, i.e. it didn't exist before
  /// our call to shm_open(), we'll need to set the size of
  /// the object to max_length via ftruncate()

  if(::ftruncate(sr.value_, static_cast<off_t>(max_length)) == -1) {
    ::close(sr.value_);
    return Error::from_native();
  }

  if(sr.size_ == 0) {
    sr.size_ = max_length;
  }

  /// Finally, map the object into this process' address space via mmap().
  sr.addr_ = ::mmap(
    nullptr,
    sr.size_,
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    sr.value_,
    0
  );

  if(sr.addr_ == MAP_FAILED) {
    sr.addr_ = nullptr;
    ::close(sr.value_);
    if(!open_if_exists) {
      ::shm_unlink(sr.name_.c_str());
    }

    return Error::from_native();
  }

  return sr;
}

auto SharedRegion::create(
  const String &name,
  const size_t max_size,
  const size_t size ) -> Result<SharedRegion>
{
  return create_impl_(name, max_size, false, size);
}

auto SharedRegion::create_or_open(
  const String &name,
  const size_t max_size,
  const size_t size ) -> Result<SharedRegion>
{
  return create_impl_(name, max_size, true, size);
}

auto SharedRegion::open(
  const String &name,
  const size_t size ) -> Result<SharedRegion>
{
  SharedRegion sr;
  sr.name_  = name;
  sr.size_  = size;
  sr.value_ = ::shm_open(name.c_str(), O_RDWR, 0);

  if(sr.value_ == -1) {
    return Error::from_native();
  }

  struct ::stat statbuff = { 0 };
  if(::fstat(sr.value_, &statbuff) == -1) {
    ::close(sr.value_);
    return Error::from_native();
  }

  if(sr.size_ == 0) {
    sr.size_ = static_cast<size_t>(statbuff.st_size);
  }

  /// Map it
  sr.addr_ = ::mmap(
    nullptr,
    sr.size_,
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    sr.value_,
    0
  );

  if(sr.addr_ == MAP_FAILED) {
    ::close(sr.value_);
    return Error(ErrC::Native, "::mmap failed.");
  }

  return sr;
}

auto SharedRegion::close() -> void {
  if(addr_ != nullptr) {
    ::munmap(addr_, size_);
  }
  if(value_ != -1) {
    ::close(value_);
  }

  invalidate();
}

auto SharedRegion::is_invalid() -> bool {
  return this->value_ == -1
    || this->addr_ == nullptr
    || this->size_ == 0
    || this->name_.empty();
}

auto SharedRegion::destroy() -> void {
  close();
  if(!name_.empty()) {
    ::shm_unlink(name_.c_str());
    name_.clear();
  }
}

auto SharedRegion::invalidate() -> void {
  this->value_ = -1;
  this->size_  = 0;
  this->addr_  = nullptr;
}

#endif
END_NAMESPACE(n19::sys);