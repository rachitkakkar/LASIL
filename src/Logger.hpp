#ifndef LOGGER_H
#define LOGGER_H

#include "Logger.hpp"
#include "Lexer.hpp"
#include "AST.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

namespace DecafLogger {

// To-Do: Fix for default Apple Terminal and Windows
#define ERROR_ESCAPE_SEQUENCE "\e[1;37;41m Error: \e[m"
#define WARNING_ESCAPE_SEQUENCE "\e[1;37;44m Warning: \e[m"
#define DEBUG_ESCAPE_SEQUENCE "\e[1;37;45m Debug Info: \e[m"

template<typename ... Args>
std::string stringFormat( const std::string& format, Args ... args ) {
  size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
  if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
  std::unique_ptr<char[]> buf( new char[ size ] ); 
  snprintf( buf.get(), size, format.c_str(), args ... );
  return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

enum class LogType {
  DEBUG_INFO,
  ERROR,
  WARNING
};

struct Line {
  std::string content;
  std::size_t startPosition;
  std::size_t endPosition;
  int lineNumber;
};

class Logger {
public:
  static void logMessage(LogType type, const std::string& msg);
  static void logMessage(LogType type, const std::string& msg, const DecafScanning::Token& token);
  static void logMessage(LogType type, const std::string& msg, std::size_t position);
  // static void logMessage(LogType type, const std::string& msg, const std::string& fileText, std::size_t position);
  static void displayTokenList(const std::vector<DecafScanning::Token>& tokens);
  static void displayToken(const DecafScanning::Token token);
  static void displayASTExpr(int level, DecafParsing::AST::Expr& expr);
  static void displayASTExpr(DecafParsing::AST::Expr& expr);

  static std::string fileText;
  static std::vector<Line> lines;
  static void setFile(std::string fileText);
};

}

#endif