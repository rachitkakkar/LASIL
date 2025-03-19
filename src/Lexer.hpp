#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <iomanip> // For std::setw()
#include <string>
#include <vector>
#include <optional>
#include <algorithm>

namespace DecafScanning {

enum class TokenType {    
  DEF,
  IF,
  ELSE,
  WHILE,
  RETURN,
  BREAK,
  CONTINUE,
  INT,
  BOOL,
  VOID,
  TRUE,
  FALSE,

  OPEN_PAREN,
  CLOSE_PAREN,
  OPEN_CURLY,
  CLOSE_CURLY,
  OPEN_BRACKET,
  CLOSE_BRACKET,

  EQUAL,
  EQUAL_EQUAL,
  LESS_THAN,
  GREATER_THAN,
  LESS_THAN_EQUAL,
  GREATER_THAN_EQUAL,
  PLUS,
  MINUS,
  TIMES,
  DIVIDE,
  COMMA,
  SEMICOLON,

  NUMBER,
  IDENTIFIER
};

struct Token {
  TokenType type;
  std::size_t position;
  std::optional<std::size_t> length;
  std::optional<std::string> value {};
};

class Lexer {
public:
  explicit Lexer(std::string src);
  std::vector<Token> tokenize();

private:
  std::string m_src;
  std::size_t m_index = 0;

  std::optional<char> peek(int offset = 0);
  char consume();
};

}

#endif // LEXER_H