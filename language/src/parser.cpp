#include "patrol/parser.hpp"
#include "patrol/error.hpp"
#include <initializer_list>
#include <utility>
namespace patrol {
Parser::Parser(std::vector<Token> tokens):tokens_(std::move(tokens)){}
std::vector<StmtPtr> Parser::parse(){std::vector<StmtPtr>s;while(!is_at_end())s.push_back(declaration());return s;}
bool Parser::is_at_end()const noexcept{return peek().type==TokenType::End;}
const Token& Parser::peek()const{return tokens_.at(current_);}
const Token& Parser::previous()const{return tokens_.at(current_-1);}
bool Parser::check(TokenType t)const{return peek().type==t;}
bool Parser::match(std::initializer_list<TokenType> ts){for(auto t:ts)if(check(t)){advance();return true;}return false;}
const Token& Parser::advance(){if(!is_at_end())++current_;return previous();}
const Token& Parser::consume(TokenType t,const char*m){if(check(t))return advance();throw ScriptError(peek().line,peek().column,m);}
StmtPtr Parser::declaration(){if(match({TokenType::Fn}))return function_declaration();if(match({TokenType::Let}))return var_declaration();return statement();}
StmtPtr Parser::function_declaration(){
 const Token opening=previous();const Token& name=consume(TokenType::Identifier,"expected function name after 'fn'");
 consume(TokenType::LeftParen,"expected '(' after function name");std::vector<Token> params;
 if(!check(TokenType::RightParen))do{if(params.size()>=64)throw ScriptError(peek().line,peek().column,"a function cannot have more than 64 parameters");params.push_back(consume(TokenType::Identifier,"expected parameter name"));}while(match({TokenType::Comma}));
 consume(TokenType::RightParen,"expected ')' after parameters");const Token& body_open=consume(TokenType::LeftBrace,"expected '{' before function body");
 auto body=block_statement(body_open);auto* block=dynamic_cast<BlockStmt*>(body.get());
 if(!block)throw ScriptError(opening.line,opening.column,"internal parser error while reading function body");
 return std::make_unique<FunctionStmt>(opening,name,std::move(params),std::move(block->statements));
}
StmtPtr Parser::var_declaration(){const Token start=previous();const Token& name=consume(TokenType::Identifier,"expected variable name after 'let'");ExprPtr init;
 if(match({TokenType::Equal}))init=expression();else init=std::make_unique<LiteralExpr>(name,Literal{std::monostate{}});
 consume(TokenType::Semicolon,"expected ';' after variable declaration");return std::make_unique<VarStmt>(start,name,std::move(init));}
StmtPtr Parser::statement(){if(match({TokenType::Print}))return print_statement();if(match({TokenType::If}))return if_statement();if(match({TokenType::While}))return while_statement();if(match({TokenType::Return}))return return_statement();if(match({TokenType::LeftBrace}))return block_statement(previous());return expression_statement();}
StmtPtr Parser::print_statement(){const Token start=previous();auto e=expression();consume(TokenType::Semicolon,"expected ';' after print value");return std::make_unique<PrintStmt>(start,std::move(e));}
StmtPtr Parser::if_statement(){const Token start=previous();consume(TokenType::LeftParen,"expected '(' after 'if'");auto c=expression();consume(TokenType::RightParen,"expected ')' after if condition");auto y=statement();StmtPtr n;if(match({TokenType::Else}))n=statement();return std::make_unique<IfStmt>(start,std::move(c),std::move(y),std::move(n));}
StmtPtr Parser::while_statement(){const Token start=previous();consume(TokenType::LeftParen,"expected '(' after 'while'");auto c=expression();consume(TokenType::RightParen,"expected ')' after while condition");return std::make_unique<WhileStmt>(start,std::move(c),statement());}
StmtPtr Parser::return_statement(){const Token k=previous();ExprPtr v;if(!check(TokenType::Semicolon))v=expression();consume(TokenType::Semicolon,"expected ';' after return value");return std::make_unique<ReturnStmt>(k,std::move(v));}
StmtPtr Parser::block_statement(const Token& open){std::vector<StmtPtr>s;while(!check(TokenType::RightBrace)&&!is_at_end())s.push_back(declaration());consume(TokenType::RightBrace,"expected '}' after block");return std::make_unique<BlockStmt>(open,std::move(s));}
StmtPtr Parser::expression_statement(){const Token start=peek();auto e=expression();consume(TokenType::Semicolon,"expected ';' after expression");return std::make_unique<ExpressionStmt>(start,std::move(e));}
ExprPtr Parser::expression(){return assignment();}
ExprPtr Parser::assignment(){auto e=logical_or();if(match({TokenType::Equal})){const Token eq=previous();auto v=assignment();if(auto*x=dynamic_cast<VariableExpr*>(e.get()))return std::make_unique<AssignExpr>(eq,x->name,std::move(v));throw ScriptError(eq.line,eq.column,"invalid assignment target");}return e;}
ExprPtr Parser::logical_or(){auto e=logical_and();while(match({TokenType::Or})){auto op=previous();e=std::make_unique<LogicalExpr>(std::move(e),op,logical_and());}return e;}
ExprPtr Parser::logical_and(){auto e=equality();while(match({TokenType::And})){auto op=previous();e=std::make_unique<LogicalExpr>(std::move(e),op,equality());}return e;}
ExprPtr Parser::equality(){auto e=comparison();while(match({TokenType::BangEqual,TokenType::EqualEqual})){auto op=previous();e=std::make_unique<BinaryExpr>(std::move(e),op,comparison());}return e;}
ExprPtr Parser::comparison(){auto e=term();while(match({TokenType::Greater,TokenType::GreaterEqual,TokenType::Less,TokenType::LessEqual})){auto op=previous();e=std::make_unique<BinaryExpr>(std::move(e),op,term());}return e;}
ExprPtr Parser::term(){auto e=factor();while(match({TokenType::Minus,TokenType::Plus})){auto op=previous();e=std::make_unique<BinaryExpr>(std::move(e),op,factor());}return e;}
ExprPtr Parser::factor(){auto e=unary();while(match({TokenType::Slash,TokenType::Star})){auto op=previous();e=std::make_unique<BinaryExpr>(std::move(e),op,unary());}return e;}
ExprPtr Parser::unary(){if(match({TokenType::Bang,TokenType::Minus})){auto op=previous();return std::make_unique<UnaryExpr>(op,unary());}return call();}
ExprPtr Parser::call(){auto e=primary();while(match({TokenType::LeftParen})){std::vector<ExprPtr>a;finish_call_arguments(a);auto p=previous();e=std::make_unique<CallExpr>(std::move(e),p,std::move(a));}return e;}
void Parser::finish_call_arguments(std::vector<ExprPtr>& a){if(!check(TokenType::RightParen))do{if(a.size()>=64)throw ScriptError(peek().line,peek().column,"a call cannot have more than 64 arguments");a.push_back(expression());}while(match({TokenType::Comma}));consume(TokenType::RightParen,"expected ')' after arguments");}
ExprPtr Parser::primary(){if(match({TokenType::False}))return std::make_unique<LiteralExpr>(previous(),Literal{false});if(match({TokenType::True}))return std::make_unique<LiteralExpr>(previous(),Literal{true});if(match({TokenType::Nil}))return std::make_unique<LiteralExpr>(previous(),Literal{std::monostate{}});
 if(match({TokenType::Number,TokenType::String}))return std::make_unique<LiteralExpr>(previous(),previous().literal);if(match({TokenType::Identifier}))return std::make_unique<VariableExpr>(previous());
 if(match({TokenType::LeftParen})){auto e=expression();consume(TokenType::RightParen,"expected ')' after expression");return e;}throw ScriptError(peek().line,peek().column,"expected expression");}
} // namespace patrol
