#pragma once

#include <stdexcept>
#include <string>

namespace PyInterpreter {
class ReturnObj : public std::runtime_error {
 public:
  std::string value;

  ReturnObj(const std::string& val) : value(val), std::runtime_error(val) {}
};
}  // namespace PyInterpreter