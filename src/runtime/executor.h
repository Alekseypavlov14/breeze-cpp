#pragma once 

#include "runtime/memory.h"
#include "runtime/stack.h"
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
  };
}
