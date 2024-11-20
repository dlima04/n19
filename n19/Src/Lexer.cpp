#include <Lexer.h>
#include <Defer.h>
#include <ConManip.h>
#include <Panic.h>
#include <ErrorCollector.h>
#include <ResultMacros.h>
#include <filesystem>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

auto n19::Lexer::create(const FileRef& file) -> Result<Lexer> {
  auto fsize = TRY(file.size());
  auto ptr   = TRY(file.get_shared(*fsize));

  Lexer lxr;
  lxr.file_name_ = file.absolute();
  lxr.src_ = std::move(*ptr);

  if(lxr.src_->size() > 3
    && static_cast<uint8_t>(lxr.src_->at(0)) == 0xEF
    && static_cast<uint8_t>(lxr.src_->at(1)) == 0xBB
    && static_cast<uint8_t>(lxr.src_->at(2)) == 0xBF
  ){
    lxr.index_ = 3;
  }

  // Wrap the lexer in a Result
  return make_result<Lexer>(lxr);
}

auto n19::Lexer::create(const std::string& file) -> Result<Lexer> {
  const auto ref = TRY(FileRef::create(file));
  return create(*ref);
}

auto n19::Lexer::advance(const uint32_t amnt) -> Lexer& {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return *this;
  }
  for(uint32_t i = 0; i < amnt; i++) {
    do {
      _advance_impl();
    } while(curr_tok_.type_ == TokenType::None);
  }

  curr_tok_.line_ = line_;
  return *this;
}

auto n19::Lexer::peek(const uint32_t amnt) -> Token {
  if(curr_tok_ == TokenType::None) {
    advance(1);
  }

  const auto line_tmp   = line_;
  const auto index_tmp  = index_;
  const auto token_tmp  = curr_tok_;

  advance(amnt);
  const auto peeked = curr_tok_;

  index_    = index_tmp;
  line_     = line_tmp;
  curr_tok_ = token_tmp;
  return peeked;
}

auto n19::Lexer::expect(
  const TokenType type,
  const std::string& msg,
  const bool adv_after ) -> Result<None>
{
  if(curr_tok_.type_ != type) {
    ErrorCollector::display_error(
      msg,
      file_name_,
      *src_,
      curr_tok_.pos_,
      curr_tok_.line_,
      false);
    return make_error(
      ErrC::BadToken,
      "Unexpected token {}, expected {}.",
      curr_tok_.type_.to_string(),
      type.to_string());
  }

  if(adv_after) advance(1);
  return make_result<None>();
}

auto n19::Lexer::expect(
  const TokenCategory cat,
  const std::string& msg,
  const bool adv_after ) -> Result<None>
{
  if(curr_tok_.cat_ != cat) {
    ErrorCollector::display_error(
      msg,
      file_name_,
      *src_,
      curr_tok_.pos_,
      curr_tok_.line_,
      false);
    return make_error(
      ErrC::BadToken,
      "Unexpected token of category {}, "
      "Expected a token of category {} instead.",
      curr_tok_.cat_.to_string(),
      cat.to_string());
  }

  if(adv_after) advance(1);
  return make_result<None>();
}

inline auto n19::Lexer::_at_utf8_begin() const -> bool {
  return static_cast<uint8_t>(_current_char()) >= 0x80;
}

inline auto n19::Lexer::_advance_char(const uint32_t amnt) -> void {
  if(curr_tok_.type_ != TokenType::EndOfFile && index_ < src_->size()) {
    index_ += amnt;
  }
}

inline auto n19::Lexer::_current_char() const -> char {
  return index_ >= src_->size()
    ? '\0'
    : src_->at(index_);
}

inline auto n19::Lexer::_peek_char(const uint32_t amnt) const -> char {
  return (index_ + amnt) >= src_->size()
    ? '\0'
    : src_->at(index_ + amnt);
}

inline auto n19::Lexer::_advance_line() -> void {
  if(index_ < src_->size()) ++line_;
}

auto n19::Lexer::current() const -> const Token& {
  return curr_tok_;
}

