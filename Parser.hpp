#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>

#include "./Expr.hpp"
#include "./Stmt.hpp"
#include "./Token.hpp"

namespace PyInterpreter {
class Parser {
 public:
  Parser(const std::vector<Token> tokens) : m_tokens(tokens) {}
  std::vector<Stmt*> parse();

 private:
  Stmt* declaration();
  Expr* expression();
  Expr* assignment();
  Expr* orLogic();
  Expr* andLogic();
  Expr* equality();
  Expr* comparison();
  Expr* term();
  Expr* factor();
  Expr* unary();
  Expr* primary();

  Stmt* statement();
  Stmt* expressionStatement();
  Stmt* ifStatement();
  Stmt* printStatement();
  Stmt* varDeclaration();

  std::vector<Stmt*> block(int indentation);
  void indentation();

  bool match(std::vector<Token::TokenType> types);
  Token consume(Token::TokenType type, std::string message);
  bool check(Token::TokenType type) const;
  Token advance();
  bool isAtEnd() const { return peek().type == Token::TokenType::ENDOFFILE; }
  Token peek() const { return m_tokens[m_current]; }
  Token previous() const { return m_tokens[m_current - 1]; }
  void synchronize();

  const std::vector<Token> m_tokens;
  int m_current = 0;
  int m_indentation = 0;
};
}  // namespace PyInterpreter