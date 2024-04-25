#include "PyFunction.hpp"

using namespace PyInterpreter;

std::string PyFunction::call(Interpreter* interpreter,
                      std::vector<std::string> arguments) {
  Environment environment = new Environment(interpreter->getGlobals());
  for (int i = 0; i < declaration.parameters.size(); i++) {
    environment.assign(declaration.parameters[i], arguments[i]);
  }

  try {
    interpreter->executeBlock(declaration.body, environment);
  } catch(ReturnObj* e) {
    return e->value;
  }
  return "";
}