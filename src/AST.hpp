#ifndef AST_H
#define AST_H

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"

#include "Lexer.hpp"

#include <memory>
#include <map>
#include <utility>

namespace DecafParsing {

namespace AST {

struct Expr {
public:
  virtual ~Expr() = default;
  virtual llvm::Value *codegen() = 0;
};

struct Prototype {
public:
  Prototype(const std::string &name, std::vector<std::string> args)
    : name(name), args(std::move(args)) {}

  const std::string &getName() const { return name; }
  std::string name;
  std::vector<std::string> args;
  llvm::Function *codegen();
};

struct Function {
public:
  Function(std::unique_ptr<Prototype> proto,
           std::unique_ptr<Expr> body)
    : proto(std::move(proto)), body(std::move(body)) {}
  std::unique_ptr<Prototype> proto;
  std::unique_ptr<Expr> body;
  llvm::Function *codegen();
};

struct NumberExpr : public Expr {
public:
  NumberExpr(double value) : value(value) {}
  llvm::Value *codegen() override;
  double value;
};

struct VariableExpr : public Expr {
public:
  VariableExpr(const std::string &name) : name(name) {}
  llvm::Value *codegen() override;
  std::string name;
};

struct BinaryExpr : public Expr {
public:
  BinaryExpr(DecafScanning::Token op, std::unique_ptr<Expr> LHS,
                std::unique_ptr<Expr> RHS)
    : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
  llvm::Value *codegen() override;
  DecafScanning::Token op;
  std::unique_ptr<Expr> LHS, RHS;
};

struct CallExpr : public Expr {
public:
  CallExpr(const std::string &callee,
              std::vector<std::unique_ptr<Expr>> args)
    : callee(callee), args(std::move(args)) {}
  llvm::Value *codegen() override;
  std::string callee;
  std::vector<std::unique_ptr<Expr>> args;
};

class IfExpr: public Expr {
public:
  IfExpr(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> then,
            std::unique_ptr<Expr> else_)
    : cond(std::move(cond)), then(std::move(then)), else_(std::move(else_)) {}

  llvm::Value *codegen() override;
  std::unique_ptr<Expr> cond, then, else_;
};

class WhileExpr: public Expr {
public:
  WhileExpr(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> body)
    : cond(std::move(cond)), body(std::move(body)) {}

  llvm::Value *codegen() override;
  std::unique_ptr<Expr> cond, body;
};

}

}

#endif