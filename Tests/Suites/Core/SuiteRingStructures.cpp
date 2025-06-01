/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Bulwark.hpp>
#include <Core/RingBuffer.hpp>
#include <Core/RingQueue.hpp>
#include <thread>
#include <chrono>
using namespace n19;

TEST_CASE(RingBuffer, BasicFunctionality) {
  SECTION(WriteAndRead, {
    RingBuffer<int, 4> buffer;
    REQUIRE(buffer.is_empty());
    REQUIRE(!buffer.is_full());
    
    REQUIRE(buffer.write(1));
    REQUIRE(!buffer.is_empty());
    REQUIRE(!buffer.is_full());
    
    auto val = buffer.read();
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 1);
    REQUIRE(buffer.is_empty());
  });

  SECTION(Overwrite, {
    RingBuffer<int, 4> buffer;
    /// Fill the buffer
    REQUIRE(buffer.write(1));
    REQUIRE(buffer.write(2));
    REQUIRE(buffer.write(3));
    REQUIRE(buffer.is_full());
    
    /// Overwrite should succeed even when full
    buffer.overwrite(5);
    REQUIRE(buffer.is_full());
    
    /// First value should be overwritten
    auto val = buffer.read();
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 2);
  });

  SECTION(CurrentAndTryCurrent, {
    RingBuffer<int, 4> buffer;
    REQUIRE(buffer.try_current().has_value() == false);
    
    REQUIRE(buffer.write(1));
    auto val = buffer.try_current();
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 1);
    
    /// Reading shouldn't affect current
    buffer.read();
    REQUIRE(buffer.try_current().has_value() == false);
  });
}

TEST_CASE(RingBuffer, EdgeCases) {
  SECTION(EmptyBuffer, {
    RingBuffer<int, 4> buffer;
    REQUIRE(buffer.is_empty());
    REQUIRE(!buffer.is_full());
    REQUIRE(buffer.read().has_value() == false);
  });

  SECTION(FullBuffer, {
    RingBuffer<int, 4> buffer;
    /// Fill the buffer
    REQUIRE(buffer.write(1));
    REQUIRE(buffer.write(2));
    REQUIRE(buffer.write(3));   /// NOTE: 3/4 elements is a full buffer because of the
    REQUIRE(buffer.is_full());  /// semantics wrt the indexes.
    REQUIRE(!buffer.write(5));  /// Should fail when full
  });

  SECTION(WrapAround, {
    RingBuffer<int, 4> buffer;
    /// Fill and empty the buffer to cause wrap-around
    REQUIRE(buffer.write(1));
    REQUIRE(buffer.write(2));
    REQUIRE(buffer.write(3));
    buffer.read();
    buffer.read();
    buffer.read();
    REQUIRE(buffer.is_empty());
    
    /// Should be able to write again after wrap-around
    REQUIRE(buffer.write(5));
    auto val = buffer.read();
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 5);
  });
}

TEST_CASE(RingQueue, BasicFunctionality) {
  SECTION(EnqueueAndDequeue, {
    RingQueue<int, 4> queue;
    REQUIRE(queue.is_empty());
    REQUIRE(!queue.is_full());
    
    queue.enqueue(1);
    REQUIRE(!queue.is_empty());
    REQUIRE(!queue.is_full());
    
    auto val = queue.dequeue();
    REQUIRE(val == 1);
    REQUIRE(queue.is_empty());
  });

  SECTION(TryEnqueueAndTryDequeue, {
    RingQueue<int, 4> queue;
    REQUIRE(queue.try_enqueue(1));
    REQUIRE(!queue.is_empty());
    
    auto val = queue.try_dequeue();
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 1);
    REQUIRE(queue.is_empty());
  });

  SECTION(CurrentAndTryCurrent, {
    RingQueue<int, 4> queue;
    REQUIRE(!queue.try_current().has_value());
    
    queue.enqueue(1);
    auto val = queue.try_current();
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 1);
    
    /// Reading shouldn't affect current
    queue.dequeue();
    REQUIRE(!queue.try_current().has_value());
  });
}

TEST_CASE(RingQueue, Peeking) {
  SECTION(BasicPeek, {
    RingQueue<int, 4> queue;
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    
    REQUIRE(queue.can_peek(0));
    REQUIRE(queue.can_peek(1));
    REQUIRE(queue.can_peek(2));
    REQUIRE(!queue.can_peek(3));

    auto val = queue.peek(1);
    REQUIRE(val == 2);
  });

  SECTION(TryPeek, {
    RingQueue<int, 4> queue;
    queue.enqueue(1);
    queue.enqueue(2);

    auto val = queue.try_peek(1);
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 2);

    val = queue.try_peek(2);
    REQUIRE(!val.has_value());
  });

  SECTION(PeekWrapAround, {
    RingQueue<int, 4> queue;
    /// Fill and partially empty to cause wrap-around
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    auto dq1 = queue.dequeue(); // Remove 1
    auto dq2 = queue.dequeue(); // Remove 2
    queue.enqueue(4);

    REQUIRE(dq1 == 1);
    REQUIRE(dq2 == 2);
    REQUIRE(queue.can_peek(0));
    REQUIRE(queue.can_peek(1));
    REQUIRE(!queue.can_peek(2));
    
    auto val = queue.peek(1);
    REQUIRE(val == 4);
  });
}

TEST_CASE(RingQueue, BlockingBehavior) {
  SECTION(BlockingDequeue, {
    RingQueue<int, 4> queue;
    
    /// Start a thread to enqueue after a delay
    std::thread producer([&queue]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      queue.enqueue(1);
    });
    
    /// This should block until the producer enqueues
    auto val = queue.dequeue();
    producer.join();
    
    REQUIRE(val == 1);
  });

  SECTION(BlockingPeek, {
    RingQueue<int, 4> queue;
    
    /// Start a thread to enqueue after a delay
    std::thread producer([&queue]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      queue.enqueue(1);
      queue.enqueue(2);
    });
    
    /// This should block until the producer enqueues enough elements
    auto val = queue.peek(1);
    producer.join();
    
    REQUIRE(val == 2);
  });
}