inline auto n19::Lexer::_skip_utf8_sequence() -> void {
  ASSERT(_at_utf8_begin());
  const auto c         = static_cast<uint8_t>(_current_char());
  const auto curr_pos  = index_;
  bool invalid         = false;

  if      ((c & 0xE0) == 0xC0) _advance_char(2);
  else if ((c & 0xF0) == 0xE0) _advance_char(3);
  else if ((c & 0xF8) == 0xF0) _advance_char(4);
  else invalid = true;

  if(invalid || index_ - 1 >= src_->size()) {
    FATAL(fmt("Invalid UTF-8 sequence at file position {}.", curr_pos));
  }
}

inline auto n19::Lexer::_token_hex_literal() -> Token {
  ASSERT(_current_char() == '0');
  ASSERT(_peek_char() == 'x' || _peek_char() == 'X');

  const size_t start = index_;
  _advance_char(2);
  while(std::isxdigit(static_cast<uint8_t>(_current_char()))) {
    _advance_char(1);
  }

  const std::string_view token_raw = {&src_->at(start), index_ - start};
  if(!std::isxdigit(static_cast<uint8_t>(token_raw.back()))) {
    return Token::illegal(start, line_, token_raw);
  }

  return {
    start,
    line_,
    TokenType::HexLiteral,
    TokenCategory::Literal,
    token_raw
  };
}

inline auto n19::Lexer::_token_octal_literal() -> Token {
  ASSERT(_current_char() == '0');
  ASSERT(std::isdigit(static_cast<uint8_t>(_peek_char())));

  const size_t start = index_;
  _advance_char(1);
  auto is_octal_digit = [](const char c) -> bool {
    return c >= '0' && c <= '7';
  };

  while(true) {
    if(!std::isdigit(static_cast<uint8_t>(_current_char()))) {
      break;
    } if(!is_octal_digit(_current_char())) {
      return Token::illegal(start, line_, {&src_->at(start), index_ - start});
    }
    _advance_char(1);
  }

  return {
    start,
    line_,
    TokenType::OctalLiteral,
    TokenCategory::Literal,
    {&src_->at(start), index_ - start}
  };
}

inline auto n19::Lexer::_token_numeric_literal() -> Token {
  ASSERT(std::isdigit(static_cast<uint8_t>(_current_char())));

  const size_t start = index_;
  bool seen_exponent = false;
  bool seen_dot      = false;

  //
  // We can pretend that this is a float literal
  // until we are done parsing it. Once we're done,
  // we can just check to see if there are any non-digit
  // characters in the string to figure out if it's actually an int.
  //

  while(true) {
    if(_current_char() == '\0') {
      break;
    }

    if(_current_char() == '.') {
      if(_peek_char() == '.' || !std::isdigit(static_cast<uint8_t>(_peek_char()))) {
        break;
      } if(seen_dot || seen_exponent) {
        return Token::illegal(start, line_, {&src_->at(start), index_ - start});
      }

      seen_dot = true;
    }

    else if(_current_char() == 'e' || _current_char() == 'E') {
      if(seen_exponent) {
        return Token::illegal(start, line_, {&src_->at(start), index_ - start});
      }

      seen_exponent = true;
      if(_peek_char() == '-' || _peek_char() == '+') {
        _advance_char(1);
        if(!std::isdigit(static_cast<uint8_t>(_peek_char()))) break;
      }
    }

    else if(!std::isdigit(static_cast<uint8_t>(_current_char()))) {
      break;
    }

    _advance_char(1);
  }

  //
  // Get string representation, current index - start.
  // Then check for dots or other related float characters.
  //

  const std::string_view token_raw = {&src_->at(start), index_ - start};
  if(!std::isdigit(static_cast<uint8_t>(token_raw.back()))) {
    return Token::illegal(start, line_, token_raw);
  }

  if(std::ranges::find_if(token_raw, [](const char c) {
    return !std::isdigit(static_cast<uint8_t>(c));
  }) == token_raw.end()) {
    return {
      start,
      line_,
      TokenType::IntLiteral,
      TokenCategory::Literal,
      token_raw
    };
  }

  return {
   start,
    line_,
    TokenType::FloatLiteral,
    TokenCategory::Literal,
    token_raw
  };
}

