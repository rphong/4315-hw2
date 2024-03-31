#include "Token.hpp"

using namespace PyInterpreter;

Token::Token(const TokenType& type, const std::string& lexeme, const int& line)
    : type(type), lexeme(lexeme), line(line) {}