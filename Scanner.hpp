#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "Token.hpp"

namespace PyInterpreter {
class Scanner {
 public:
  Scanner(std::string source);
  std::vector<Token> scanTokens();

 private:
  void scanToken();
  void addToken(Token::TokenType type);
  void addToken(Token::TokenType type, std::string literal);
  void identifier();
  void indentation();
  void string();
  void number();
  void comment();
  bool match(char expected);
  char advance() { return m_source[m_current++]; }
  char peek() const {
    if (isAtEnd()) return '\0';
    return m_source[m_current];
  }
  bool isAtEnd() const { return m_current >= m_source.length(); }
  constexpr bool isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }
  constexpr bool isDigit(char c) const { return c >= '0' && c <= '9'; }
  constexpr bool isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

  std::vector<Token> m_tokens;
  const std::string m_source;
  int m_start = 0;
  int m_current = 0;
  int m_line = 1;

  const std::unordered_map<std::string, Token::TokenType> m_keywords{
      {"and", Token::TokenType::AND},
      {"def", Token::TokenType::DEF},
      {"else", Token::TokenType::ELSE},
      {"false", Token::TokenType::FALSE},
      {"global", Token::TokenType::GLOBAL},
      {"if", Token::TokenType::IF},
      {"none", Token::TokenType::NONE},
      {"not", Token::TokenType::NOT},
      {"or", Token::TokenType::OR},
      {"return", Token::TokenType::RETURN},
      {"true", Token::TokenType::TRUE},
      {"print", Token::TokenType::PRINT}};
};
}  // namespace PyInterpreter