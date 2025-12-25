#include "builtins/declarations.h"

namespace Builtins {
  ConstantBuiltinDeclaration::ConstantBuiltinDeclaration(std::string name, Runtime::Value* value) {
    this->name = name;
    this->value = value;
  }
  std::string ConstantBuiltinDeclaration::getName() {
    return this->name;
  }
  Runtime::Value* ConstantBuiltinDeclaration::getValue() {
    return this->value;
  }

  FunctionBuiltinDeclaration::FunctionBuiltinDeclaration(std::string name, Runtime::Callable callable, Runtime::FunctionArgumentsAmount argumentsAmount) {
    this->name = name;
    this->callable = callable;
    this->argumentsAmount = argumentsAmount;
  }
  std::string FunctionBuiltinDeclaration::getName() {
    return this->name;
  }
  Runtime::Callable FunctionBuiltinDeclaration::getCallable() {
    return this->callable;
  }
  Runtime::FunctionArgumentsAmount FunctionBuiltinDeclaration::getArgumentsAmount() {
    return this->argumentsAmount;
  }
}