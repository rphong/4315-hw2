#include "Parser.hpp"

using namespace PyInterpreter;

std::vector<Stmt*> Parser::parse() {
  std::vector<Stmt*> statements;
  while (peek().type == Token::TokenType::INDENTATION && !isAtEnd()) advance();

  while (!isAtEnd()) {
    statements.push_back(declaration());
    while (peek().type == Token::TokenType::INDENTATION && !isAtEnd()) advance();
  }
  return statements;
}

Stmt* Parser::declaration() {
  try {
    if (peek().type == Token::TokenType::IDENTIFIER) {
      return varDeclaration();
    }
    return statement();
  } catch (std::runtime_error e) {
    std::cout << e.what() << std::endl;
    synchronize();
    return nullptr;
  }
}

Stmt* Parser::statement() {
  if (match({Token::TokenType::IF})) return ifStatement();
  if (match({Token::TokenType::PRINT})) return printStatement();
  return expressionStatement();
}

Stmt* Parser::ifStatement() {
  Expr* condition = expression();
  consume(Token::TokenType::COLON, "Expect colon after condition");
  indentation();

  Stmt* thenBranch = new Block(block(m_indentation));
  Stmt* elseBranch = nullptr;
  if (match({Token::TokenType::ELSE})) {
    consume(Token::TokenType::COLON, "Expect colon after else");
    indentation();
    elseBranch = new Block(block(m_indentation));
  }

  return new If(condition, thenBranch, elseBranch);
}

Stmt* Parser::expressionStatement() {
  Expr* expr = expression();
  indentation();
  return new Expression(expr);
}

Stmt* Parser::printStatement() {
  consume(Token::TokenType::LEFT_PAREN, "Expect '(' after print");

  std::vector<Expr*> expressions;
  expressions.push_back(expression());
  while (peek().type != Token::TokenType::RIGHT_PAREN && !isAtEnd()) {
    consume(Token::TokenType::COMMA, "Expect ',' after argument");
    expressions.push_back(expression());
  }
  consume(Token::TokenType::RIGHT_PAREN, "Expect ) at end of argument list");
  indentation();
  return new Print(expressions);
}

Stmt* Parser::varDeclaration() {
  Token name = consume(Token::TokenType::IDENTIFIER, "Expect variable name.");

  Expr* initializer = nullptr;
  if (match({Token::TokenType::EQUAL})) {
    initializer = expression();
  }
  indentation();
  return new Var(name, initializer);
}

std::vector<Stmt*> Parser::block(int indentation) {
  std::vector<Stmt*> statements;

  while (m_indentation >= indentation && !isAtEnd()) {
    statements.push_back(declaration());
  }

  return statements;
}

void Parser::indentation() {
  if (isAtEnd()) return;
  Token ind = consume(Token::TokenType::INDENTATION, "Expect indentation.");
  m_indentation = ind.lexeme.size();
}

Expr* Parser::expression() { return assignment(); }

Expr* Parser::assignment() {
  Expr* expr = orLogic();

  if (match({Token::TokenType::EQUAL})) {
    Token equals = previous();
    Expr* value = assignment();

    if (dynamic_cast<Variable*>(expr)) {
      Token name = ((Variable*)expr)->name;
      return new Assign(name, value);
    }

    throw std::runtime_error("Invalid assignment target");
  }

  return expr;
}

Expr* Parser::orLogic() {
  Expr* expr = andLogic();

  while (match({Token::TokenType::OR})) {
    Token op = previous();
    Expr* right = andLogic();
    expr = new Logical(expr, op, right);
  }

  return expr;
}

Expr* Parser::andLogic() {
  Expr* expr = equality();

  while (match({Token::TokenType::AND})) {
    Token op = previous();
    Expr* right = equality();
    expr = new Logical(expr, op, right);
  }

  return expr;
}

Expr* Parser::equality() {
  Expr* expr = comparison();

  while (match({Token::TokenType::BANG_EQUAL, Token::TokenType::EQUAL_EQUAL})) {
    Token op = previous();
    Expr* right = comparison();
    expr = new Binary(expr, op, right);
  }

  return expr;
}

Expr* Parser::comparison() {
  Expr* expr = term();

  while (match({Token::TokenType::GREATER, Token::TokenType::GREATER_EQUAL,
                Token::TokenType::LESS, Token::TokenType::LESS_EQUAL})) {
    Token op = previous();
    Expr* right = term();
    expr = new Binary(expr, op, right);
  }

  return expr;
}

Expr* Parser::term() {
  Expr* expr = factor();

  while (match({Token::TokenType::MINUS, Token::TokenType::PLUS})) {
    Token op = previous();
    Expr* right = factor();
    expr = new Binary(expr, op, right);
  }

  return expr;
}

Expr* Parser::factor() {
  Expr* expr = unary();

  while (match({Token::TokenType::SLASH, Token::TokenType::STAR})) {
    Token op = previous();
    Expr* right = unary();
    expr = new Binary(expr, op, right);
  }

  return expr;
}

Expr* Parser::unary() {
  if (match({Token::TokenType::BANG, Token::TokenType::MINUS})) {
    Token op = previous();
    Expr* right = unary();
    return new Unary(op, right);
  }

  return primary();
}

Expr* Parser::primary() {
  if (match({Token::TokenType::FALSE})) {
    return new Literal("false");
  }
  if (match({Token::TokenType::TRUE})) {
    return new Literal("true");
  }
  if (match({Token::TokenType::NUL})) {
    return new Literal("null");
  }
  if (match({Token::TokenType::NUMBER, Token::TokenType::STRING})) {
    return new Literal(previous().lexeme);
  }
  if (match({Token::TokenType::IDENTIFIER})) {
    return new Variable(previous());
  }
  if (match({Token::TokenType::LEFT_PAREN})) {
    Expr* expr = expression();
    consume(Token::TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return new Grouping(expr);
  }

  throw std::runtime_error("Expect expression.");
}

bool Parser::match(std::vector<Token::TokenType> types) {
  for (const auto& type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

Token Parser::consume(Token::TokenType type, std::string message) {
  if (check(type)) return advance();
  throw std::runtime_error(message);
}

bool Parser::check(Token::TokenType type) const {
  if (isAtEnd()) return false;
  return peek().type == type;
}

Token Parser::advance() {
  if (!isAtEnd()) m_current++;
  return previous();
}

void Parser::synchronize() {
  advance();
  while (!isAtEnd()) {
    if (previous().type == Token::TokenType::INDENTATION) return;
  }
}