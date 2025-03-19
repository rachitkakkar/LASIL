#ifndef PARSER_H
#define PARSER_H

#include "Lexer.hpp"
#include "Logger.hpp"
#include "AST.hpp"

#include <memory>
#include <map>
#include <utility>

namespace DecafParsing {

class Parser {
public:
  explicit Parser(std::vector<DecafScanning::Token> tokens);

  std::unique_ptr<AST::Function> parse();
  std::unique_ptr<AST::Expr> parseBinaryExpr(int exprPrec, std::unique_ptr<AST::Expr> LHS);
  std::unique_ptr<AST::Prototype> parsePrototype();
  std::unique_ptr<AST::Function> parseFuncDefinition();
  std::unique_ptr<AST::Expr> parsePrimaryExpr();
  std::unique_ptr<AST::Expr> parseExpr();
  std::unique_ptr<AST::Function> parseTopLevelExpr();

  DecafScanning::Token consume();
  std::optional<DecafScanning::Token> peek(int offset = 0);
  bool isAtEnd();

private:
  std::vector<DecafScanning::Token> m_tokens;
  std::map<DecafScanning::TokenType, int> m_binopPrecedence;
  int getTokPrecedence();
  int m_index = 0;

  std::unique_ptr<AST::Expr> numberExpr();
  std::unique_ptr<AST::Expr> groupingExpr();
  std::unique_ptr<AST::Expr> identifierExpr();
  std::unique_ptr<AST::Expr> conditionalExpr();
  std::unique_ptr<AST::Expr> whileExpr();
};

}

#endif
