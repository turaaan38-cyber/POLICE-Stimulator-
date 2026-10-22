#include "patrol/lexer.hpp"
#include "patrol/error.hpp"
#include <unordered_map>
#include <utility>
namespace patrol {
namespace {
const std::unordered_map<std::string, TokenType> keywords{
    {"let",TokenType::Let},{"fn",TokenType::Fn},{"if",TokenType::If},{"else",TokenType::Else},
    {"while",TokenType::While},{"return",TokenType::Return},{"print",TokenType::Print},
    {"true",TokenType::True},{"false",TokenType::False},{"nil",TokenType::Nil}};
bool is_digit(char c) { return c >= '0' && c <= '9'; }
bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
}
Lexer::Lexer(std::string source) : source_(std::move(source)) {}
std::vector<Token> Lexer::scan_tokens() {
    while (!is_at_end()) { start_=current_; token_column_=column_; token_line_=line_; scan_token(); }
    tokens_.push_back(Token{TokenType::End,"",{},line_,column_}); return tokens_;
}
bool Lexer::is_at_end() const noexcept { return current_ >= source_.size(); }
char Lexer::advance() { const char c=source_[current_++]; ++column_; return c; }
char Lexer::peek() const noexcept { return is_at_end() ? '\0' : source_[current_]; }
char Lexer::peek_next() const noexcept { return current_+1 >= source_.size() ? '\0' : source_[current_+1]; }
bool Lexer::match(char expected) {
    if (is_at_end() || source_[current_] != expected) return false;
    ++current_; ++column_; return true;
}
void Lexer::add(TokenType type, Literal literal) {
    tokens_.push_back(Token{type,source_.substr(start_,current_-start_),std::move(literal),token_line_,token_column_});
}
void Lexer::scan_token() {
    const char c=advance();
    switch(c) {
    case '(':add(TokenType::LeftParen);break; case ')':add(TokenType::RightParen);break;
    case '{':add(TokenType::LeftBrace);break; case '}':add(TokenType::RightBrace);break;
    case ',':add(TokenType::Comma);break; case '.':add(TokenType::Dot);break;
    case '-':add(TokenType::Minus);break; case '+':add(TokenType::Plus);break;
    case ';':add(TokenType::Semicolon);break; case '*':add(TokenType::Star);break;
    case '!':add(match('=')?TokenType::BangEqual:TokenType::Bang);break;
    case '=':add(match('=')?TokenType::EqualEqual:TokenType::Equal);break;
    case '<':add(match('=')?TokenType::LessEqual:TokenType::Less);break;
    case '>':add(match('=')?TokenType::GreaterEqual:TokenType::Greater);break;
    case '&':if(match('&'))add(TokenType::And);else throw ScriptError(line_,token_column_,"expected '&' after '&'");break;
    case '|':if(match('|'))add(TokenType::Or);else throw ScriptError(line_,token_column_,"expected '|' after '|'");break;
    case '/':if(match('/')){while(peek()!='\n'&&!is_at_end())advance();}else add(TokenType::Slash);break;
    case ' ':case '\r':case '\t':break;
    case '\n':++line_;column_=1;break;
    case '"':string_literal();break;
    default:if(is_digit(c))number_literal();else if(is_alpha(c))identifier();else throw ScriptError(line_,token_column_,std::string("unexpected character '")+c+"'"); 
    }
}
void Lexer::string_literal() {
    std::string value;
    while(!is_at_end()&&peek()!='"') {
        char c=advance(); if(c=='\n'){++line_;column_=1;}
        if(c=='\\') {
            if(is_at_end())throw ScriptError(line_,token_column_,"unterminated escape sequence");
            const char escaped=advance();
            switch(escaped){case 'n':value.push_back('\n');break;case 'r':value.push_back('\r');break;
            case 't':value.push_back('\t');break;case '"':value.push_back('"');break;case '\\':value.push_back('\\');break;
            default:throw ScriptError(line_,column_-1,std::string("unsupported escape sequence \\")+escaped+"'");}
        } else value.push_back(c);
    }
    if(is_at_end())throw ScriptError(line_,token_column_,"unterminated string");
    advance(); add(TokenType::String,std::move(value));
}
void Lexer::number_literal() {
    while(is_digit(peek()))advance();
    if(peek()=='.'&&is_digit(peek_next())){advance();while(is_digit(peek()))advance();}
    const std::string text=source_.substr(start_,current_-start_);
    try{add(TokenType::Number,std::stod(text));}catch(...){throw ScriptError(line_,token_column_,"invalid numeric literal");}
}
void Lexer::identifier() {
    while(is_alpha(peek())||is_digit(peek()))advance();
    const std::string text=source_.substr(start_,current_-start_);
    const auto found=keywords.find(text);
    add(found==keywords.end()?TokenType::Identifier:found->second,{});
    if(found!=keywords.end()&&(found->second==TokenType::True||found->second==TokenType::False))tokens_.back().literal=(found->second==TokenType::True);
}
} // namespace patrol
