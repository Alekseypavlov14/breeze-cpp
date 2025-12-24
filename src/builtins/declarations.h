#pragma once

#include "runtime/types.h"
#include "parser/ast.h"
#include "base/position.h"

namespace Builtins {
  class BuiltinDeclaration: public AST::Statement {};
  
  inline const Base::Position BUILTIN_POSITION(0, 0);
  
  using BuiltinModuleDeclarations = std::vector<BuiltinDeclaration*>;
  
  // constants
  class ConstantBuiltinDeclaration: public BuiltinDeclaration {
    private:
      std::string name;
      Runtime::Value* value;

    public:
      ConstantBuiltinDeclaration(std::string name, Runtime::Value* value);
      ConstantBuiltinDeclaration* clone() const;
  
      std::string getName();
      Runtime::Value* getValue();
  };

  // functions
  class FunctionArgumentsAmount {
    private:
      int argumentsAmount;
      int optionalArguments;

    public:
      FunctionArgumentsAmount(int argumentsAmount = 0, int optionalArguments = 0);

      int getTotalArgumentsAmount();
      int getRequiredArgumentsAmount();
      int getOptionalArgumentsAmount();
  };
  class FunctionBuiltinDeclaration: public BuiltinDeclaration {
    private:
      std::string name;
      Runtime::Callable callable;
      FunctionArgumentsAmount argumentsAmount;

    public:
      FunctionBuiltinDeclaration(std::string name, Runtime::Callable callable, FunctionArgumentsAmount argumentsAmount);
      FunctionBuiltinDeclaration* clone() const;

      std::string getName();
      Runtime::Callable getCallable();
      FunctionArgumentsAmount getArgumentsAmount();
  };

  // classes
  class ClassBuiltinDeclaration: public BuiltinDeclaration {
    public:
      ClassBuiltinDeclaration();
      ClassBuiltinDeclaration* clone() const;
  };
}