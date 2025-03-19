#include "Lexer.hpp"
#include "Logger.hpp"

namespace DecafScanning {

// Constructor to initialize the lexer with the source string
Lexer::Lexer(std::string src) : m_src(std::move(src)) {}

// Tokenize the source string into a vector of tokens
std::vector<Token> Lexer::tokenize() {
  std::vector<Token> tokens; // Vector to store the tokens
  std::string buffer;        // Buffer to accumulate characters for multi-character tokens

  // Loop through the source string until all characters are processed
  while (peek().has_value()) {
    // If the current character is a letter or underscore, it might be an identifier or keyword
    if (std::isalpha(peek().value()) || peek().value() == '_') {
      buffer.push_back(consume()); // Add the character to the buffer

      // Continue adding characters to the buffer as long as they are alphanumeric or underscores
      while (peek().has_value() && (std::isalnum(peek().value()) || peek().value() == '_')) {
        buffer.push_back(consume());
      }

      std::vector<std::string> reserved = {
        "for",
        "callout",
        "class",
        "interface",
        "extends",
        "implements",
        "new",
        "this",
        "string",
        "float",
        "double",
        "null"
      };
      
      // Check for specific keywords and add the corresponding token
      std::size_t startPosition = m_index - buffer.length(); // Get start of identifier or keyword
      if (buffer == "def") {
        tokens.push_back({ .type = TokenType::DEF, .position = startPosition, .length = buffer.length() });
      } else if (buffer == "if") {
        tokens.push_back({ .type = TokenType::IF, .position = startPosition, .length = buffer.length() });
      } else if (buffer == "else") {
        tokens.push_back({ .type = TokenType::ELSE, .position = startPosition, .length = buffer.length() });
      } else if (buffer == "while") {
        tokens.push_back({ .type = TokenType::WHILE, .position = startPosition, .length = buffer.length() });
      } else if (buffer == "return") {
        tokens.push_back({ .type = TokenType::RETURN, .position = startPosition, .length = buffer.length() });
      } else {
        if (std::find(reserved.begin(), reserved.end(), buffer) != reserved.end()) { // Warn  about reserved keywords being used as identifiers
          DecafLogger::Logger::logMessage(DecafLogger::LogType::WARNING, DecafLogger::stringFormat("Reserved keyword '%s' used as identifier! This can cause issues in later versions of the compiler.", buffer.c_str()), startPosition);
        }

        // If the buffer does not match any keywords, treat it as an identifier
        tokens.push_back({ .type = TokenType::IDENTIFIER, .value = buffer });
      }

      buffer.clear(); // Clear the buffer for the next token
    }

    // If the current character is a digit, it might be a number
    else if (std::isdigit(peek().value())) {
      buffer.push_back(consume()); // Add the digit to the buffer

      // Continue adding digits to the buffer
      while (peek().has_value() && std::isdigit(peek().value())) {
        buffer.push_back(consume());
      }

      // Add the number token with the value from the buffer
      tokens.push_back({ .type = TokenType::NUMBER, .value = buffer });
      buffer.clear(); // Clear the buffer for the next token
    }

    // Handle various single-character tokens like parentheses, brackets, and operators
    else if (peek().value() == '(') {
      consume();
      tokens.push_back({ .type = TokenType::OPEN_PAREN, .position = m_index });
    } else if (peek().value() == ')') {
      consume();
      tokens.push_back({ .type = TokenType::CLOSE_PAREN, .position = m_index });
    } else if (peek().value() == '{') {
      consume();
      tokens.push_back({ .type = TokenType::OPEN_CURLY, .position = m_index });
    } else if (peek().value() == '}') {
      consume();
      tokens.push_back({ .type = TokenType::CLOSE_CURLY, .position = m_index });
    } else if (peek().value() == '[') {
      consume();
      tokens.push_back({ .type = TokenType::OPEN_BRACKET, .position = m_index });
    } else if (peek().value() == ']') {
      consume();
      tokens.push_back({ .type = TokenType::CLOSE_BRACKET, .position = m_index  });
    }

    // Handle two-character operators
    else if (peek().value() == '=') {
      consume();
      if (peek().value() == '=') {
        consume();
        tokens.push_back({ .type = TokenType::EQUAL_EQUAL, .position = m_index, .length = 2  });
      } else {
        tokens.push_back({ .type = TokenType::EQUAL, .position = m_index });
      }
    } else if (peek().value() == '<') {
      consume();
      if (peek().value() == '=') {
        consume();
        tokens.push_back({ .type = TokenType::LESS_THAN_EQUAL, .position = m_index, .length = 2 });
      } else {
        tokens.push_back({ .type = TokenType::LESS_THAN, .position = m_index });
      }
    } else if (peek().value() == '>') {
      consume();
      if (peek().value() == '=') {
        consume();
        tokens.push_back({ .type = TokenType::GREATER_THAN_EQUAL });
      } else {
        tokens.push_back({ .type = TokenType::GREATER_THAN });
      }
    }

    // Handle single-character operators and punctuation
    else if (peek().value() == '+') {
      consume();
      tokens.push_back({ .type = TokenType::PLUS });
    } else if (peek().value() ==  '-') {
      consume();
      tokens.push_back({ .type = TokenType::MINUS });
    } else if (peek().value() ==  '*') {
      consume();
      tokens.push_back({ .type = TokenType::TIMES });
    } else if (peek().value() ==  '/') {
      consume();
      tokens.push_back({ .type = TokenType::DIVIDE });
    } else if (peek().value() == ',') {
      consume();
      tokens.push_back({ .type = TokenType::COMMA });
    } else if (peek().value() == ';') {
      consume();
      tokens.push_back({ .type = TokenType::SEMICOLON });
    }

    // Skip any comments (beginning with "#")
    else if (peek().value() == '#') {
      while (peek().has_value() && peek().value() != '\n' && peek().value() != '\r') {
        consume();
      }
    }
    // Skip whitespace characters
    else if (std::isspace(peek().value())) {
      consume();
    }

    // Handle unrecognized characters
    else {
      DecafLogger::Logger::logMessage(DecafLogger::LogType::ERROR, DecafLogger::stringFormat("Unrecognized character: %c", peek().value()), m_index);
    }
  }

  m_index = 0;
  return tokens; // Return the vector of tokens
}

// Peek at the character at a given offset from the current position
std::optional<char> Lexer::peek(int offset) {
  if (m_index + offset >= m_src.length()) {
    return {}; // Return an empty optional if the offset is out of bounds
  } else {
    return m_src.at(m_index + offset); // Return the character at the given offset
  }
}

// Consume the current character and advance the index
char Lexer::consume() {
  return m_src.at(m_index++); // Return the current character and increment the index
}

}