#include "Environment.hpp"

using namespace PyInterpreter;

std::string Environment::get(Token name) {
  if (m_values.find(name.lexeme) != m_values.end()) {
    return m_values[name.lexeme];
  }
  if (enclosing != nullptr) {
    return enclosing->get(name);
  }
  if (m_functions.find(name.lexeme) != m_functions.end()) {
    return name.lexeme;
  }
  throw new std::runtime_error("Line " + std::to_string(name.line) +
                               ": Undefined variable " + name.lexeme + ".");
}

PyFunction* Environment::getFunction(std::string name) {
  if (m_functions.find(name) != m_functions.end()) {
    return m_functions[name];
  }
  if (enclosing != nullptr) {
    return enclosing->getFunction(name);
  }
  throw new std::runtime_error("Undefined function " + name + ".");
}

void Environment::assign(Token name, std::string value) {
  m_values[name.lexeme] = value;
}

void Environment::assignFunction(Token name, PyFunction* func) {
  m_functions[name.lexeme] = func;
}