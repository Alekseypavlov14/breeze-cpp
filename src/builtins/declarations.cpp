#include "builtins/declarations.h"

namespace Builtins {
  ConstantBuiltinDeclaration::ConstantBuiltinDeclaration(std::string name, Runtime::Value* value) {
    this->position = BUILTIN_POSITION;
    this->name = name;
    this->value = value;
  }
  ConstantBuiltinDeclaration* ConstantBuiltinDeclaration::clone() const {
    return new ConstantBuiltinDeclaration(this->name, this->value);
  }
  std::string ConstantBuiltinDeclaration::getName() {
    return this->name;
  }
  Runtime::Value* ConstantBuiltinDeclaration::getValue() {
    return this->value;
  }

  FunctionArgumentsAmount::FunctionArgumentsAmount(int argumentsAmount, int optionalArguments) {
    this->argumentsAmount = argumentsAmount;
    this->optionalArguments = optionalArguments;
  }
  int FunctionArgumentsAmount::getTotalArgumentsAmount() {
    return this->argumentsAmount;
  }
  int FunctionArgumentsAmount::getRequiredArgumentsAmount() {
    return this->argumentsAmount - this->optionalArguments;
  }
  int FunctionArgumentsAmount::getOptionalArgumentsAmount() {
    return this->optionalArguments;
  }

  FunctionBuiltinDeclaration::FunctionBuiltinDeclaration(std::string name, Runtime::Callable callable, FunctionArgumentsAmount argumentsAmount) {
    this->position = BUILTIN_POSITION;
    this->name = name;
    this->callable = callable;
    this->argumentsAmount = argumentsAmount;
  }
  FunctionBuiltinDeclaration* FunctionBuiltinDeclaration::clone() const {
    return new FunctionBuiltinDeclaration(this->name, this->callable, this->argumentsAmount);
  }
  std::string FunctionBuiltinDeclaration::getName() {
    return this->name;
  }
  Runtime::Callable FunctionBuiltinDeclaration::getCallable() {
    return this->callable;
  }
  FunctionArgumentsAmount FunctionBuiltinDeclaration::getArgumentsAmount() {
    return this->argumentsAmount;
  }
}