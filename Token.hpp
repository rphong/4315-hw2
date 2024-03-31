#pragma once

#include <string>

namespace PyInterpreter {

class Token {
 public:
  enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    COLON,
    SLASH,
    STAR,
    POUND,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,
    INDENTATION,

    // Keywords.
    AND,
    DEF,
    ELSE,
    FALSE,
    GLOBAL,
    IF,
    NONE,
    NOT,
    OR,
    RETURN,
    TRUE,
    NUL,
    PRINT,

    ENDOFFILE
  };
  Token(const TokenType& type, const std::string& lexeme,
        const int& line);

  const TokenType type;
  const std::string lexeme;
  const int line;
};
}  // namespace PyInterpreter
