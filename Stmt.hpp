#pragma once

#include <string>
#include <vector>

#include "Token.hpp"
#include "Expr.hpp"

#define MAKE_VISITABLE_STMT \
  void accept(Stmt::Visitor& vis) override { vis.visit(*this); }

namespace PyInterpreter {
class Block;
class IfElseBlock;
class Expression;
class ReturnStmt;
class Function;
class If;
class Print;
class Var;

class Stmt {
 public:
  class Visitor {
   public:
    virtual void visit(Block& stmt) = 0;
    virtual void visit(IfElseBlock& stmt) = 0;
    virtual void visit(Expression& stmt) = 0;
    virtual void visit(ReturnStmt& stmt) = 0;
    virtual void visit(Function& stmt) = 0;
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

class IfElseBlock : public Stmt {
 public:
  IfElseBlock(std::vector<Stmt*> stmts) : statements(stmts) {}
  MAKE_VISITABLE_STMT

  std::vector<Stmt*> statements;
};

class Expression : public Stmt {
 public:
  Expression(Expr* expr) : expression(expr) {}
  MAKE_VISITABLE_STMT

  Expr* expression;
};

class ReturnStmt : public Stmt {
 public:
  ReturnStmt(Token k, Expr* val) : keyword(k), value(val) {}
  MAKE_VISITABLE_STMT

  Token keyword;
  Expr* value;
};

class Function : public Stmt {
 public:
  Function(Token n, std::vector<Token> p, std::vector<Stmt*> b)
      : name(n), parameters(p), body(b) {}
  MAKE_VISITABLE_STMT

  Token name;
  std::vector<Token> parameters;
  std::vector<Stmt*> body;
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