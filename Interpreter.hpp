#pragma once

#include <string>
#include <stdexcept>
#include <iostream>

#include "Environment.hpp"
#include "PyCallable.hpp"
#include "PyFunction.hpp"
#include "Scanner.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "VisitorReturnVal.hpp"
#include "ReturnObj.hpp"

namespace PyInterpreter {
class Environment;
class Interpreter : public VisitorReturnVal<Interpreter, Expr*, std::string>,
                    public Expr::Visitor,
                    public Stmt::Visitor {
 public:
  void visit(Assign& expr);
  void visit(Literal& expr);
  void visit(Logical& expr);
  void visit(Unary& expr);
  void visit(Variable& expr);
  void visit(Grouping& expr);
  void visit(Binary& expr);
  void visit(Call& expr);

  void visit(Block& stmt);
  void visit(IfElseBlock& stmt);
  void visit(Expression& stmt);
  void visit(Function& stmt);
  void visit(If& stmt);
  void visit(ReturnStmt& stmt);
  void visit(Print& stmt);
  void visit(Var& stmt);

  void interpret(std::vector<Stmt*> statements);

  void execute(Stmt* stmt) { stmt->accept(*this); }
  void executeBlock(std::vector<Stmt*> stmts, const Environment& env);

  Environment getGlobals() {
    return m_environment;
  }

 private:
  std::string evaluate(Expr* expr) { return GetValue(expr); }
  void executeIfElseBlock(std::vector<Stmt*> stmts);
  void checkNumberOperand(Token op, std::string operand);
  void checkNumberOperands(Token op, std::string left, std::string right);
  void checkNumberOrStringOperands(Token op, std::string left,
                                   std::string right);
  bool isTruthy(std::string val) const {
    return !(val == "") && !(val == "0") && !(val == "null") &&
           !(val == "false");
  }
  bool isNumber(std::string str) const;

  static Environment m_environment;
};
}  // namespace PyInterpreter