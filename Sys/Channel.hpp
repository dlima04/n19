/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <Sys/IODevice.hpp>
#include <Core/Concepts.hpp>
#include <Core/Try.hpp>
#include <utility>
BEGIN_NAMESPACE(n19::sys)

template<Concrete T>
class Channel {
  using ValueType      = T;
  using ReferenceType  = T&;
  using PointerType    = T*;

  static auto create() -> Result<Channel> {
    const auto ends = TRY(IODevice::create_pipe());
    Channel the_channel;
    the_channel.read_end_  = ends[ 0 ];
    the_channel.write_end_ = ends[ 1 ];
    return the_channel;
  }

 ~Channel() = default;
  Channel() = default;
protected:
  IODevice read_end_;
  IODevice write_end_;
};

END_NAMESPACE(n19::sys);
#endif //CHANNEL_HPP
