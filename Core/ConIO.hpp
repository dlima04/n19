/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef CONIO_HPP
#define CONIO_HPP
#include <Sys/IODevice.hpp>
#include <Core/ClassTraits.hpp>
BEGIN_NAMESPACE(n19);

#define N19_COSTREAM_BUFFER_SIZE 4096
#define N19_CISTREAM_BUFFER_SIZE 4096

// TODO: finish

class COStream {
N19_MAKE_NONCOPYABLE(COStream);
public:
  static auto from_stdout() -> Result<COStream>;
  static auto from_stderr() -> Result<COStream>;
protected:
  COStream() = default;
private:
  sys::IODevice fd_;
};

END_NAMESPACE(n19);
#endif //CONIO_HPP
