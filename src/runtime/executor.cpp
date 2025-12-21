#include "executor.h"
#include "runtime/exceptions.h"
#include "shared/utils.h"

#include <iostream>

namespace Runtime {
  Executor::Executor() {
    this->memory = Memory();
  }

  void Executor::execute() {} 

  Value* Executor::evaluateAdditionExpression(Value* left, Value* right) {
    // number + number
    if (Shared::isInstanceOf<Value, NumberValue>(left) && Shared::isInstanceOf<Value, NumberValue>(right)) {
      double leftValue = NumberValue::getDataOf(left);
      double rightValue = NumberValue::getDataOf(right);

      NumberValue* result = new NumberValue(leftValue + rightValue);
      this->memory.addTemporaryValue(result);

      return result;
    }

    // string + string
    if (Shared::isInstanceOf<Value, StringValue>(left) && Shared::isInstanceOf<Value, StringValue>(right)) {
      std::string leftValue = StringValue::getDataOf(left);
      std::string rightValue = StringValue::getDataOf(right);

      StringValue* result = new StringValue(leftValue + rightValue);
      this->memory.addTemporaryValue(result);

      return result;
    }

    throw new TypeException("Operator \"+\" is used with invalid type pair");
  }
  Value* Executor::evaluateSubtractionExpression(Value* left, Value* right) {
    if (Shared::isInstanceOf<Value, NumberValue>(left) && Shared::isInstanceOf<Value, NumberValue>(right)) {
      double leftValue = NumberValue::getDataOf(left);
      double rightValue = NumberValue::getDataOf(right);

      NumberValue* result = new NumberValue(leftValue - rightValue);
      this->memory.addTemporaryValue(result);

      return result;
    }

    throw new TypeException("Operator \"-\" is used with invalid type pair");
  }
  Value* Executor::evaluateMultiplicationExpression(Value* left, Value* right) {
    if (Shared::isInstanceOf<Value, NumberValue>(left) && Shared::isInstanceOf<Value, NumberValue>(right)) {
      double leftValue = NumberValue::getDataOf(left);
      double rightValue = NumberValue::getDataOf(right);

      NumberValue* result = new NumberValue(leftValue * rightValue);
      this->memory.addTemporaryValue(result);

      return result;
    }

    throw new TypeException("Operator \"*\" is used with invalid type pair");
  }
  Value* Executor::evaluateDivisionExpression(Value* left, Value* right) {
    if (Shared::isInstanceOf<Value, NumberValue>(left) && Shared::isInstanceOf<Value, NumberValue>(right)) {
      double leftValue = NumberValue::getDataOf(left);
      double rightValue = NumberValue::getDataOf(right);

      NumberValue* result = new NumberValue(leftValue / rightValue);
      this->memory.addTemporaryValue(result);

      return result;
    }

    throw new TypeException("Operator \"/\" is used with invalid type pair");
  }

  Container* Executor::evaluateLeftExpression(AST::Expression* expression) {
  }
}
