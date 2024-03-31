// Interpreter created using Crafting Interpreters by Robert Nystrom for reference
// https://craftinginterpreters.com/contents.html
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Interpreter.hpp"
#include "Scanner.hpp"
#include "Parser.hpp"

namespace PyInterpreter {
class Python {
 public:
  void run(std::string file);

 private:
  void executeCode(std::string code);
};
}  // namespace PyInterpreter