#ifndef JIT_H
#define JIT_H

#include "KaleidoscopeJIT.hpp"
#include "AST.hpp"
#include "Parser.hpp"

namespace DecafJIT {

class JIT {
public:
  static std::unique_ptr<llvm::orc::KaleidoscopeJIT> JIT_;  
  static llvm::ExitOnError exitOnError;
  static void initJIT();
};

void handleFuncDefinition(DecafParsing::Parser* parser);
double handleTopLevelStatement(DecafParsing::Parser* parser);

}

#endif