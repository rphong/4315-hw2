#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Token.hpp"

namespace PyInterpreter {
class Environment {
 public:
  Environment() : enclosing(nullptr){};
  Environment(Environment* encl): enclosing(encl){};
  std::string get(Token name);  
  void assign(Token name, std::string value);

  Environment* enclosing;

 private:
  std::unordered_map<std::string, std::string> m_values;
};
}  // namespace PyInterpreter