inline auto n19::Lexer::_token_ambiguous() -> void {
  ASSERT(index_ < src_->size());
  ASSERT(!_curr_char_is_reserved());
  using KeywordMap = std::unordered_map<std::string, TokenType>;
  const auto start = index_;

  //
  // Key value pairs for easy keyword lookup.
  //

  static KeywordMap keywords = {
    {"return", TokenType::Return},
    {"break", TokenType::Break},
    {"continue", TokenType::Continue},
    {"for", TokenType::For},
    {"while", TokenType::While},
    {"do", TokenType::Do},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"struct", TokenType::Struct},
    {"switch", TokenType::Switch},
    {"case", TokenType::Case},
    {"default", TokenType::Default},
    {"block", TokenType::Block},
    {"defer", TokenType::Defer},
    {"defer_if", TokenType::DeferIf},
    {"sizeof", TokenType::Sizeof},
    {"typeof", TokenType::Typeof},
    {"fallthrough", TokenType::Fallthrough},
    {"namespace", TokenType::Namespace},
    {"where", TokenType::Where},
    {"otherwise", TokenType::Otherwise},
    {"proc", TokenType::Proc},
    {"let", TokenType::Let},
    {"const", TokenType::Const},
    {"as", TokenType::As},
    {"with", TokenType::With},
  };

  static KeywordMap keyword_literals = {
    {"null", TokenType::NullLiteral},
    {"true", TokenType::BooleanLiteral},
    {"false", TokenType::BooleanLiteral},
  };

  // Check for a numeric literal.
  if(std::isdigit(static_cast<uint8_t>(_current_char()))) {
    if(_current_char() == '0' && (_peek_char() == 'x' || _peek_char() == 'X')) {
      curr_tok_ = _token_hex_literal();
    } else if(_current_char() == '0' && std::isdigit(static_cast<uint8_t>(_peek_char()))) {
      curr_tok_ = _token_octal_literal();
    } else {
      curr_tok_ = _token_numeric_literal();
    }
    return;
  }

  // Get to the end of the identifier.
  while(!_curr_char_is_reserved() && _current_char() != '\0') {
    if(_at_utf8_begin()) {
      _skip_utf8_sequence();
    } else {
      _advance_char(1);
    }
  }

  const std::string_view token_raw = {&src_->at(start), index_ - start};
  const std::string temp           = {token_raw.begin(), token_raw.end()};

  curr_tok_.value_ = token_raw;
  curr_tok_.pos_   = start;
  curr_tok_.line_  = line_;

  if(keywords.contains(temp)) {
    curr_tok_.type_  = keywords[temp];
    curr_tok_.cat_   = TokenCategory::Keyword;
  } else if(keyword_literals.contains(temp)) {
    curr_tok_.type_  = keyword_literals[temp];
    curr_tok_.cat_   = TokenCategory::Literal;
  } else { // Identifier
    curr_tok_.type_  = TokenType::Identifier;
    curr_tok_.cat_   = TokenCategory::Identifier;
  }

  if(curr_tok_.type_ == TokenType::Sizeof || curr_tok_.type_ == TokenType::Typeof) {
    curr_tok_.cat_ |= TokenCategory::ValidPrefix | TokenCategory::UnaryOp;
  } else if(curr_tok_.type_ == TokenType::As || curr_tok_.type_ == TokenType::With) {
    curr_tok_.cat_ |= TokenCategory::ValidPostfix | TokenCategory::UnaryOp;
  }
}

inline auto n19::Lexer::_token_skip() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
  } else {
    curr_tok_.type_ = TokenType::None;
    curr_tok_.cat_  = TokenCategory::NonCategorical;
    _advance_char(1);
  }
}

inline auto n19::Lexer::_token_newline() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
  } else {
    curr_tok_.type_ = TokenType::None;
    curr_tok_.cat_  = TokenCategory::NonCategorical;
    _advance_line();
    _advance_char(1);
  }
}

inline auto n19::Lexer::_token_null() -> void {
  curr_tok_ = Token::eof(src_->size() - 1, line_);
}

inline auto n19::Lexer::_token_single_char(
  const TokenType type,
  const TokenCategory cat ) -> void
{
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }
  curr_tok_.pos_   = index_;
  curr_tok_.line_  = line_;
  curr_tok_.type_  = type;
  curr_tok_.cat_   = cat;
  curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
  _advance_char(1);
}

