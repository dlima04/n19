/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#  ifndef N19_WIN32
#include <unistd.h>
#include <sys/mman.h>
#  else
#include <n19/System/Win32.hpp>
#  endif

#include <cstdint>
#include <n19/Core/Common.hpp>
#include <n19/Core/Platform.hpp>
#include <n19/Core/Result.hpp>
BEGIN_NAMESPACE(n19::sys);

struct PageAllocator {
  NODISCARD_ static void* alloc(size_t size, void* hint = nullptr) {
#  ifdef N19_WIN32
    return ::VirtualAlloc(hint, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#  else
    return ::mmap(hint, size,
      PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#  endif //N19_WIN32
  }

  static void free(void* addr, [[maybe_unused]] size_t size) {
#  ifdef N19_WIN32
    ::VirtualFree(addr, 0, MEM_RELEASE);
#  else
    ::munmap(addr, size);
#  endif //N19_WIN32
  }

  NODISCARD_ static size_t page_size() {
#  ifdef N19_WIN32
    ::SYSTEM_INFO info{};
    ::GetSystemInfo(&info);
    return static_cast<size_t>(info.dwPageSize);
#  else
    return static_cast<size_t>(::sysconf(_SC_PAGESIZE));
#  endif
  }

  PageAllocator() = delete;
};

END_NAMESPACE(n19::sys);