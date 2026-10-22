#include "patrol/runtime.hpp"
#include "patrol/error.hpp"
#include <chrono>
#include <cmath>
#include <functional>
#include <iomanip>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
namespace patrol { namespace {
class Environment;class Interpreter;struct Callable;
using Value=std::variant<std::monostate,bool,double,std::string,std::shared_ptr<Callable>>;
class Environment final {
public: explicit Environment(std::shared_ptr<Environment> e={}):enclosing_(std::move(e)){}
 void define(const std::string& n,Value v){values_[n]=std::move(v);}
 Value get(const Token& n)const{auto i=values_.find(n.lexeme);if(i!=values_.end())return i->second;if(enclosing_)return enclosing_->get(n);throw ScriptError(n.line,n.column,"undefined variable '"+n.lexeme+"'");}
 void assign(const Token& n,Value v){auto i=values_.find(n.lexeme);if(i!=values_.end()){i->second=std::move(v);return;}if(enclosing_){enclosing_->assign(n,std::move(v));return;}throw ScriptError(n.line,n.column,"undefined variable '"+n.lexeme+"'");}
private:std::unordered_map<std::string,Value> values_;std::shared_ptr<Environment> enclosing_;
};
struct Callable{virtual ~Callable()=default;virtual std::size_t arity()const noexcept=0;virtual Value call(Interpreter&,const std::vector<Value>&)=0;};
struct ReturnSignal{Value value;Token keyword;};
bool truthy(const Value& v){if(std::holds_alternative<std::monostate>(v))return false;if(auto*b=std::get_if<bool>(&v))return *b;return true;}
std::string stringify(const Value& v){if(std::holds_alternative<std::monostate>(v))return "nil";if(auto*b=std::get_if<bool>(&v))return *b?"true":"false";if(auto*n=std::get_if<double>(&v)){std::ostringstream o;o<<std::setprecision(15)<<*n;return o.str();}if(auto*s=std::get_if<std::string>(&v))return *s;return "<function>";}
double require_number(const Value&v,const Token&t){if(auto*n=std::get_if<double>(&v))return *n;throw ScriptError(t.line,t.column,"operand must be a number");}
bool equal_values(const Value&a,const Value&b){if(a.index()!=b.index())return false;if(std::holds_alternative<std::monostate>(a))return true;if(auto*v=std::get_if<bool>(&a))return *v==std::get<bool>(b);if(auto*v=std::get_if<double>(&a))return *v==std::get<double>(b);if(auto*v=std::get_if<std::string>(&a))return *v==std::get<std::string>(b);return std::get<std::shared_ptr<Callable>>(a)==std::get<std::shared_ptr<Callable>>(b);}
struct NativeFunction final:Callable{std::size_t expected;std::function<Value(const std::vector<Value>&)> body;NativeFunction(std::size_t n,std::function<Value(const std::vector<Value>&)>f):expected(n),body(std::move(f)){}std::size_t arity()const noexcept override{return expected;}Value call(Interpreter&,const std::vector<Value>&a)override{return body(a);}};
struct UserFunction final:Callable{FunctionStmt* declaration;std::shared_ptr<Environment> closure;UserFunction(FunctionStmt*d,std::shared_ptr<Environment>e):declaration(d),closure(std::move(e)){}std::size_t arity()const noexcept override{return declaration->params.size();}Value call(Interpreter&,const std::vector<Value>&)override;};
class Interpreter final {
public:explicit Interpreter(std::ostream&o):output(o),globals(std::make_shared<Environment>()),env(globals){
 auto start=std::chrono::steady_clock::now();globals->define("clock",std::make_shared<NativeFunction>(0,[start](const std::vector<Value>&){return Value{std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()};}));
 globals->define("abs",std::make_shared<NativeFunction>(1,[](const std::vector<Value>&v){if(auto*n=std::get_if<double>(&v.at(0)))return Value{std::abs(*n)};throw ScriptError(1,1,"abs() expects a number");}));}
 void run(std::vector<StmtPtr>&s){try{for(auto&x:s)execute(x.get());}catch(const ReturnSignal&r){throw ScriptError(r.keyword.line,r.keyword.column,"return is only valid inside a function");}}
 Value evaluate(Expr*e){
 if(auto*x=dynamic_cast<LiteralExpr*>(e)){if(std::holds_alternative<std::monostate>(x->value))return std::monostate{};if(auto*v=std::get_if<bool>(&x->value))return *v;if(auto*v=std::get_if<double>(&x->value))return *v;if(auto*v=std::get_if<std::string>(&x->value))return *v;}
 if(auto*x=dynamic_cast<VariableExpr*>(e))return env->get(x->name);
 if(auto*x=dynamic_cast<AssignExpr*>(e)){auto v=evaluate(x->value.get());env->assign(x->name,v);return v;}
 if(auto*x=dynamic_cast<UnaryExpr*>(e)){auto r=evaluate(x->right.get());if(x->op.type==TokenType::Bang)return !truthy(r);if(x->op.type==TokenType::Minus)return -require_number(r,x->op);}
 if(auto*x=dynamic_cast<LogicalExpr*>(e)){auto l=evaluate(x->left.get());if(x->op.type==TokenType::Or){if(truthy(l))return l;}else if(!truthy(l))return l;return evaluate(x->right.get());}
 if(auto*x=dynamic_cast<BinaryExpr*>(e)){auto l=evaluate(x->left.get()),r=evaluate(x->right.get());switch(x->op.type){
 case TokenType::BangEqual:return !equal_values(l,r);case TokenType::EqualEqual:return equal_values(l,r);
 case TokenType::Greater:return require_number(l,x->op)>require_number(r,x->op);case TokenType::GreaterEqual:return require_number(l,x->op)>=require_number(r,x->op);
 case TokenType::Less:return require_number(l,x->op)<require_number(r,x->op);case TokenType::LessEqual:return require_number(l,x->op)<=require_number(r,x->op);
 case TokenType::Minus:return require_number(l,x->op)-require_number(r,x->op);
 case TokenType::Slash:{double d=require_number(r,x->op);if(d==0.0)throw ScriptError(x->op.line,x->op.column,"division by zero");return require_number(l,x->op)/d;}
 case TokenType::Star:return require_number(l,x->op)*require_number(r,x->op);
 case TokenType::Plus:if(auto*n=std::get_if<double>(&l))return *n+require_number(r,x->op);if(auto*s=std::get_if<std::string>(&l)){if(auto*t=std::get_if<std::string>(&r))return *s+*t;}throw ScriptError(x->op.line,x->op.column,"'+' expects two numbers or two strings");default:break;}}
 if(auto*x=dynamic_cast<CallExpr*>(e)){auto c=evaluate(x->callee.get());std::vector<Value>a;for(auto&arg:x->arguments)a.push_back(evaluate(arg.get()));auto*f=std::get_if<std::shared_ptr<Callable>>(&c);if(!f||!*f)throw ScriptError(x->paren.line,x->paren.column,"only functions can be called");if(a.size()!=(*f)->arity())throw ScriptError(x->paren.line,x->paren.column,"expected "+std::to_string((*f)->arity())+" arguments but got "+std::to_string(a.size()));return (*f)->call(*this,a);}
 throw ScriptError(e->where.line,e->where.column,"internal runtime error: unsupported expression");}
 void execute_block(std::vector<StmtPtr>&s,std::shared_ptr<Environment> next){auto prev=env;env=std::move(next);try{for(auto&x:s)execute(x.get());}catch(...){env=prev;throw;}env=prev;}
 Value call_user(UserFunction&f,const std::vector<Value>&a){auto local=std::make_shared<Environment>(f.closure);for(std::size_t i=0;i<f.declaration->params.size();++i)local->define(f.declaration->params[i].lexeme,a[i]);try{execute_block(f.declaration->body,std::move(local));}catch(const ReturnSignal&r){return r.value;}return std::monostate{};}
private:std::ostream&output;std::shared_ptr<Environment>globals,env;
 void execute(Stmt*s){
 if(auto*x=dynamic_cast<ExpressionStmt*>(s)){(void)evaluate(x->expression.get());return;}if(auto*x=dynamic_cast<PrintStmt*>(s)){output<<stringify(evaluate(x->expression.get()))<<'\n';return;}
 if(auto*x=dynamic_cast<VarStmt*>(s)){env->define(x->name.lexeme,evaluate(x->initializer.get()));return;}
 if(auto*x=dynamic_cast<BlockStmt*>(s)){execute_block(x->statements,std::make_shared<Environment>(env));return;}
 if(auto*x=dynamic_cast<IfStmt*>(s)){if(truthy(evaluate(x->condition.get())))execute(x->then_branch.get());else if(x->else_branch)execute(x->else_branch.get());return;}
 if(auto*x=dynamic_cast<WhileStmt*>(s)){std::size_t n=0;while(truthy(evaluate(x->condition.get()))){if(++n>1'000'000)throw ScriptError(x->where.line,x->where.column,"loop exceeded the 1,000,000-iteration safety limit");execute(x->body.get());}return;}
 if(auto*x=dynamic_cast<FunctionStmt*>(s)){env->define(x->name.lexeme,std::make_shared<UserFunction>(x,env));return;}
 if(auto*x=dynamic_cast<ReturnStmt*>(s)){Value v=x->value?evaluate(x->value.get()):Value{std::monostate{}};throw ReturnSignal{std::move(v),x->keyword};}
 throw ScriptError(s->where.line,s->where.column,"internal runtime error: unsupported statement");}
};
Value UserFunction::call(Interpreter&i,const std::vector<Value>&a){return i.call_user(*this,a);}
}
class Runtime::Implementation final{public:explicit Implementation(std::ostream&o):interpreter(o){}Interpreter interpreter;};
Runtime::Runtime(std::ostream&o):implementation_(std::make_unique<Implementation>(o)){}Runtime::~Runtime()=default;Runtime::Runtime(Runtime&&)noexcept=default;Runtime&Runtime::operator=(Runtime&&)noexcept=default;
void Runtime::run(std::vector<StmtPtr>&s){implementation_->interpreter.run(s);}
} // namespace patrol
