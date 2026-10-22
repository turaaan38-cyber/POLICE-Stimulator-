#include "patrol/error.hpp"
#include "patrol/lexer.hpp"
#include "patrol/parser.hpp"
#include "patrol/runtime.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
namespace {
void print_usage(const char* executable) {
    std::cout << "PatrolScript 0.1\nUsage: " << executable << " [--check] <file.patrol>\n       " << executable << " --version\n";
}
}
int main(int argc,char** argv) {
    if(argc==2&&std::string(argv[1])=="--version"){std::cout<<"PatrolScript 0.1.0\n";return 0;}
    if(argc==2&&(std::string(argv[1])=="--help"||std::string(argv[1])=="-h")){print_usage(argv[0]);return 0;}
    bool check_only=false; int path_index=1;
    if(argc>1&&std::string(argv[1])=="--check"){check_only=true;path_index=2;}
    if(argc<=path_index){print_usage(argv[0]);return 2;}
    if(argc>path_index+1){std::cerr<<"error: unexpected extra argument\n";return 2;}
    const std::string path=argv[path_index]; std::ifstream file(path,std::ios::binary);
    if(!file){std::cerr<<"error: cannot open script: "<<path<<'\n';return 2;}
    const std::string source((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    try {
        patrol::Lexer lexer(source); auto tokens=lexer.scan_tokens(); patrol::Parser parser(std::move(tokens)); auto statements=parser.parse();
        if(check_only){std::cout<<"OK: "<<path<<" parsed successfully\n";return 0;}
        patrol::Runtime runtime(std::cout);runtime.run(statements);return 0;
    } catch(const patrol::ScriptError& error) {
        std::cerr<<path<<':'<<error.line()<<':'<<error.column()<<": error: "<<error.what()<<'\n';return 1;
    } catch(const std::exception& error){std::cerr<<path<<": error: "<<error.what()<<'\n';return 1;}
}
