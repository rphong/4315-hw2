#pragma once

#include <string>
#include <vector>

#include "Token.hpp"
#include "Expr.hpp"

#define MAKE_VISITABLE_STMT \
  void accept(Stmt::Visitor& vis) override { vis.visit(*this); }

namespace PyInterpreter {
class Block;
class Expression;
class If;
class Print;
class Var;

class Stmt {
 public:
  class Visitor {
   public:
    virtual void visit(Block& stmt) = 0;
    virtual void visit(Expression& stmt) = 0;
    virtual void visit(If& stmt) = 0;
    virtual void visit(Print& stmt) = 0;
    virtual void visit(Var& stmt) = 0;
  };

  virtual void accept(Visitor& visitor) = 0;
};

class Block : public Stmt {
 public:
  Block(std::vector<Stmt*> stmts) : statements(stmts) {}
  MAKE_VISITABLE_STMT

  std::vector<Stmt*> statements;
};

class Expression : public Stmt {
 public:
  Expression(Expr* expr) : expression(expr) {}
  MAKE_VISITABLE_STMT

  Expr* expression;
};

class If : public Stmt {
 public:
  If(Expr* cond, Stmt* thenBr, Stmt* elseBr)
      : condition(cond), thenBranch(thenBr), elseBranch(elseBr) {}
  MAKE_VISITABLE_STMT

  Expr* condition;
  Stmt* thenBranch;
  Stmt* elseBranch;
};

class Print : public Stmt {
 public:
  Print(std::vector<Expr*> expr) : expressions(expr){};
  MAKE_VISITABLE_STMT
  
  std::vector<Expr*> expressions;
};

class Var : public Stmt {
 public:
  Var(Token n, Expr* init) : name(n), initializer(init) {}
  MAKE_VISITABLE_STMT

  Token name;
  Expr* initializer;
};
}  // namespace PyInterpreter