/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Bulwark.hpp>
#include <IO/Stream.hpp>
#include <Core/Bytes.hpp>
#include <string>
#include <vector>
using namespace n19;

TEST_CASE(BufferedOStream, BasicFunctionality) {
  SECTION(Initialization, {
    // Test default initialization
    auto stream = BufferedOStream<>::create_testable();
    REQUIRE(stream.buffer_max_size()  == N19_OSTREAM_BUFFSIZE);
    REQUIRE(stream.buffer_current()   == 0);
    REQUIRE(stream.buffer_remaining() == N19_OSTREAM_BUFFSIZE);
  });

  SECTION(WriteSmallData, {
    // Test writing small data that fits in the buffer
    auto stream = BufferedOStream<>::create_testable();
    std::string data = "Hello, World!";
    
    stream.write(n19::as_bytes(data));
    
    // Check buffer state
    REQUIRE(stream.buffer_current() == data.size());
    REQUIRE(stream.buffer_remaining() == N19_OSTREAM_BUFFSIZE - data.size());
    
    // Check buffer contents
    const auto* buffer_data = stream.buffer_data();
    
    for(size_t i = 0; i < stream.buffer_current(); ++i) {
      if(i >= data.size() || i >= stream.buffer_max_size()) { TEST_FATAL("Buffer overrun??"); }
      if((char)buffer_data[i] != data[i]) { TEST_FATAL("Buffer contents mismatch"); }
    }
  });

  SECTION(WriteLargeData, {
    // Test writing data larger than the buffer
    auto stream = BufferedOStream<>::create_testable();
    
    // Create a string larger than the buffer
    std::string large_data;
    large_data.reserve(N19_OSTREAM_BUFFSIZE + 100);
    for (size_t i = 0; i < (N19_OSTREAM_BUFFSIZE + 100); ++i) {
      large_data.push_back('A' + (i % 26));
    }
    
    stream.write(n19::as_bytes(large_data));
    
    // Since the data is larger than the buffer, it should be written directly
    // and the buffer should be empty
    REQUIRE(stream.buffer_current() == 0);
    REQUIRE(stream.buffer_remaining() == N19_OSTREAM_BUFFSIZE);
  });

  SECTION(FlushBuffer, {
    // Test flushing the buffer
    auto stream = BufferedOStream<>::create_testable();
    std::string data = "Flush test";
    
    stream.write(n19::as_bytes(data));
    REQUIRE(stream.buffer_current() == data.size());
    
    stream.flush();
    REQUIRE(stream.buffer_current() == 0);
    REQUIRE(stream.buffer_remaining() == N19_OSTREAM_BUFFSIZE);
  });

  SECTION(BufferOverflow, {
    // Test writing data that would overflow the buffer
    auto stream = BufferedOStream<>::create_testable();
    
    // Fill the buffer almost to capacity
    std::string partial_data;
    partial_data.reserve(N19_OSTREAM_BUFFSIZE - 4);
    for (size_t i = 0; i < (N19_OSTREAM_BUFFSIZE - 4); ++i) {
      partial_data.push_back('X');
    }
    
    stream.write(n19::as_bytes(partial_data));
    REQUIRE(stream.buffer_current() == partial_data.size());
    
    // Now write more data that would overflow
    std::string overflow_data = "Overflow";
    stream.write(n19::as_bytes(overflow_data));
    
    // The buffer should have been flushed and the new data written
    REQUIRE(stream.buffer_current() == overflow_data.size());
    REQUIRE(stream.buffer_remaining() == N19_OSTREAM_BUFFSIZE - overflow_data.size());
    
    // Check buffer contents
    const auto* buffer_data = stream.buffer_data();
    
    for(size_t i = 0; i < stream.buffer_current(); ++i) {
      if(i >= overflow_data.size() || i >= stream.buffer_max_size()) { TEST_FATAL("Buffer overrun??"); }
      if((char)buffer_data[i] != overflow_data[i]) { TEST_FATAL("Buffer contents mismatch"); }
    }
  });

  SECTION(OperatorOverloads, {
    // Test operator<< overloads
    auto stream = BufferedOStream<>::create_testable();
    
    // Test with different types
    stream
      << "String: "
      << 42
      << " "
      << 3.14
      << " "
      << '\n';
    
    // Check buffer contents
    std::string expected = "String: 42 3.14 \n";
    const auto* buffer_data = stream.buffer_data();
    
    for(size_t i = 0; i < stream.buffer_current(); ++i) {
      if(i >= expected.size() || i >= stream.buffer_max_size()) { TEST_FATAL("Buffer overrun??"); }
      if((char)buffer_data[i] != expected[i]) { TEST_FATAL("Buffer contents mismatch"); }
    }
  });

  SECTION(EmptyWrite, {
    // Test writing empty data
    auto stream = BufferedOStream<>::create_testable();
    
    // Write empty string
    stream.write(n19::as_bytes(std::string_view("")));
    
    // Buffer should remain unchanged
    REQUIRE(stream.buffer_current() == 0);
    REQUIRE(stream.buffer_remaining() == N19_OSTREAM_BUFFSIZE);
  });
}
