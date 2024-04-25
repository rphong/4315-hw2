// Interpreter created using Crafting Interpreters by Robert Nystrom for
// reference https://craftinginterpreters.com/contents.html

#include <iostream>

#include "Python.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    return -1;
  } else {
    PyInterpreter::Python interpreter{};
    interpreter.run(argv[1]);
  }
  return 0;
}