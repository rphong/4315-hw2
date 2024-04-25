#include "Parser.hpp"

using namespace PyInterpreter;

std::vector<Stmt*> Parser::parse() {
  std::vector<Stmt*> statements;
  clearEmptyLines();
  while (!isAtEnd()) {
    statements.push_back(declaration());
    clearEmptyLines();
  }
  return statements;
}

Stmt* Parser::declaration() {
  indentation();
  try {
    if (match({Token::TokenType::DEF})) {
      return function("function");
    }
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
  if (match({Token::TokenType::RETURN})) return returnStatement();
  if (match({Token::TokenType::PRINT})) return printStatement();
  return expressionStatement();
}

Stmt* Parser::expressionStatement() {
  Expr* expr = expression();
  return new Expression(expr);
}

Stmt* Parser::function(const std::string& kind) {
  Token name =
      consume(Token::TokenType::IDENTIFIER, "Expect " + kind + " name.");
  consume(Token::TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
  std::vector<Token> parameters;
  if (!check(Token::TokenType::RIGHT_PAREN)) {
    do {
      if (parameters.size() >= 255) {
        throw std::runtime_error("Can't have more than 255 parameters");
      }
      parameters.push_back(
          consume(Token::TokenType::IDENTIFIER, "Expect parameter name."));
    } while (match({Token::TokenType::COMMA}));
  }
  consume(Token::TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

  consume(Token::TokenType::COLON, "Expect ':' before " + kind + " body.");
  m_indentation = peek().lexeme.size();

  std::vector<Stmt*> body = block(m_indentation);
  return new Function(name, parameters, body);
}

Stmt* Parser::ifStatement() {
  Expr* condition = expression();
  consume(Token::TokenType::COLON, "Expect colon after condition");
  clearEmptyLines();
  int localIndentation = m_indentation;
  m_indentation = peek().lexeme.size();

  Stmt* thenBranch = new IfElseBlock(block(m_indentation));
  Stmt* elseBranch = nullptr;
  clearEmptyLines();
  if(next().type == Token::TokenType::ELSE && peek().lexeme.size() == localIndentation) {
    indentation();
    consume(Token::TokenType::ELSE, "Expect else");
    consume(Token::TokenType::COLON, "Expect colon after else");
    clearEmptyLines();
    m_indentation = peek().lexeme.size();
    elseBranch = new IfElseBlock(block(m_indentation));
  }

  return new If(condition, thenBranch, elseBranch);
}

Stmt* Parser::returnStatement() {
  Token keyword = previous();
  Expr* value = nullptr;
  if (!check(Token::TokenType::INDENTATION)) {
    value = expression();
  }

  return new ReturnStmt(keyword, value);
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
  return new Print(expressions);
}

Stmt* Parser::varDeclaration() {
  Token name = consume(Token::TokenType::IDENTIFIER, "Expect variable name.");

  Expr* initializer = nullptr;
  if (match({Token::TokenType::EQUAL})) {
    initializer = expression();
  }
  return new Var(name, initializer);
}

std::vector<Stmt*> Parser::block(int indentation) {
  std::vector<Stmt*> statements;

  while (m_indentation >= indentation && !isAtEnd()) {
    statements.push_back(declaration());
    clearEmptyLines();
    m_indentation = peek().lexeme.size();
  }

  return statements;
}

void Parser::indentation() {
  if (isAtEnd()) return;
  Token ind = consume(Token::TokenType::INDENTATION, "Expect indentation.");
  m_indentation = ind.lexeme.size();
}

void Parser::clearEmptyLines() {
  while ((next().type == Token::TokenType::INDENTATION))
    advance();
  if(next().type == Token::TokenType::ENDOFFILE) advance();
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

  return call();
}

Expr* Parser::call() {
  Expr* expr = primary();

  while (true) {
    if (match({Token::TokenType::LEFT_PAREN})) {
      expr = finishCall(expr);
    } else {
      break;
    }
  }

  return expr;
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

Expr* Parser::finishCall(Expr* callee) {
  std::vector<Expr*> arguments;
  if (!check(Token::TokenType::RIGHT_PAREN)) {
    do {
      arguments.push_back(expression());
    } while (match({Token::TokenType::COMMA}));
  }

  Token paren =
      consume(Token::TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

  return new Call(callee, paren, arguments);
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