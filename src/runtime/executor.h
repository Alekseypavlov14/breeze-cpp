#pragma once 

#include "runtime/memory.h"
#include "runtime/stack.h"
#include "runtime/types.h"
#include "resolution/module.h"

namespace Runtime {
  class Executor {
    public:
      Executor();

      void loadModules(std::vector<Resolution::Module*>);
      void registerBuiltins();
      void execute();

    private:
      Memory memory;

      Value* evaluateAdditionExpression(Value* left, Value* right);
      Value* evaluateSubtractionExpression(Value* left, Value* right);
      Value* evaluateMultiplicationExpression(Value* left, Value* right);
      Value* evaluateDivisionExpression(Value* left, Value* right);
  };
}
