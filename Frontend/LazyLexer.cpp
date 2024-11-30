/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Frontend/LazyLexer.hpp>
#include <Core/ResultMacros.hpp>
#include <filesystem>
#include <algorithm>

auto n19::LazyLexer::create(const FileRef& ref) -> Result<LazyLexer> {
  auto fsize = ref.size().OR_RETURN();
  auto buff  = std::make_unique<std::vector<char8_t>>(*fsize);
  auto view  = n19::as_writable_bytes(*buff);

  // Read file into the buffer
  ref.read_into(view).OR_RETURN();

  LazyLexer lxr;
  lxr.file_name_ = fs::absolute(*ref).string();
  lxr.src_ = std::move(buff);

  if(lxr.src_->size() > 3
    && static_cast<uint8_t>(lxr.src_->at(0)) == 0xEF
    && static_cast<uint8_t>(lxr.src_->at(1)) == 0xBB
    && static_cast<uint8_t>(lxr.src_->at(2)) == 0xBF
  ){
    lxr.index_ = 3;
  }

  return make_result<LazyLexer>(std::move(lxr));
}

