#include "patrol/error.hpp"
#include "patrol/lexer.hpp"
#include "patrol/parser.hpp"
#include "patrol/runtime.hpp"
#include <iostream>
#include <sstream>
#include <string>
namespace {
int failures=0;
void check(bool passed,const char* label){if(passed)std::cout<<"[PASS] "<<label<<'\n';else{std::cerr<<"[FAIL] "<<label<<'\n';++failures;}}
template<typename Function> void expect_script_error(Function&& action,int line,int column,const char* label){
    try{action();check(false,label);}catch(const patrol::ScriptError& e){check(e.line()==line&&e.column()==column,label);}catch(...){check(false,label);}
}
std::string execute(const std::string& source){
    patrol::Lexer lexer(source);patrol::Parser parser(lexer.scan_tokens());auto statements=parser.parse();
    std::ostringstream output;patrol::Runtime runtime(output);runtime.run(statements);return output.str();
}
}
int main(){
    {patrol::Lexer lexer("let x = 12.5;\nprint \"a\\nb\"; // comment\n");const auto tokens=lexer.scan_tokens();
     check(tokens.size()==9,"lexer emits expected tokens");
     check(tokens.size()>6&&tokens[5].line==2&&tokens[5].column==1,"lexer tracks line and column after newline");
     check(tokens.size()>6&&std::get<std::string>(tokens[6].literal)=="a\nb","lexer decodes supported string escapes");}
    {const std::string source=R"PATROL(
fn add(a, b) { return a + b; }
let i = 0; let total = 0;
while (i < 4) { total = add(total, i); i = i + 1; }
if (total == 6 && true) { print total; } else { print 0; }
print "patrol " + "ready";
)PATROL";
     check(execute(source)=="6\npatrol ready\n","runtime executes functions, return, scopes, loops, conditionals, and strings");}
    expect_script_error([]{(void)execute("let x = ;\n");},1,9,"parser reports syntax error location");
    expect_script_error([]{(void)execute("let x = 1;\nprint missing;\n");},2,7,"runtime reports undefined variable location");
    expect_script_error([]{(void)execute("print 5 / 0;\n");},1,9,"runtime detects division by zero");
    expect_script_error([]{(void)execute("return 2;\n");},1,1,"runtime rejects return outside a function");
    expect_script_error([]{patrol::Lexer lexer("print @;\n");(void)lexer.scan_tokens();},1,7,"lexer reports unexpected-character location");
    if(failures!=0){std::cerr<<failures<<" test(s) failed\n";return 1;}
    std::cout<<"All PatrolScript tests passed\n";return 0;
}
