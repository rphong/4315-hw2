#include "Environment.hpp"

using namespace PyInterpreter;

std::string Environment::get(Token name) {
  if (m_values.find(name.lexeme) != m_values.end()) {
    return m_values[name.lexeme];
  }
  if (enclosing != nullptr) {
    return enclosing->get(name);
  }
  throw new std::runtime_error("Line " + std::to_string(name.line) +
                               ": Undefined variable " + name.lexeme + ".");
}

void Environment::assign(Token name, std::string value) {
  m_values[name.lexeme] = value;
}