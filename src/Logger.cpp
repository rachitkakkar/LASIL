#include "Logger.hpp"
#include "Lexer.hpp"
using namespace DecafScanning;
using namespace DecafParsing;

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

namespace DecafLogger {

std::string Logger::fileText;
std::vector<Line> Logger::lines;

void Logger::setFile(std::string fileText) {
  Logger::fileText = fileText;
  lines.clear();
  // Break file's text into an array of lines
  std::size_t currChar = 0;
  int lineNum = 1;
  std::string currLine;
  std::stringstream fileStream(Logger::fileText);
  while (std::getline(fileStream, currLine, '\n')) {
    std::size_t endChar = currChar + currLine.length();
    Logger::lines.push_back({ .content = currLine, .startPosition = currChar, .endPosition = endChar, .lineNumber = lineNum });
    currChar = endChar + 1;
    lineNum++;
  }
}

void Logger::logMessage(LogType type, const std::string& msg) {
  switch (type) {
    case LogType::ERROR:
      std::cout << ERROR_ESCAPE_SEQUENCE << std::endl;
      throw std::runtime_error(msg);
    case LogType::DEBUG_INFO:
      std::cout << DEBUG_ESCAPE_SEQUENCE <<  " " << msg << std::endl;
  }
}

void Logger::logMessage(LogType type, const std::string& msg, const Token& token) {
  Line currLine;
  bool lineFound = false;
  for (const Line& line : Logger::lines) {
    if (line.startPosition <= token.position && line.endPosition >= token.position) {
      currLine = line;
      lineFound = true;
      break;
    }
  }
  // Ensure position is within bounds
  if (!lineFound) {
    throw std::out_of_range("Position is out of the range of the file content.");
  }

  switch (type) {
    case LogType::ERROR:
      // Print the line number and the line
      std::cout << ERROR_ESCAPE_SEQUENCE << " Line " << currLine.lineNumber << std::endl;
      std::cout << currLine.content << std::endl;

      // Print spaces until the caret's position and then print '^'
      std::cout << std::string(token.position - currLine.startPosition, ' ') << "^" << std::endl;

      throw std::runtime_error(msg);
      break;
    
    case LogType::WARNING:
      // Print the line number and the line
      std::cout << WARNING_ESCAPE_SEQUENCE << " Line " << currLine.lineNumber << std::endl;
      std::cout << currLine.content << std::endl;

      // Print spaces until the caret's position and then print '^'
      std::cout << std::string(token.position - currLine.startPosition, ' ') << "^" << std::endl;
      std::cout << msg << std::endl;
      std::cout << std::endl;
      break;
  }
}

void Logger::logMessage(LogType type, const std::string& msg, std::size_t position) {
  Line currLine;
  bool lineFound = false;
  for (const Line& line : Logger::lines) {
    if (line.startPosition <= position && line.endPosition >= position) {
      currLine = line;
      lineFound = true;
      break;
    }
  }
  // Ensure position is within bounds
  if (!lineFound) {
    throw std::out_of_range("Position is out of the range of the file content.");
  }

  switch (type) {
    case LogType::ERROR:
      // Print the line number and the line
      std::cout << ERROR_ESCAPE_SEQUENCE << " Line " << currLine.lineNumber << std::endl;
      std::cout << currLine.content << std::endl;

      // Print spaces until the caret's position and then print '^'
      std::cout << std::string(position - currLine.startPosition, ' ') << "^" << std::endl;

      throw std::runtime_error(msg);
      break;
    
    case LogType::WARNING:
      // Print the line number and the line
      std::cout << WARNING_ESCAPE_SEQUENCE << " Line " << currLine.lineNumber << std::endl;
      std::cout << currLine.content << std::endl;

      // Print spaces until the caret's position and then print '^'
      std::cout << std::string(position - currLine.startPosition, ' ') << "^" << std::endl;
      std::cout << msg << std::endl;
      std::cout << std::endl;
      break;
  }
}

// void Logger::logMessage(LogType type, const std::string& msg, const std::string& fileText, std::size_t position) {
//   // Ensure position is within bounds
//   if (position >= fileText.length()) {
//     throw std::out_of_range("Position is out of the range of the file content.");
//   }

//   // Initialize the line number
//   size_t lineNumber = 1;

//   // Find the start of the line by searching for the nearest newline before the position
//   std::size_t start = fileText.rfind('\n', position);
//   if (start == std::string::npos) {
//     start = 0; // If no newline is found, start is the beginning of the string
//   } else {
//     start += 1; // Move to the character after the newline
//   }

//   // Count the line number by counting newlines up to the position
//   for (std::size_t i = 0; i < start; ++i) {
//     if (fileText[i] == '\n') {
//       ++lineNumber;
//     }
//   }

//   // Find the end of the line by searching for the nearest newline after the position
//   std::size_t end = fileText.find('\n', position);
//   if (end == std::string::npos) {
//     end = fileText.length();  // If no newline is found, end is the end of the string
//   }

//   // Find line and calculate the position of the caret '^'
//   std::string line = fileText.substr(start, end - start);
//   std::size_t caretPosition = position - fileText.rfind('\n', position) - 1;

//   switch (type) {
//     case LogType::ERROR:
//       // Print the line number and the line
//       std::cout << ERROR_ESCAPE_SEQUENCE << std::endl;
//       std::cout << " Line " << lineNumber << ": " << line << std::endl;

//       // Print spaces until the caret's position and then print '^'
//       std::cout << std::string(caretPosition + 6, ' ') << "^" << std::endl;

//       throw std::runtime_error(msg);
//       break;
    
//     case LogType::WARNING:
//       // Print the line number and the line
//       std::cout << WARNING_ESCAPE_SEQUENCE << std::endl;
//       std::cout << " Line " << lineNumber << ": " << line << std::endl;

//       // Print spaces until the caret's position and then print '^'
//       std::cout << std::string(caretPosition + 5, ' ') << "^" << std::endl;

//       std::cout << msg << std::endl;
//       std::cout << std::endl;
//       break;
//   }
// }

void Logger::displayASTExpr(int level, AST::Expr& expr) {
  if (typeid(expr) == typeid(AST::NumberExpr)) {
    AST::NumberExpr* num = dynamic_cast<AST::NumberExpr*>(&expr);
    std::cout << std::string(level * 3, ' ') << ((level != 0) ? "└" : "") <<  "number: " << num->value << std::endl;
  }
  else if (typeid(expr) == typeid(AST::VariableExpr)) {
    AST::VariableExpr* var = dynamic_cast<AST::VariableExpr*>(&expr);
    std::cout << std::string(level * 3, ' ') << ((level != 0) ? "└" : "") << "variable: " << var->name << std::endl;
  }
  else if (typeid(expr) == typeid(AST::BinaryExpr)) {
    AST::BinaryExpr* bin = dynamic_cast<AST::BinaryExpr*>(&expr);
    std::string op;
    switch(bin->op.type) {
      case TokenType::TIMES:
        op = "*";
        break;
      case TokenType::DIVIDE:
        op = "/";
        break;
      case TokenType::PLUS:
        op = "+";
        break;
      case TokenType::MINUS:
        op = "-";
        break;
      case TokenType::LESS_THAN:
        op = "<";
        break;
      case TokenType::GREATER_THAN:
        op = ">";
        break;
      case TokenType::LESS_THAN_EQUAL:
        op = "<=";
        break;
      case TokenType::GREATER_THAN_EQUAL:
        op = ">=";
        break;
      case TokenType::EQUAL_EQUAL:
        op = "==";
        break;
    }

    std::cout << std::string(level * 3, ' ') << ((level != 0) ? "└" : "") <<  "binary operation: " << op << std::endl;
    Logger::displayASTExpr(level+1, *bin->LHS);
    Logger::displayASTExpr(level+1, *bin->RHS);
  }
  else if (typeid(expr) == typeid(AST::CallExpr)) {
    AST::CallExpr* call = dynamic_cast<AST::CallExpr*>(&expr);
    std::cout << std::string(level * 3, ' ') << ((level != 0) ? "└" : "") << "function call: " << call->callee << std::endl;
    for (auto& expr_ : call->args)
      Logger::displayASTExpr(level+1, *expr_);
  }
  else if (typeid(expr) == typeid(AST::IfExpr)) {
    AST::IfExpr* ifStatement = dynamic_cast<AST::IfExpr*>(&expr);
    std::cout << std::string(level * 3, ' ') << ((level != 0) ? "└" : "") << "if/else statement: " << std::endl;
    Logger::displayASTExpr(level+1, *ifStatement->cond);
    Logger::displayASTExpr(level+1, *ifStatement->then);
    Logger::displayASTExpr(level+1, *ifStatement->else_);
  }
  else if (typeid(expr) == typeid(AST::WhileExpr)) {
    AST::WhileExpr* whileStatement = dynamic_cast<AST::WhileExpr*>(&expr);
    std::cout << std::string(level * 3, ' ') << ((level != 0) ? "└" : "") << "while statement: " << std::endl;
    Logger::displayASTExpr(level+1, *whileStatement->cond);
    Logger::displayASTExpr(level+1, *whileStatement->body);
  }
}

void Logger::displayASTExpr(AST::Expr& expr) {
  logMessage(LogType::DEBUG_INFO, "Abstract Syntax Tree");
  Logger::displayASTExpr(0, expr);
  std::cout << std::endl;
}

// Function to display tokens in a readable format
void Logger::displayTokenList(const std::vector<Token>& tokens) {
  logMessage(LogType::DEBUG_INFO, "Token List");
  for (const auto& token : tokens) {
    Logger::displayToken(token);
  }
}

void Logger::displayToken(const DecafScanning::Token token) {
  switch (token.type) {
    case TokenType::DEF:
      std::cout << "Token Type: DEF\n";
      break;
    case TokenType::IF:
      std::cout << "Token Type: IF\n";
      break;
    case TokenType::ELSE:
      std::cout << "Token Type: ELSE\n";
      break;
    case TokenType::WHILE:
      std::cout << "Token Type: WHILE\n";
      break;
    case TokenType::RETURN:
      std::cout << "Token Type: RETURN\n";
      break;
    case TokenType::IDENTIFIER:
      std::cout << "Token Type: IDENTIFIER, Value: " << *token.value << '\n';
      break;
    case TokenType::NUMBER:
      std::cout << "Token Type: NUMBER, Value: " << *token.value << '\n';
      break;
    case TokenType::OPEN_PAREN:
      std::cout << "Token Type: OPEN_PAREN\n";
      break;
    case TokenType::CLOSE_PAREN:
      std::cout << "Token Type: CLOSE_PAREN\n";
      break;
    case TokenType::OPEN_CURLY:
      std::cout << "Token Type: OPEN_CURLY\n";
      break;
    case TokenType::CLOSE_CURLY:
      std::cout << "Token Type: CLOSE_CURLY\n";
      break;
    case TokenType::OPEN_BRACKET:
      std::cout << "Token Type: OPEN_BRACKET\n";
      break;
    case TokenType::CLOSE_BRACKET:
      std::cout << "Token Type: CLOSE_BRACKET\n";
      break;
    case TokenType::EQUAL:
      std::cout << "Token Type: EQUAL\n";
      break;
    case TokenType::EQUAL_EQUAL:
      std::cout << "Token Type: EQUAL_EQUAL\n";
      break;
    case TokenType::LESS_THAN:
      std::cout << "Token Type: LESS_THAN\n";
      break;
    case TokenType::LESS_THAN_EQUAL:
      std::cout << "Token Type: LESS_THAN_EQUAL\n";
      break;
    case TokenType::GREATER_THAN:
      std::cout << "Token Type: GREATER_THAN\n";
      break;
    case TokenType::GREATER_THAN_EQUAL:
      std::cout << "Token Type: GREATER_THAN_EQUAL\n";
      break;
    case TokenType::PLUS:
      std::cout << "Token Type: PLUS\n";
      break;
    case TokenType::COMMA:
      std::cout << "Token Type: COMMA\n";
      break;
    case TokenType::SEMICOLON:
      std::cout << "Token Type: SEMICOLON\n";
      break;
  }
}

}