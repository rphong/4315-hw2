#pragma once

#include <string>

#include "Interpreter.hpp"
#include "Environment.hpp"
#include "PyCallable.hpp"
#include "Stmt.hpp"

namespace PyInterpreter {
class Interpreter;
class PyFunction : public PyCallable {
 public:
  PyFunction(const Function& func) : declaration(func) {}

  std::string call(Interpreter* interpreter, std::vector<std::string> arguments);

  int arity() { return declaration.parameters.size(); }

 private:
  const Function declaration;
};
}  // namespace PyInterpreter