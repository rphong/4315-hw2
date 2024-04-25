#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Token.hpp"
#include "PyCallable.hpp"

namespace PyInterpreter {
class PyFunction;
class Environment {
 public:
  Environment() : enclosing(nullptr){};
  Environment(Environment* encl) : enclosing(encl){};
  std::string get(Token name);
  PyFunction* getFunction(std::string name);
  void assign(Token name, std::string value);
  void assignFunction(Token name, PyFunction* func);

  Environment* enclosing;

 private:
  std::unordered_map<std::string, std::string> m_values;
  std::unordered_map<std::string, PyFunction*> m_functions;
};
}  // namespace PyInterpreter