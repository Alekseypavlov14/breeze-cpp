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
  class FunctionBuiltinDeclaration: public BuiltinDeclaration {
    private:
      std::string name;
      Runtime::Callable callable;
      Runtime::FunctionArgumentsAmount argumentsAmount;

    public:
      FunctionBuiltinDeclaration(std::string name, Runtime::Callable callable, Runtime::FunctionArgumentsAmount argumentsAmount);
      FunctionBuiltinDeclaration* clone() const;

      std::string getName();
      Runtime::Callable getCallable();
      Runtime::FunctionArgumentsAmount getArgumentsAmount();
  };

  // classes
  class ClassBuiltinDeclaration: public BuiltinDeclaration {
    public:
      ClassBuiltinDeclaration();
      ClassBuiltinDeclaration* clone() const;
  };
}