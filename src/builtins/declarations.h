#pragma once

#include "runtime/types.h"
#include "parser/ast.h"
#include "base/position.h"

namespace Builtins {
  class BuiltinDeclaration {
    public:
      virtual ~BuiltinDeclaration() = default;
  };
  using BuiltinModuleDeclarations = std::vector<BuiltinDeclaration*>;
  
  // constants
  class ConstantBuiltinDeclaration: public BuiltinDeclaration {
    private:
      std::string name;
      Runtime::Value* value;

    public:
      ConstantBuiltinDeclaration(std::string name, Runtime::Value* value);
  
      std::string getName();
      Runtime::Value* getValue();
  };

  // functions
  class FunctionBuiltinDeclaration: public BuiltinDeclaration {
    private:
      std::string name;
      Runtime::Callable callable;
      Runtime::FunctionArgumentsAmount argumentsAmount;

    public:
      FunctionBuiltinDeclaration(std::string name, Runtime::Callable callable, Runtime::FunctionArgumentsAmount argumentsAmount);

      std::string getName();
      Runtime::Callable getCallable();
      Runtime::FunctionArgumentsAmount getArgumentsAmount();
  };

  // classes
  class ClassBuiltinDeclaration: public BuiltinDeclaration {
    public:
      ClassBuiltinDeclaration();
  };
}