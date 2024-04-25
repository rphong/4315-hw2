#include "Interpreter.hpp"

using namespace PyInterpreter;

Environment Interpreter::m_environment = Environment();

void Interpreter::visit(Assign& expr) {
  std::string val = evaluate(expr.value);
  m_environment.assign(expr.name, val);
  Return(val);
}

void Interpreter::visit(Literal& expr) { Return(expr.value); }

void Interpreter::visit(Logical& expr) {
  std::string left = evaluate(expr.left);

  if (expr.op.type == Token::TokenType::OR) {
    if (isTruthy(left)) Return(left);
  } else if (!isTruthy(left))
    Return(left);

  Return(evaluate(expr.right));
}

void Interpreter::visit(Unary& expr) {
  std::string right = evaluate(expr.right);

  if (expr.op.type == Token::TokenType::BANG) {
    Return(std::to_string(!isTruthy(right)));
  } else if (expr.op.type == Token::TokenType::MINUS) {
    checkNumberOperand(expr.op, right);
    Return(std::to_string(-std::stoi(right)));
  }
}

void Interpreter::visit(Variable& expr) {
  Return(m_environment.get(expr.name));
}

void Interpreter::visit(Grouping& expr) { Return(evaluate(expr.expression)); }

void Interpreter::visit(Binary& expr) {
  std::string left = evaluate(expr.left);
  std::string right = evaluate(expr.right);

  switch (expr.op.type) {
    case Token::TokenType::GREATER:
      checkNumberOrStringOperands(expr.op, left, right);
      if (isNumber(left))
        Return(std::stoi(left) > std::stoi(right) ? "true" : "false");
      else
        Return(left > right ? "true" : "false");
      break;
    case Token::TokenType::GREATER_EQUAL:
      checkNumberOrStringOperands(expr.op, left, right);
      if (isNumber(left))
        Return(std::stoi(left) >= std::stoi(right) ? "true" : "false");
      else
        Return(left >= right ? "true" : "false");
      break;
    case Token::TokenType::LESS:
      checkNumberOrStringOperands(expr.op, left, right);
      if (isNumber(left))
        Return(std::stoi(left) < std::stoi(right) ? "true" : "false");
      else
        Return(left < right ? "true" : "false");
      break;
    case Token::TokenType::LESS_EQUAL:
      checkNumberOrStringOperands(expr.op, left, right);
      if (isNumber(left))
        Return(std::stoi(left) <= std::stoi(right) ? "true" : "false");
      else
        Return(left <= right ? "true" : "false");
      break;
    case Token::TokenType::MINUS:
      checkNumberOperands(expr.op, left, right);
      Return(std::to_string(std::stoi(left) - std::stoi(right)));
      break;
    case Token::TokenType::BANG_EQUAL:
      Return(left != right ? "true" : "false");
      break;
    case Token::TokenType::EQUAL_EQUAL:
      Return(left == right ? "true" : "false");
      break;
    case Token::TokenType::PLUS:
      if (isNumber(left) && isNumber(right)) {
        Return(std::to_string(std::stoi(left) + std::stoi(right)));
      } else {
        Return(left + right);
      }
      break;
    case Token::TokenType::SLASH:
      checkNumberOperands(expr.op, left, right);
      Return(std::to_string(std::stoi(left) / std::stoi(right)));
      break;
    case Token::TokenType::STAR:
      checkNumberOperands(expr.op, left, right);
      Return(std::to_string(std::stoi(left) * std::stoi(right)));
      break;
  }
}

void Interpreter::visit(Call& expr) {
  const std::string callee = evaluate(expr.callee);

  std::vector<std::string> arguments;
  for (Expr* arg : expr.arguments) {
    arguments.push_back(evaluate(arg));
  }

  Return(m_environment.getFunction(callee)->call(this, arguments));
}

void Interpreter::interpret(std::vector<Stmt*> statements) {
  try {
    for (Stmt* stmt : statements) {
      execute(stmt);
    }
  } catch (std::runtime_error e) {
    return;
  }
}

void Interpreter::visit(Block& stmt) {
  executeBlock(stmt.statements, PyInterpreter::Environment(m_environment));
}

void Interpreter::visit(IfElseBlock& stmt) {
  executeIfElseBlock(stmt.statements);
}

void Interpreter::visit(Expression& stmt) { evaluate(stmt.expression); }

void Interpreter::visit(Function& stmt) {
  PyFunction* function = new PyFunction(stmt);
  m_environment.assignFunction(stmt.name, function);
}

void Interpreter::visit(If& stmt) {
  if (isTruthy(evaluate(stmt.condition))) {
    execute(stmt.thenBranch);
  } else if (stmt.elseBranch != nullptr) {
    execute(stmt.elseBranch);
  }
}

void Interpreter::visit(ReturnStmt& stmt) {
  std::string value = "";
  if (stmt.value != nullptr) value = evaluate(stmt.value);

  throw new ReturnObj(value);
}

void Interpreter::visit(Print& stmt) {
  for (Expr* expr : stmt.expressions) {
    std::cout << evaluate(expr) << " ";
  }
  std::cout << std::endl;
}

void Interpreter::visit(Var& stmt) {
  std::string val = "";
  if (stmt.initializer != nullptr) {
    val = evaluate(stmt.initializer);
  }
  m_environment.assign(stmt.name, val);
}

void Interpreter::executeBlock(std::vector<Stmt*> stmts,
                               const Environment& env) {
  Environment prev = m_environment;
  m_environment = env;
  for (Stmt* stmt : stmts) {
    execute(stmt);
  }
  m_environment = prev;
  for (Stmt* stmt : stmts) {
    delete stmt;
  }
}

void Interpreter::executeIfElseBlock(std::vector<Stmt*> stmts) {
  for (Stmt* stmt : stmts) {
    execute(stmt);
  }

  for (Stmt* stmt : stmts) {
    delete stmt;
  }
}

void Interpreter::checkNumberOperand(Token op, std::string operand) {
  if (isNumber(operand)) return;
  throw std::runtime_error("Line " + std::to_string(op.line) +
                           ": Operand must be a number!");
}

void Interpreter::checkNumberOperands(Token op, std::string left,
                                      std::string right) {
  if (isNumber(left) && isNumber(right)) return;
  throw std::runtime_error("Line " + std::to_string(op.line) +
                           ": Operands must be numbers!");
}

void Interpreter::checkNumberOrStringOperands(Token op, std::string left,
                                              std::string right) {
  if (isNumber(left) && isNumber(right)) return;
  if (!isNumber(left) && !isNumber(right)) return;
  throw std::runtime_error("Line " + std::to_string(op.line) +
                           ": Operands must have matching types!");
}

bool Interpreter::isNumber(std::string str) const {
  for (const auto& c : str) {
    if (!isdigit(c) && c != '-') return false;
  }
  return true;
}