#include "Lexer.hpp"
#include "Logger.hpp"
#include "FileHandler.hpp"
#include "CodeGenerator.hpp"
#include "Parser.hpp"
#include "JIT.hpp"

#include <cstring>

int main(int argc, char* argv[]) {
  try {
    std::cout << "---------------------------------------------------------" << std::endl;
    std::cout << "Lasa Simplified Imperative Language (LaSIL) COMPILER v0.1" << std::endl;
    std::cout << "                  AUTHOR: RACHIT KAKKAR                  " << std::endl;
    std::cout << "---------------------------------------------------------\n" << std::endl;

    if (argc < 2) {
      DecafLogger::Logger::logMessage(DecafLogger::LogType::ERROR, "Please provide a valid LaSIL file.");
    }

    std::string filePath = argv[1];
    std::string content = DecafIO::readFileToString(filePath);
    DecafLogger::Logger::setFile(content);

    DecafScanning::Lexer lexer(content);
    std::vector<DecafScanning::Token> tokens(lexer.tokenize());
    DecafLogger::Logger::displayTokenList(tokens); // Display the tokens
    std::cout << std::endl;

    DecafParsing::Parser parser(tokens);
    std::unique_ptr<DecafParsing::AST::Function> AST;

    DecafJIT::JIT::initJIT();
    DecafCodeGen::CodeGenerator::initializeModuleAndPassManager();

    while (!parser.isAtEnd()) {
      // Parse program consiting of functions or (To-do) top level declarations
      switch (parser.peek().value().type) {
        default:
          DecafJIT::handleTopLevelStatement(&parser);
        case DecafScanning::TokenType::DEF:
          DecafJIT::handleFuncDefinition(&parser);
      }
    }
  
    std::exit(0);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
