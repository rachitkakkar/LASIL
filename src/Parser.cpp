#include "Parser.hpp"
#define DEBUG_LOG DecafLogger::Logger::displayToken(peek().value()); std::cout << "Line "<< __LINE__ << " ran!" << std::endl;

namespace DecafParsing {

Parser::Parser(std::vector<DecafScanning::Token> tokens) : m_tokens(tokens) {
  // 1 is lowest precedence.
  m_binopPrecedence[DecafScanning::TokenType::TIMES] = 4;
  m_binopPrecedence[DecafScanning::TokenType::DIVIDE] = 4;
  m_binopPrecedence[DecafScanning::TokenType::PLUS] = 3;
  m_binopPrecedence[DecafScanning::TokenType::MINUS] = 3;
  m_binopPrecedence[DecafScanning::TokenType::LESS_THAN] = 2;
  m_binopPrecedence[DecafScanning::TokenType::GREATER_THAN] = 2;
  m_binopPrecedence[DecafScanning::TokenType::LESS_THAN_EQUAL] = 2;
  m_binopPrecedence[DecafScanning::TokenType::GREATER_THAN_EQUAL] = 2;
  m_binopPrecedence[DecafScanning::TokenType::EQUAL_EQUAL] = 1;
}

bool Parser::isAtEnd () {
  return (m_tokens.size()-1) <= m_index;
}

int Parser::getTokPrecedence() {
  if (m_binopPrecedence.count(peek().value().type) > 0)
    return m_binopPrecedence[peek().value().type];
  else
    return -1;
}

std::optional<DecafScanning::Token> Parser::peek(int offset) {
  if (m_index + offset >= m_tokens.size()) {
    return {}; // Return an empty optional if the offset is out of bounds
  } else {
    return m_tokens.at(m_index + offset); // Return the character at the given offset
  }
}

DecafScanning::Token Parser::consume() {
  return m_tokens.at(m_index++);
}

std::unique_ptr<AST::Expr> Parser::numberExpr() {
  std::cout << "Parse number expression" << std::endl;
  if (peek().value().type == DecafScanning::TokenType::NUMBER) {
    auto result = std::make_unique<AST::NumberExpr>(std::stof(*peek().value().value));
    if (!isAtEnd()) {
      DEBUG_LOG
      consume();
    }
    return result;
  }
  
  return nullptr; // Todo: Throw error
}

std::unique_ptr<AST::Expr> Parser::groupingExpr() {
  std::cout << "Parse grouping expression" << std::endl;
  if (peek().value().type == DecafScanning::TokenType::OPEN_PAREN) {
    DEBUG_LOG
    consume();
    auto expr = parseExpr();
    if (!expr)
      return nullptr;
    if (peek().value().type == DecafScanning::TokenType::CLOSE_PAREN) {
      if (!isAtEnd()) {
        DEBUG_LOG
        consume();
      }
      return expr;
    }
  }
  
  return nullptr; // Todo: Throw error
}

std::unique_ptr<AST::Expr> Parser::identifierExpr() {
  std::cout << "Parse identifier expression" << std::endl;

  if (peek().value().type == DecafScanning::TokenType::IDENTIFIER) {
    std::string name = *peek().value().value;
    if (!isAtEnd()) {
      DEBUG_LOG
      consume();
    }

    if (peek().value().type != DecafScanning::TokenType::OPEN_PAREN) // Simple variable reference
      return std::make_unique<AST::VariableExpr>(name);
    
    // Function call
    DEBUG_LOG
    consume();
    std::vector<std::unique_ptr<AST::Expr>> args;
    while (true) {
      if (auto arg = parseExpr()) { // Parse arguments
        args.push_back(std::move(arg));
      }
      else {
        DecafLogger::Logger::logMessage(DecafLogger::LogType::ERROR, "Failed to parse argument", peek().value());
        return nullptr; // Todo: Throw error
      }

      if (peek().value().type == DecafScanning::TokenType::CLOSE_PAREN) { // End of function call
        std::cout << "why this no run :(" << std::endl;
        break;
      }

      if (peek().value().type != DecafScanning::TokenType::COMMA) {
        std::cout << "Expected ')' or ',' in argument list" << std::endl;
        return nullptr; // Todo: Throw an error
      }
      DEBUG_LOG
      consume();
    }
    if (!isAtEnd()) {
      DEBUG_LOG
      consume(); // Consume ')'
    }
    return std::make_unique<AST::CallExpr>(name, std::move(args));
  }

  return nullptr; // Todo: Throw error
}

std::unique_ptr<AST::Expr> Parser::conditionalExpr() {
  if (peek().value().type == DecafScanning::TokenType::IF) 
    { DEBUG_LOG consume(); } // eat the If
  else return nullptr;
  if (peek().value().type == DecafScanning::TokenType::OPEN_PAREN) 
    { DEBUG_LOG consume(); }  // eat the (
  else
    return nullptr;

  auto cond = parseExpr();
  if (!cond)
    return nullptr;

  if (peek().value().type == DecafScanning::TokenType::CLOSE_PAREN) 
    { DEBUG_LOG consume(); }  // eat the )
  else 
    return nullptr;
  if (peek().value().type != DecafScanning::TokenType::OPEN_CURLY)
    return nullptr; // To-do: Throw error
  DEBUG_LOG
  consume();  // eat the {

  auto then = parseExpr();
  if (!then)
    return nullptr;

  if (peek().value().type == DecafScanning::TokenType::CLOSE_CURLY) 
    { DEBUG_LOG consume(); } // eat the }
  else 
    return nullptr;

  if (peek().value().type != DecafScanning::TokenType::ELSE)
    return nullptr; // To-do: Throw error
  DEBUG_LOG
  consume();
  if (peek().value().type != DecafScanning::TokenType::OPEN_CURLY)
    return nullptr; // To-do: Throw error
  DEBUG_LOG
  consume();  // eat the {

  auto else_ = parseExpr();
  if (!else_)
    return nullptr;

  if (peek().value().type == DecafScanning::TokenType::CLOSE_CURLY) {
    if (!isAtEnd()) {
      DEBUG_LOG
      consume();
    }
  } // Eat the } if not at end
  else 
    return nullptr;

  return std::make_unique<AST::IfExpr>(std::move(cond), std::move(then),
                                      std::move(else_));
}

std::unique_ptr<AST::Expr> Parser::whileExpr() {
  if (peek().value().type == DecafScanning::TokenType::WHILE) 
    { DEBUG_LOG consume(); } // eat the while
  else return nullptr;
  if (peek().value().type == DecafScanning::TokenType::OPEN_PAREN) 
    { DEBUG_LOG consume(); }  // eat the (
  else
    return nullptr;

  auto cond = parseExpr();
  if (!cond)
    return nullptr;

  if (peek().value().type == DecafScanning::TokenType::CLOSE_PAREN) 
    { DEBUG_LOG consume(); }  // eat the )
  else 
    return nullptr;
  if (peek().value().type != DecafScanning::TokenType::OPEN_CURLY)
    return nullptr; // To-do: Throw error
  DEBUG_LOG
  consume();  // eat the {

  auto body = parseExpr();
  if (!body)
    return nullptr;

  if (peek().value().type == DecafScanning::TokenType::CLOSE_CURLY) {
    if (!isAtEnd()) {
      DEBUG_LOG
      consume();
    }
  } // Eat the } if not at end
  else 
    return nullptr;

  return std::make_unique<AST::WhileExpr>(std::move(cond), std::move(body));
}

std::unique_ptr<AST::Expr> Parser::parseBinaryExpr(int exprPrec, std::unique_ptr<AST::Expr> LHS) {
  std::cout << "Parse binary expression" << std::endl;
  if (isAtEnd()) // End of token sequence
    return LHS; // Can't be binary
  while (true) {
    int tokPrec = getTokPrecedence();

    // If this is a binop that binds at least as tightly as the current binop,
    // Consume it, otherwise we are done.
    if (tokPrec < exprPrec) {
      // Not binary
      return LHS;
    }

    // Ok, we know this must be a binary value at this point
    DecafScanning::Token binOp = peek().value();
    DEBUG_LOG
    consume();

    // Parse the primary expression after the binary operator.
    auto RHS = parsePrimaryExpr();
    if (!RHS) {
      std::cout << "no right hand side???" << std::endl; // To-do: Throw error
      return nullptr;
    }
    
    // If BinOp binds less tightly with RHS than the operator after RHS, let
    // the pending operator take RHS as its LHS
    int nextPrec = getTokPrecedence();
    if (tokPrec < nextPrec) {
      RHS = parseBinaryExpr(tokPrec + 1, std::move(RHS));
      if (!RHS) {
        return nullptr;
      }
    }

    // Merge LHS/RHS for complete binary expression
    LHS = std::make_unique<AST::BinaryExpr>(binOp, std::move(LHS), std::move(RHS));
  }
}

std::unique_ptr<AST::Prototype> Parser::parsePrototype() {
  if (peek().value().type != DecafScanning::TokenType::IDENTIFIER)
    return nullptr; // Todo: Throw Error
  // Get function name
  std::string fnName = *peek().value().value;
  DEBUG_LOG
  consume();

  if (peek().value().type != DecafScanning::TokenType::OPEN_PAREN)
    return nullptr; // Todo: Throw error
  DEBUG_LOG
  consume();

  // Read the list of argument names.
  std::vector<std::string> argNames;
  while (peek().value().type == DecafScanning::TokenType::IDENTIFIER || peek().value().type == DecafScanning::TokenType::COMMA) {
    if (peek().value().type == DecafScanning::TokenType::IDENTIFIER) {
      argNames.push_back(*peek().value().value);
    }
    DEBUG_LOG
    consume();
  }

  if (peek().value().type != DecafScanning::TokenType::CLOSE_PAREN)
    return nullptr; // Todo: Throw an error
  DEBUG_LOG
  consume();

  return std::make_unique<AST::Prototype>(fnName, std::move(argNames));
}

std::unique_ptr<AST::Function> Parser::parseFuncDefinition() {
  if (peek().value().type == DecafScanning::TokenType::DEF) {
    DEBUG_LOG
    consume();
    auto proto = parsePrototype(); // Parse function declaration
    if (!proto) return nullptr;

    if (peek().value().type != DecafScanning::TokenType::OPEN_CURLY) {
      std::cout << "{ expected after function declaration!" << std::endl;
      return nullptr;
    }

    DEBUG_LOG
    consume();

    auto expr = parseExpr(); // Parse function body
    if (peek().value().type != DecafScanning::TokenType::CLOSE_CURLY) {
      std::cout << "} expected after function definition!" << std::endl;
      return nullptr;
    }
    DEBUG_LOG
    consume();

    if (expr) {
      return std::make_unique<AST::Function>(std::move(proto), std::move(expr));
    }

    return nullptr;
  }

  return nullptr; // Todo: Throw an error
}

std::unique_ptr<AST::Expr> Parser::parsePrimaryExpr() {
  // Parse basic, not bin-op expressions
  switch (peek().value().type) {
    default:
      return nullptr; // Todo: Throw an error
    case DecafScanning::TokenType::IDENTIFIER:
      return identifierExpr();
    case DecafScanning::TokenType::NUMBER:
      return numberExpr();
    case DecafScanning::TokenType::OPEN_PAREN:
      return groupingExpr();
    case DecafScanning::TokenType::IF:
      return conditionalExpr();
    case DecafScanning::TokenType::WHILE:
      return whileExpr();
  }
}

std::unique_ptr<AST::Expr> Parser::parseExpr() {
  // Parse any expression (including both the primary ones and bin-ops)
  auto LHS = parsePrimaryExpr();

  if (!LHS)
    return nullptr;
  
  auto expr = parseBinaryExpr(0, std::move(LHS));
  return expr;
}

std::unique_ptr<AST::Function> Parser::parseTopLevelExpr() {
  if (auto expr = parseExpr()) {
    std::cout << "Finished parsing top level statement" << std::endl;
    auto proto = std::make_unique<AST::Prototype>("__anon_expr", std::vector<std::string>());
    return std::make_unique<AST::Function>(std::move(proto), std::move(expr));
  }
  return nullptr;
}

std::unique_ptr<AST::Function> Parser::parse() {
  // Parse program consiting of functions or (To-do) top level declarations
  switch (peek().value().type) {
    default:
      return parseTopLevelExpr(); // Todo: Throw an error
    case DecafScanning::TokenType::DEF:
      return parseFuncDefinition();
  }
}

}