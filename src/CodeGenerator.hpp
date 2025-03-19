#ifndef CODEGEN_H
#define CODEGEN_H

#include "AST.hpp"

#include <map>
#include <string>

namespace DecafCodeGen {

class CodeGenerator {
public:
  static std::unique_ptr<llvm::LLVMContext> context;
  static std::unique_ptr<llvm::IRBuilder<>> builder;
  static std::unique_ptr<llvm::Module> module_;
  static std::map<std::string, llvm::Value*> namedValues;
  static std::map<std::string, std::unique_ptr<DecafParsing::AST::Prototype>> functionProtos;

  static std::unique_ptr<llvm::FunctionPassManager> FPM;
  static std::unique_ptr<llvm::LoopAnalysisManager> LAM;
  static std::unique_ptr<llvm::FunctionAnalysisManager> FAM;
  static std::unique_ptr<llvm::CGSCCAnalysisManager> CGAM;
  static std::unique_ptr<llvm::ModuleAnalysisManager> MAM;
  static std::unique_ptr<llvm::PassInstrumentationCallbacks> PIC;
  static std::unique_ptr<llvm::StandardInstrumentations> SI;

  static llvm::PassBuilder PB;
  
  static void initializeModuleAndPassManager();
};

}

#endif
