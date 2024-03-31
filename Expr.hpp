#pragma once

#include <string>

#include "Token.hpp"

#define MAKE_VISITABLE_EXPR \
  void accept(Expr::Visitor& vis) override { vis.visit(*this); }

namespace PyInterpreter {
class Assign;
class Binary;
class Grouping;
class Literal;
class Logical;
class Unary;
class Variable;

class Expr {
 public:
  class Visitor {
   public:
    virtual void visit(Assign& expr) = 0;
    virtual void visit(Binary& expr) = 0;
    virtual void visit(Grouping& expr) = 0;
    virtual void visit(Literal& expr) = 0;
    virtual void visit(Logical& expr) = 0;
    virtual void visit(Unary& expr) = 0;
    virtual void visit(Variable& expr) = 0;
  };

  virtual void accept(Visitor& visitor) = 0;
};

class Assign : public Expr {
 public:
  Assign(Token n, Expr* val) : name(n), value(val) {}
  MAKE_VISITABLE_EXPR

  Token name;
  Expr* value;
};

class Binary : public Expr {
 public:
  Binary(Expr* l, Token o, Expr* r) : left(l), op(o), right(r) {}
  MAKE_VISITABLE_EXPR

  Expr* left;
  Token op;
  Expr* right;
};

class Grouping : public Expr {
 public:
  Grouping(Expr* expr) : expression(expr) {}
  MAKE_VISITABLE_EXPR

  Expr* expression;
};

class Literal : public Expr {
 public:
  Literal(std::string val) : value(val) {}
  MAKE_VISITABLE_EXPR

  std::string value;
};

class Logical : public Expr {
 public:
  Logical(Expr* l, Token o, Expr* r) : left(l), op(o), right(r) {}
  MAKE_VISITABLE_EXPR

  Expr* left;
  Token op;
  Expr* right;
};

class Unary : public Expr {
 public:
  Unary(Token o, Expr* r) : op(o), right(r) {}
  MAKE_VISITABLE_EXPR

  Token op;
  Expr* right;
};

class Variable : public Expr {
 public:
  Variable(Token n) : name(n) {}
  MAKE_VISITABLE_EXPR

  Token name;
};
}  // namespace PyInterpreter