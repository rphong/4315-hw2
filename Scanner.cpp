#include "Scanner.hpp"

using namespace PyInterpreter;

Scanner::Scanner(std::string source) : m_source(source){};

std::vector<Token> Scanner::scanTokens() {
  while (!isAtEnd()) {
    m_start = m_current;
    scanToken();
  }
  m_tokens.emplace_back(Token::TokenType::ENDOFFILE, "", m_line);
  return m_tokens;
}

void Scanner::scanToken() {
  const char c = advance();
  switch (c) {
    case '(':
      addToken(Token::TokenType::LEFT_PAREN);
      break;
    case ')':
      addToken(Token::TokenType::RIGHT_PAREN);
      break;
    case '{':
      addToken(Token::TokenType::LEFT_BRACE);
      break;
    case '}':
      addToken(Token::TokenType::RIGHT_BRACE);
      break;
    case ',':
      addToken(Token::TokenType::COMMA);
      break;
    case '.':
      addToken(Token::TokenType::DOT);
      break;
    case '-':
      addToken(Token::TokenType::MINUS);
      break;
    case '+':
      addToken(Token::TokenType::PLUS);
      break;
    case ':':
      addToken(Token::TokenType::COLON);
      break;
    case '*':
      addToken(Token::TokenType::STAR);
      break;
    case '#':
      comment();
      break;
    case '!':
      addToken(match('=') ? Token::TokenType::BANG_EQUAL
                          : Token::TokenType::BANG);
      break;
    case '=':
      addToken(match('=') ? Token::TokenType::EQUAL_EQUAL
                          : Token::TokenType::EQUAL);
      break;
    case '<':
      addToken(match('=') ? Token::TokenType::LESS_EQUAL
                          : Token::TokenType::LESS);
      break;
    case '>':
      addToken(match('=') ? Token::TokenType::GREATER_EQUAL
                          : Token::TokenType::GREATER);
      break;
    case '/':
      if (match('/')) {
        while (peek() != '\n' && !isAtEnd()) advance();
      } else {
        addToken(Token::TokenType::SLASH);
      }
      break;
    case ' ':
    case '\r':
    case '\t':
      // Ignore whitespace
      break;
    case '\n':
      m_line++;
      indentation();
      break;
    case '"':
      string();
      break;
    default:
      if (isDigit(c)) {
        number();
      } else if (isAlpha(c)) {
        identifier();
      } else {
        std::cout << "Token not recognized on line " << m_line << std::endl;
      }
      break;
  }
}

void Scanner::addToken(Token::TokenType type) {
  std::string text = m_source.substr(m_start, m_current - m_start);
  m_tokens.emplace_back(type, text, m_line);
}

void Scanner::addToken(Token::TokenType type, std::string literal) {
  m_tokens.emplace_back(type, literal, m_line);
}

void Scanner::indentation() {
  while (peek() == ' ') advance();
  addToken(Token::TokenType::INDENTATION,
           m_source.substr(m_start + 1, m_current - m_start - 1));
}

void Scanner::identifier() {
  while (isAlphaNumeric(peek())) advance();

  std::string text = m_source.substr(m_start, m_current - m_start);
  Token::TokenType type;
  try {
    type = m_keywords.at(text);
  } catch (std::out_of_range err) {
    type = Token::TokenType::IDENTIFIER;
  }
  addToken(type);
}

void Scanner::string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') m_line++;
    advance();
  }

  if (isAtEnd()) {
    std::cout << "Undetermined string" << std::endl;
    return;
  }

  // Closing "
  advance();

  // Trim quotes
  std::string val = m_source.substr(m_start + 1, m_current - m_start - 2);
  addToken(Token::TokenType::STRING, val);
}

void Scanner::number() {
  while (isDigit(peek())) advance();
  addToken(Token::TokenType::NUMBER,
           m_source.substr(m_start, m_current - m_start));
}

void Scanner::comment() {
  while(peek() != '\n') advance();
}

bool Scanner::match(char expected) {
  if (isAtEnd()) return false;
  if (m_source[m_current] != expected) return false;

  m_current++;
  return true;
}
