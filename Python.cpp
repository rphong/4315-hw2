#include "Python.hpp"

using namespace PyInterpreter;

void Python::run(std::string file) {
  std::ifstream ifstr(file);
  std::ostringstream buffer;
  buffer << ifstr.rdbuf();
  executeCode(buffer.str());

  ifstr.close();
}

void Python::executeCode(std::string code) {
  Interpreter interpreter = Interpreter();
  Scanner scanner = Scanner(code);
  std::vector<Token> tokens = scanner.scanTokens();

  Parser parser = Parser(tokens);
  std::vector<Stmt*> statements = parser.parse();

  interpreter.interpret(statements);
}