inline auto n19::Lexer::_token_plus() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.pos_ = index_;
  switch(_peek_char()) {
    case '=': // '+='
      curr_tok_.type_  = TokenType::PlusEq;
      curr_tok_.cat_   = TokenCategory::BinaryOp
        | TokenCategory::ArithAssignOp
        | TokenCategory::ArithmeticOp
        | TokenCategory::PointerArithOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
      break;
    case '+': // '++'
      curr_tok_.type_  = TokenType::Inc;
      curr_tok_.cat_   = TokenCategory::UnaryOp
        | TokenCategory::ArithAssignOp
        | TokenCategory::ArithmeticOp
        | TokenCategory::PointerArithOp
        | TokenCategory::ValidPostfix
        | TokenCategory::ValidPrefix;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
      break;
    default: // '+'
      curr_tok_.type_  = TokenType::Plus;
      curr_tok_.cat_   = TokenCategory::BinaryOp
        | TokenCategory::UnaryOp
        | TokenCategory::ArithmeticOp
        | TokenCategory::PointerArithOp
        | TokenCategory::ValidPrefix;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
      _advance_char(1);
      break;
  }
}

inline auto n19::Lexer::_token_tilde() -> void {
  _token_single_char(
    TokenType::BitwiseNot,
    TokenCategory::UnaryOp
      | TokenCategory::BitwiseOp
      | TokenCategory::ValidPrefix
  );
}

inline auto n19::Lexer::_token_at() -> void {
  _token_single_char(TokenType::At,
    TokenCategory::NonCategorical
  );
}

inline auto n19::Lexer::_token_money() -> void {
  _token_single_char(TokenType::Money,
    TokenCategory::NonCategorical
  );
}

inline auto n19::Lexer::_token_lsqbracket() -> void {
  _token_single_char(TokenType::LeftSqBracket,
    TokenCategory::Punctuator | TokenCategory::ValidPostfix
  );
}

inline auto n19::Lexer::_token_rsqbracket() -> void {
  _token_single_char(TokenType::RightSqBracket,
    TokenCategory::Punctuator
  );
}

inline auto n19::Lexer::_token_semicolon() -> void {
  _token_single_char(TokenType::Semicolon,
    TokenCategory::Punctuator | TokenCategory::Terminator);
}

inline auto n19::Lexer::_token_lparen() -> void {
  _token_single_char(TokenType::LeftParen,
    TokenCategory::Punctuator | TokenCategory::ValidPostfix);
}

inline auto n19::Lexer::_token_comma() -> void {
  _token_single_char(TokenType::Comma,
    TokenCategory::Punctuator | TokenCategory::Terminator
  );
}

inline auto n19::Lexer::_token_rparen() -> void {
  _token_single_char(TokenType::RightParen,
    TokenCategory::Punctuator
  );
}

inline auto n19::Lexer::_token_lbrace() -> void {
  _token_single_char(TokenType::LeftBrace,
    TokenCategory::Punctuator
  );
}

inline auto n19::Lexer::_token_rbrace() -> void {
  _token_single_char(TokenType::RightBrace,
    TokenCategory::Punctuator
  );
}

inline auto n19::Lexer::_token_illegal() -> void {
  _token_single_char(TokenType::Illegal,
    TokenCategory::NonCategorical
  );
}

inline auto n19::Lexer::_token_asterisk() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.pos_ = index_;
  if(_peek_char() == '=') { // '*='
    curr_tok_.type_  = TokenType::MulEq;
    curr_tok_.cat_   = TokenCategory::BinaryOp
      | TokenCategory::ArithAssignOp
      | TokenCategory::ArithmeticOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
    _advance_char(2);
  } else { // '*'
    curr_tok_.type_  = TokenType::Mul;
    curr_tok_.cat_   = TokenCategory::BinaryOp
      | TokenCategory::UnaryOp
      | TokenCategory::ArithmeticOp
      | TokenCategory::ValidPrefix;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
    _advance_char(1);
  }
}

inline auto n19::Lexer::_token_colon() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.pos_  = index_;
  if(_peek_char() == ':') { // '::'
    curr_tok_.type_  = TokenType::NamespaceOperator;
    curr_tok_.cat_   = TokenCategory::ValidPrefix;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
    _advance_char(2);
  } else { // ':'
    curr_tok_.type_  = TokenType::TypeAssignment;
    curr_tok_.cat_   = TokenCategory::NonCategorical;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
    _advance_char(1);
  }
}

