#include "patrol/token.hpp"
namespace patrol {
const char* token_type_name(const TokenType type) noexcept {
    switch (type) {
    case TokenType::LeftParen:return "LeftParen"; case TokenType::RightParen:return "RightParen";
    case TokenType::LeftBrace:return "LeftBrace"; case TokenType::RightBrace:return "RightBrace";
    case TokenType::Comma:return "Comma"; case TokenType::Dot:return "Dot"; case TokenType::Minus:return "Minus";
    case TokenType::Plus:return "Plus"; case TokenType::Semicolon:return "Semicolon"; case TokenType::Slash:return "Slash";
    case TokenType::Star:return "Star"; case TokenType::Bang:return "Bang"; case TokenType::BangEqual:return "BangEqual";
    case TokenType::Equal:return "Equal"; case TokenType::EqualEqual:return "EqualEqual"; case TokenType::Greater:return "Greater";
    case TokenType::GreaterEqual:return "GreaterEqual"; case TokenType::Less:return "Less"; case TokenType::LessEqual:return "LessEqual";
    case TokenType::And:return "And"; case TokenType::Or:return "Or"; case TokenType::Identifier:return "Identifier";
    case TokenType::String:return "String"; case TokenType::Number:return "Number"; case TokenType::Let:return "Let";
    case TokenType::Fn:return "Fn"; case TokenType::If:return "If"; case TokenType::Else:return "Else";
    case TokenType::While:return "While"; case TokenType::Return:return "Return"; case TokenType::Print:return "Print";
    case TokenType::True:return "True"; case TokenType::False:return "False"; case TokenType::Nil:return "Nil";
    case TokenType::End:return "End";
    }
    return "Unknown";
}
} // namespace patrol
