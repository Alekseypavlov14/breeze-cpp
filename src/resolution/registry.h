#pragma once

#include <vector>

#include "resolution/module.h"

namespace Resolution {
  // stores pointers to allocated modules
  // releases memory after destruction
  class ModulesRegistry {
    private:
      std::vector<Module*> modules;
      
    public:
      ModulesRegistry();
      ~ModulesRegistry();

      std::vector<Module*> getModules();

      void addModule(Module*);
      bool isModuleAddedByPath(std::string);
      Module* getModuleByAbsolutePath(std::string);
  };
}