inline auto n19::Lexer::_token_bang() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
  } else if(_peek_char() == '=') { // '!='
    curr_tok_.pos_   = index_;
    curr_tok_.type_  = TokenType::Neq;
    curr_tok_.cat_   = TokenCategory::BinaryOp
      | TokenCategory::LogicalOp
      | TokenCategory::ComparisonOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
    _advance_char(2);
  } else { // '!'
    curr_tok_.pos_   = index_;
    curr_tok_.type_  = TokenType::LogicalNot;
    curr_tok_.cat_   = TokenCategory::UnaryOp
      | TokenCategory::LogicalOp
      | TokenCategory::ValidPrefix;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
    _advance_char(1);
  }
}

inline auto n19::Lexer::_token_percent() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.cat_ = TokenCategory::BinaryOp | TokenCategory::ArithmeticOp;
  curr_tok_.pos_ = index_;

  if(_peek_char() == '=') { // '%='
    curr_tok_.type_  = TokenType::ModEq;
    curr_tok_.cat_  |= TokenCategory::ArithAssignOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
    _advance_char(2);
  } else { // '%'
    curr_tok_.type_  = TokenType::Mod;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
    _advance_char(1);
  }
}

inline auto n19::Lexer::_token_uparrow() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.pos_ = index_;
  curr_tok_.cat_ = TokenCategory::BinaryOp | TokenCategory::BitwiseOp;

  if(_peek_char() == '=') { // '^='
    curr_tok_.type_  = TokenType::XorEq;
    curr_tok_.cat_  |= TokenCategory::BitwiseAssignOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
    _advance_char(2);
  } else { // '^'
    curr_tok_.type_  = TokenType::Xor;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
    _advance_char(1);
  }
}

inline auto n19::Lexer::_token_verticalline() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.pos_ = index_;
  curr_tok_.cat_ = TokenCategory::BinaryOp;

  switch(_peek_char()) {
    case '|': // '||'
      curr_tok_.type_  = TokenType::LogicalOr;
      curr_tok_.cat_  |= TokenCategory::LogicalOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
      break;
    case '=': // '|='
      curr_tok_.type_  = TokenType::BitwiseOrEq;
      curr_tok_.cat_  |= TokenCategory::BitwiseOp | TokenCategory::BitwiseAssignOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
      break;
    default: // '|'
      curr_tok_.type_  = TokenType::BitwiseOr;
      curr_tok_.cat_   = TokenCategory::BitwiseOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
      _advance_char(1);
      break;
  }
}

inline auto n19::Lexer::_token_lessthan() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.cat_ = TokenCategory::BinaryOp;
  curr_tok_.pos_ = index_;

  if(_peek_char(1) == '=') { // '<='
    curr_tok_.type_  = TokenType::Lte;
    curr_tok_.cat_  |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
    _advance_char(2);
  } else if(_peek_char(1) == '<' && _peek_char(2) == '=') { // <<=
    curr_tok_.type_  = TokenType::LshiftEq;
    curr_tok_.cat_  |= TokenCategory::BitwiseOp | TokenCategory::BitwiseAssignOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 3);
    _advance_char(3);
  } else if(_peek_char(1) == '<') { // '<<'
    curr_tok_.type_  = TokenType::Lshift;
    curr_tok_.cat_  |= TokenCategory::BitwiseOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
    _advance_char(2);
  } else { // '<'
    curr_tok_.type_  = TokenType::Lt;
    curr_tok_.cat_  |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
    _advance_char(1);
  }
}

inline auto n19::Lexer::_token_greaterthan() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.cat_ = TokenCategory::BinaryOp;
  curr_tok_.pos_ = index_;

  if(_peek_char(1) == '=') { // '>='
    curr_tok_.type_  = TokenType::Gte;
    curr_tok_.cat_  |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
    _advance_char(2);
  } else if(_peek_char(1) == '>' && _peek_char(2) == '=') { // '>>='
    curr_tok_.type_  = TokenType::RshiftEq;
    curr_tok_.cat_  |= TokenCategory::BitwiseOp | TokenCategory::BitwiseAssignOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 3);
    _advance_char(3);
  } else if(_peek_char(1) == '>') { // '>>'
    curr_tok_.type_  = TokenType::Rshift;
    curr_tok_.cat_  |= TokenCategory::BitwiseOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
    _advance_char(2);
  } else { // '>'
    curr_tok_.type_  = TokenType::Gt;
    curr_tok_.cat_  |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
    _advance_char(1);
  }
}

