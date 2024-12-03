/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <print>
#include <iostream>
#include <cstdint>
#include <Frontend/Lexer.hpp>
#include <Core/FileRef.hpp>
#include <Core/ResultMacros.hpp>
#include <Native/LastError.hpp>
#include <Core/Panic.hpp>
#include <Core/ArgParse.hpp>
#include <Core/RingBuffer.hpp>
#include <Core/RingQueue.hpp>
#include <thread>

#define CURRENT_TEST "/Users/Diago/Desktop/compiler_tests/test2.txt"
using namespace n19;

struct message {
  char buff[40] = { 0 };
  uint16_t ival = 0;
};

static void start_consuming_rb(RingBuffer<message, 8>& buff) {
  std::this_thread::sleep_for(std::chrono::seconds(20));
  while(true) {
    auto msg = buff.read();
    if(!msg) {
      continue;
    }

    msg->buff[39] = '\0';
    std::cout << "CONSUMER: Got message: " << msg->buff << "\n";
    std::cout << "CONSUMER: Got ival: " << msg->ival << std::endl;

    if(msg->ival == 6969) {
      std::cout << "CONSUMER: ival was 6969. Stopping now." << std::endl;
      break;
    }
  }
}

static void test_ringbuffer() {
  RingBuffer<message, 8> buff;
  std::thread t(start_consuming_rb, std::ref(buff));

  message m;
  while(true) {
    std::memset(&m, 0, sizeof(m));
    std::cout << "input> ";
    std::cin >> m.buff >> m.ival;

    if(!buff.write(m)) {
      std::cerr << "COULDNT WRITE TO BUFF!!\n";
    }
    if(m.ival == 6969) {
      break;
    }
  }

  std::cout << "joining t...\n";
  t.join();
  std::cout << "joined." << std::endl;
}

static void start_consuming_rq(RingQueue<message, 8>& buff) {
#if 1
  std::this_thread::sleep_for(std::chrono::seconds(20));
  while(true) {
    auto msg = buff.dequeue();

    msg.buff[39] = '\0';
    std::cout << "CONSUMER: Got message: " << msg.buff << "\n";
    std::cout << "CONSUMER: Got ival: " << msg.ival << std::endl;

    if(msg.ival == 6969) {
      std::cout << "CONSUMER: ival was 6969. Stopping now." << std::endl;
      break;
    }
  }
#else
  while(true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    auto msg = buff.try_dequeue();
    if(!msg) {
      continue;
    }

    msg->buff[39] = '\0';
    std::cout << "CONSUMER: Got message: " << msg->buff << "\n";
    std::cout << "CONSUMER: Got ival: " << msg->ival << std::endl;

    if(msg->ival == 6969) {
      std::cout << "CONSUMER: ival was 6969. Stopping now." << std::endl;
      break;
    }
  }
#endif
}

static void test_ringqueue() {
  RingQueue<message, 8> buff;
  std::thread t(start_consuming_rq, std::ref(buff));

  message m;
  while(true) {
    std::memset(&m, 0, sizeof(m));
    std::cout << "input> ";
    std::cin >> m.buff >> m.ival;

    buff.enqueue(m);
    if(m.ival == 6969) {
      break;
    }
  }

  std::cout << "joining t...\n";
  t.join();
  std::cout << "joined." << std::endl;
}

int main(int argc, char** argv) {

  //
  // std::vector<native::StringView> strs
  // = { "--output-directory", "--demangle-funcs=true", "-a", "3123", "-z" };
  //
  // argp::Parser parser;
  // parser
  //   .add_param(argp::Parameter::create("--output-directory", "-f", "The foo argument" ))
  //   .add_param(argp::Parameter::create("--demangle-funcs", "-b", "The bar argument", false, argp::Value{"thedefault"}))
  //   .add_param(argp::Parameter::create("--time-actions", "-a", "The asshole argument", true))
  //   .add_param(argp::Parameter::create("--jobs", "-z", "idk lol", true));
  //
  // if(!parser.parse(strs)) {
  //    return 1;
  // }
  //
  // parser.debug_print();
  // parser.print();
  //
  // const auto val = parser.get_arg("--time-actions");
  // if(val) {
  //   std::println("Value: \"{}\"", val->value_);
  // }

  try {
    const auto file = MUST(FileRef::open(CURRENT_TEST));
    auto lxr = Lexer::create(*file);
    if(!lxr) {
      return 1;
    }

    lxr->get()->testfuck();

  } catch(const std::exception& e) {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
  }

  return 0;
}


