/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <catch2/catch_test_macros.hpp>
#include <n19/System/Error.hpp>
#include <string>
using namespace n19;
using namespace n19::sys;

TEST_CASE("LastError", "[System.SystemError]") {
#ifdef N19_WIN32
  SetLastError(ERROR_FILE_NOT_FOUND);
#else
  errno = ENOENT;
#endif
  /// Test that last_error() returns a non-empty string
  std::string error_msg = last_error();
  REQUIRE(!error_msg.empty());  

#ifdef N19_WIN32
  SetLastError(0);
#else
  errno = 0;
#endif
} 

TEST_CASE("Translation", "[System.SystemError]") {
  SECTION("TranslateNativeError") {
    /// Test translating a known error code...
    /// On windows, this is ERROR_FILE_NOT_FOUND.
    /// On POSIX, this is ENOENT.
    ErrorCode error_code = 2;
    
    std::string error_msg = translate_native_error(error_code);
    REQUIRE(!error_msg.empty());
    
    ///  Check that the error message contains expected text
    const bool is_ok = error_msg.find("file") != std::string::npos ||
            error_msg.find("File") != std::string::npos;
    REQUIRE(is_ok);
  }

  SECTION("CommonErrorCodes") {
    /// Test a few common error codes...
    std::vector<ErrorCode> common_codes;
    
#ifdef N19_WIN32
    /// Common Windows error codes
    common_codes = {2, 3, 5, 32, 87, 183};
#else
    /// Common POSIX error codes
    common_codes = {1, 2, 3, 13, 14, 22};
#endif
    
    for (auto code : common_codes) {
      std::string error_msg = translate_native_error(code);
      REQUIRE(!error_msg.empty());
    }
  }
}