inline auto n19::Lexer::_token_dot() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.cat_  = TokenCategory::NonCategorical;
  curr_tok_.pos_  = index_;

  if(_peek_char(1) == '.' && _peek_char(2) == '.') {
    curr_tok_.type_  = TokenType::DotThree;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 3);
    _advance_char(3);
  } else if(_peek_char(1) == '.') {
    curr_tok_.type_  = TokenType::DotTwo;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
    _advance_char(2);
  } else {
    curr_tok_.type_  = TokenType::Dot;
    curr_tok_.cat_   = TokenCategory::ValidPostfix | TokenCategory::BinaryOp;
    curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
    _advance_char(1);
  }
}

inline auto n19::Lexer::_token_equals() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.pos_ = index_;
  switch(_peek_char()) {
    case '=': // '=='
      curr_tok_.type_  = TokenType::Eq;
      curr_tok_.cat_   = TokenCategory::BinaryOp
        | TokenCategory::LogicalOp
        | TokenCategory::ComparisonOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
      break;
    case '>': // '=>'
      curr_tok_.type_  = TokenType::FatArrow;
      curr_tok_.cat_   = TokenCategory::NonCategorical;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
      break;
    default: // '='
      curr_tok_.type_  = TokenType::ValueAssignment;
      curr_tok_.cat_   = TokenCategory::BinaryOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
      _advance_char(1);
      break;
  }
}

inline auto n19::Lexer::_token_ampersand() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.pos_ = index_;
  curr_tok_.cat_ = TokenCategory::BinaryOp;

  switch(_peek_char()) {
    case '=': // '&='
      curr_tok_.type_  = TokenType::BitwiseAndEq;
      curr_tok_.cat_  |= TokenCategory::BitwiseAssignOp
        | TokenCategory::BitwiseOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
      break;
    case '&': // '&&'
      curr_tok_.type_  = TokenType::LogicalAnd;
      curr_tok_.cat_  |= TokenCategory::LogicalOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
      break;
    default: // '&'
      curr_tok_.type_  = TokenType::BitwiseAnd;
      curr_tok_.cat_  |= TokenCategory::BitwiseOp
        | TokenCategory::UnaryOp
        | TokenCategory::ValidPrefix;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
      _advance_char(1);
      break;
  }
}

inline auto n19::Lexer::_token_fwdslash() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  switch(_peek_char()) {
    case '/': // Single-line comment.
      while(_current_char() != '\n' && _current_char() != '\0') {
        if(_at_utf8_begin()) {
          _skip_utf8_sequence();
        } else {
          _advance_char(1);
        }
      }

      if(_current_char() == '\0') {
        curr_tok_ = Token::eof(src_->size() - 1, line_);
      } else {
        curr_tok_.pos_  = index_;
        curr_tok_.type_ = TokenType::None;
        curr_tok_.cat_  = TokenCategory::NonCategorical;
      }
      break;

    case '*': // Multi-line comment.
      while(true) {
        if(_current_char() == '\0') {
          break;
        } if(_current_char() == '*' && _peek_char() == '/') {
          _advance_char(2);
          break;
        } if(_current_char() == '\n') {
          _advance_line();
        } if(_at_utf8_begin()) {
          _skip_utf8_sequence();
        } else {
          _advance_char(1);
        }
      }

      if(_current_char() == '\0') {
        curr_tok_ = Token::eof(src_->size() - 1, line_);
      } else {
        curr_tok_.pos_  = index_;
        curr_tok_.type_ = TokenType::None;
        curr_tok_.cat_  = TokenCategory::NonCategorical;
      }
      break;

    case '=': // '/='
      curr_tok_.pos_   = index_;
      curr_tok_.type_  = TokenType::DivEq;
      curr_tok_.cat_   = TokenCategory::BinaryOp
        | TokenCategory::ArithmeticOp
        | TokenCategory::ArithAssignOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
      break;

    default: // '/'
      curr_tok_.pos_   = index_;
      curr_tok_.type_  = TokenType::Div;
      curr_tok_.cat_   = TokenCategory::BinaryOp
        | TokenCategory::ArithmeticOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
      _advance_char(1);
      break;
  }
}

