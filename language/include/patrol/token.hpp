#pragma once
#include <string>
#include <variant>
namespace patrol {
enum class TokenType {
    LeftParen, RightParen, LeftBrace, RightBrace, Comma, Dot, Minus, Plus, Semicolon, Slash, Star,
    Bang, BangEqual, Equal, EqualEqual, Greater, GreaterEqual, Less, LessEqual, And, Or,
    Identifier, String, Number, Let, Fn, If, Else, While, Return, Print, True, False, Nil, End
};
using Literal = std::variant<std::monostate, bool, double, std::string>;
struct Token { TokenType type{TokenType::End}; std::string lexeme; Literal literal{}; int line{1}; int column{1}; };
const char* token_type_name(TokenType type) noexcept;
} // namespace patrol
