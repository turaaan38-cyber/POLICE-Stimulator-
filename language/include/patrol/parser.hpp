#pragma once
#include "patrol/token.hpp"
#include <memory>
#include <utility>
#include <vector>
namespace patrol {
struct Expr { explicit Expr(Token source) : where(std::move(source)) {} virtual ~Expr() = default; Token where; };
using ExprPtr = std::unique_ptr<Expr>;
struct LiteralExpr final : Expr { LiteralExpr(Token t, Literal v) : Expr(std::move(t)), value(std::move(v)) {} Literal value; };
struct VariableExpr final : Expr { explicit VariableExpr(Token t) : Expr(t), name(std::move(t)) {} Token name; };
struct AssignExpr final : Expr { AssignExpr(Token t, Token n, ExprPtr v) : Expr(std::move(t)), name(std::move(n)), value(std::move(v)) {} Token name; ExprPtr value; };
struct UnaryExpr final : Expr { UnaryExpr(Token t, ExprPtr r) : Expr(t), op(std::move(t)), right(std::move(r)) {} Token op; ExprPtr right; };
struct BinaryExpr final : Expr { BinaryExpr(ExprPtr l, Token o, ExprPtr r) : Expr(o), left(std::move(l)), op(std::move(o)), right(std::move(r)) {} ExprPtr left; Token op; ExprPtr right; };
struct LogicalExpr final : Expr { LogicalExpr(ExprPtr l, Token o, ExprPtr r) : Expr(o), left(std::move(l)), op(std::move(o)), right(std::move(r)) {} ExprPtr left; Token op; ExprPtr right; };
struct CallExpr final : Expr { CallExpr(ExprPtr c, Token p, std::vector<ExprPtr> a) : Expr(p), callee(std::move(c)), paren(std::move(p)), arguments(std::move(a)) {} ExprPtr callee; Token paren; std::vector<ExprPtr> arguments; };
struct Stmt { explicit Stmt(Token source) : where(std::move(source)) {} virtual ~Stmt() = default; Token where; };
using StmtPtr = std::unique_ptr<Stmt>;
struct ExpressionStmt final : Stmt { ExpressionStmt(Token t, ExprPtr e) : Stmt(std::move(t)), expression(std::move(e)) {} ExprPtr expression; };
struct PrintStmt final : Stmt { PrintStmt(Token t, ExprPtr e) : Stmt(std::move(t)), expression(std::move(e)) {} ExprPtr expression; };
struct VarStmt final : Stmt { VarStmt(Token t, Token n, ExprPtr e) : Stmt(std::move(t)), name(std::move(n)), initializer(std::move(e)) {} Token name; ExprPtr initializer; };
struct BlockStmt final : Stmt { BlockStmt(Token t, std::vector<StmtPtr> s) : Stmt(std::move(t)), statements(std::move(s)) {} std::vector<StmtPtr> statements; };
struct IfStmt final : Stmt { IfStmt(Token t, ExprPtr c, StmtPtr y, StmtPtr n) : Stmt(std::move(t)), condition(std::move(c)), then_branch(std::move(y)), else_branch(std::move(n)) {} ExprPtr condition; StmtPtr then_branch; StmtPtr else_branch; };
struct WhileStmt final : Stmt { WhileStmt(Token t, ExprPtr c, StmtPtr b) : Stmt(std::move(t)), condition(std::move(c)), body(std::move(b)) {} ExprPtr condition; StmtPtr body; };
struct FunctionStmt final : Stmt { FunctionStmt(Token t, Token n, std::vector<Token> p, std::vector<StmtPtr> b) : Stmt(std::move(t)), name(std::move(n)), params(std::move(p)), body(std::move(b)) {} Token name; std::vector<Token> params; std::vector<StmtPtr> body; };
struct ReturnStmt final : Stmt { ReturnStmt(Token t, ExprPtr v) : Stmt(t), keyword(std::move(t)), value(std::move(v)) {} Token keyword; ExprPtr value; };
class Parser final {
public:
    explicit Parser(std::vector<Token> tokens); [[nodiscard]] std::vector<StmtPtr> parse();
private:
    std::vector<Token> tokens_; std::size_t current_{0};
    [[nodiscard]] bool is_at_end() const noexcept; [[nodiscard]] const Token& peek() const;
    [[nodiscard]] const Token& previous() const; bool check(TokenType type) const;
    bool match(std::initializer_list<TokenType> types); const Token& advance();
    const Token& consume(TokenType type, const char* message);
    StmtPtr declaration(); StmtPtr function_declaration(); StmtPtr var_declaration(); StmtPtr statement();
    StmtPtr print_statement(); StmtPtr if_statement(); StmtPtr while_statement(); StmtPtr return_statement();
    StmtPtr block_statement(const Token& opening); StmtPtr expression_statement();
    ExprPtr expression(); ExprPtr assignment(); ExprPtr logical_or(); ExprPtr logical_and(); ExprPtr equality();
    ExprPtr comparison(); ExprPtr term(); ExprPtr factor(); ExprPtr unary(); ExprPtr call(); ExprPtr primary();
    void finish_call_arguments(std::vector<ExprPtr>& arguments);
};
} // namespace patrol