inline auto n19::Lexer::_token_singlequote() -> void {
  // Singlequotes indicate a character or byte literal.
  // A character literal is defined as being an
  // 8 bit ASCII value, because the type of a char literal
  // is I8. For this reason we cannot allow Unicode characters
  // inside of singlequotes. These are multi-byte character sequences,
  // and must be represented by a string literal instead.

  const size_t start = index_;
  if(start >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  //
  // Check what's INSIDE the quotes first.
  //

  _advance_char(1);
  if(_current_char() == '\\') {
    _advance_char(1);
    if(_at_utf8_begin()) {
      curr_tok_ = Token::illegal(start, line_, {&src_->at(start), index_ - start});
      return;
    }
    _advance_char(1);
  } else if(_current_char() == '\'') {
    _advance_char(1);
    curr_tok_.type_  = TokenType::ByteLiteral;
    curr_tok_.cat_   = TokenCategory::Literal;
    curr_tok_.pos_   = start;
    curr_tok_.value_ = std::string_view(&src_->at(start), index_ - start);
    return;
  } else if(_at_utf8_begin() || _current_char() == '\n') {
    curr_tok_.type_  = TokenType::Illegal;
    curr_tok_.cat_   = TokenCategory::NonCategorical;
    curr_tok_.pos_   = start;
    curr_tok_.value_ = std::string_view(&src_->at(start), index_ - start);
    return;
  } else {
    _advance_char(1);
  }

  //
  // Ensure the following character is a closing quote, token == illegal otherwise.
  //

  if(_current_char() == '\0') {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
  } else if(_current_char() == '\'') {
    _advance_char(1);
    curr_tok_.type_  = TokenType::ByteLiteral;
    curr_tok_.cat_   = TokenCategory::Literal;
    curr_tok_.pos_   = start;
    curr_tok_.value_ = std::string_view(&src_->at(start), index_ - start);
  } else {
    curr_tok_.type_  = TokenType::Illegal;
    curr_tok_.cat_   = TokenCategory::NonCategorical;
    curr_tok_.pos_   = start;
    curr_tok_.value_ = std::string_view(&src_->at(start), index_ - start);
  }
}

inline auto n19::Lexer::_token_quote() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  const auto opening_quote = _current_char();
  const auto string_start  = index_;

  _advance_char(1);
  while(true) {
    if(_current_char() == '\0' || _current_char() == '\n') {
      curr_tok_.type_  = TokenType::Illegal;
      curr_tok_.cat_   = TokenCategory::NonCategorical;
      curr_tok_.pos_   = string_start;
      curr_tok_.value_ = std::string_view("\\0");
      break;
    } if(_current_char() == opening_quote) {
      _advance_char(1);
      curr_tok_.type_  = TokenType::StringLiteral;
      curr_tok_.cat_   = TokenCategory::Literal;
      curr_tok_.pos_   = string_start;
      curr_tok_.value_ = std::string_view(&src_->at(string_start), index_ - string_start);
      break;
    }

    if(_current_char() == '\\' && _peek_char() == opening_quote) {
      _advance_char(2);
    } else if(_at_utf8_begin()) {
      _skip_utf8_sequence();
    } else {
      _advance_char(1);
    }
  }

  //
  // When we exit this function (successfully):
  // - We are one character past the literal.
  // - The token contains the opening and closing quotes.
  //
}

inline auto n19::Lexer::_token_hyphen() -> void {
  if(index_ >= src_->size()) {
    curr_tok_ = Token::eof(src_->size() - 1, line_);
    return;
  }

  curr_tok_.pos_ = index_;
  switch(_peek_char()) {
    case '=': // '-='
      curr_tok_.type_  = TokenType::SubEq;
      curr_tok_.cat_   = TokenCategory::BinaryOp
        | TokenCategory::ArithmeticOp
        | TokenCategory::ArithAssignOp
        | TokenCategory::PointerArithOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
      break;
    case '-': // '--'
      curr_tok_.type_  = TokenType::Dec;
      curr_tok_.cat_   = TokenCategory::UnaryOp
        | TokenCategory::PointerArithOp
        | TokenCategory::ValidPostfix
        | TokenCategory::ValidPrefix
        | TokenCategory::ArithAssignOp
        | TokenCategory::ArithmeticOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
      break;
    case '>': // '->'
      curr_tok_.type_  = TokenType::SkinnyArrow;
      curr_tok_.cat_   = TokenCategory::ValidPostfix
        | TokenCategory::BinaryOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 2);
      _advance_char(2);
    default: // '-'
      curr_tok_.type_  = TokenType::Sub;
      curr_tok_.cat_   = TokenCategory::BinaryOp
        | TokenCategory::ArithmeticOp
        | TokenCategory::PointerArithOp;
      curr_tok_.value_ = std::string_view(&src_->at(index_), 1);
      _advance_char(1);
      break;
  }
}

