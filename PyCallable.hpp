#pragma once

#include <vector>
#include <string>

namespace PyInterpreter {
class Interpreter;
class PyCallable {
 public:
  virtual int arity() = 0;
  virtual std::string call(Interpreter* interpreter,
                    std::vector<std::string> arguments) = 0;
};


}  // namespace PyInterpreter