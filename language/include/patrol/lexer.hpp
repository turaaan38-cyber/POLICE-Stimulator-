#pragma once
#include "patrol/token.hpp"
#include <string>
#include <vector>
namespace patrol {
class Lexer final {
public:
    explicit Lexer(std::string source);
    [[nodiscard]] std::vector<Token> scan_tokens();
private:
    std::string source_; std::vector<Token> tokens_;
    std::size_t start_{0}, current_{0};
    int line_{1}, column_{1}, token_column_{1}, token_line_{1};
    [[nodiscard]] bool is_at_end() const noexcept;
    char advance(); [[nodiscard]] char peek() const noexcept; [[nodiscard]] char peek_next() const noexcept;
    bool match(char expected); void scan_token(); void add(TokenType type, Literal literal = {});
    void string_literal(); void number_literal(); void identifier();
};
} // namespace patrol