auto n19::Lexer::_curr_char_is_reserved() const -> bool {
  switch(_current_char()) {
    case ' ':  [[fallthrough]];
    case '\r': [[fallthrough]];
    case '\b': [[fallthrough]];
    case '\t': [[fallthrough]];
    case '\n': [[fallthrough]];
    case ';':  [[fallthrough]];
    case '(':  [[fallthrough]];
    case ')':  [[fallthrough]];
    case '{':  [[fallthrough]];
    case '}':  [[fallthrough]];
    case ',':  [[fallthrough]];
    case '-':  [[fallthrough]];
    case '+':  [[fallthrough]];
    case '*':  [[fallthrough]];
    case '/':  [[fallthrough]];
    case '%':  [[fallthrough]];
    case '=':  [[fallthrough]];
    case '<':  [[fallthrough]];
    case '>':  [[fallthrough]];
    case '&':  [[fallthrough]];
    case '|':  [[fallthrough]];
    case '!':  [[fallthrough]];
    case '~':  [[fallthrough]];
    case '^':  [[fallthrough]];
    case '\'': [[fallthrough]];
    case '"':  [[fallthrough]];
    case '`':  [[fallthrough]];
    case '[':  [[fallthrough]];
    case ']':  [[fallthrough]];
    case '?':  [[fallthrough]];
    case ':':  [[fallthrough]];
    case '#':  [[fallthrough]];
    case '@':  [[fallthrough]];
    case '.':  [[fallthrough]];
    case '\\': [[fallthrough]];
    case '$':  [[fallthrough]];
    case '\0': return true;
    default:   return false;
  }
}

auto n19::Lexer::_advance_impl() -> void {
  switch(_current_char()) {
    case ' ':  return _token_skip();
    case '\r': return _token_skip();
    case '\b': return _token_skip();
    case '\t': return _token_skip();
    case '\n': return _token_newline();
    case '-':  return _token_hyphen();
    case '+':  return _token_plus();
    case '*':  return _token_asterisk();
    case '/':  return _token_fwdslash();
    case '%':  return _token_percent();
    case '=':  return _token_equals();
    case '<':  return _token_lessthan();
    case '>':  return _token_greaterthan();
    case '&':  return _token_ampersand();
    case '|':  return _token_verticalline();
    case '!':  return _token_bang();
    case '^':  return _token_uparrow();
    case '\'': return _token_singlequote();
    case '"':  return _token_quote();
    case '`':  return _token_quote();
    case ':':  return _token_colon();
    case '.':  return _token_dot();
    case '\0': return _token_null();
    case '~':  return _token_tilde();
    case '@':  return _token_at();
    case '$':  return _token_money();
    case '[':  return _token_lsqbracket();
    case ']':  return _token_rsqbracket();
    case ';':  return _token_semicolon();
    case '(':  return _token_lparen();
    case ')':  return _token_rparen();
    case '{':  return _token_lbrace();
    case '}':  return _token_rbrace();
    case ',':  return _token_comma();
    case '#':  return _token_illegal();
    case '\\': return _token_illegal();
    case '?':  return _token_illegal();
    default:   return _token_ambiguous();
  }
}

auto n19::Lexer::warn(const std::string &msg) -> Lexer& {
  ErrorCollector::display_error(
    msg,
    file_name_,
    *src_,
    curr_tok_.pos_,
    curr_tok_.line_,
    true);
  return *this;
}

auto n19::Lexer::error(const std::string &msg) -> Lexer & {
  ErrorCollector::display_error(
    msg,
    file_name_,
    *src_,
    curr_tok_.pos_,
    curr_tok_.line_,
    false);
  return *this;
}

auto n19::Lexer::warn(
  const std::string& msg,
  const size_t pos,
  const uint32_t line ) -> Lexer&
{
  ErrorCollector::display_error(
    msg,
    file_name_,
    *src_,
    pos,
    line,
    true);
  return *this;
}

auto n19::Lexer::error(
  const std::string& msg,
  const size_t pos,
  const uint32_t line ) -> Lexer&
{
  ErrorCollector::display_error(
    msg,
    file_name_,
    *src_,
    pos,
    line,
    false);
  return *this;
}