#include "executor.h"
#include "runtime/exceptions.h"
#include "shared/utils.h"

namespace Runtime {
  Value* Executor::evaluateAdditionExpression(Value* left, Value* right) {
    // number + number
    if (Shared::isInstanceOf<Value, NumberValue>(left) && Shared::isInstanceOf<Value, NumberValue>(right)) {
      double leftValue = Shared::cast<Value, NumberValue>(left)->getData();
      double rightValue = Shared::cast<Value, NumberValue>(right)->getData();
      return new NumberValue(leftValue + rightValue);
    }

    // string + string
    if (Shared::isInstanceOf<Value, StringValue>(left) && Shared::isInstanceOf<Value, StringValue>(right)) {
      std::string leftValue = Shared::cast<Value, StringValue>(left)->getData();
      std::string rightValue = Shared::cast<Value, StringValue>(right)->getData();
      return new StringValue(leftValue + rightValue);
    }

    throw new TypeException("Operator \"+\" is used with invalid type pair");
  }
}
