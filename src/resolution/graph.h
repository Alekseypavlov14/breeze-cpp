#pragma once

#include <vector>

#include "resolution/module.h"

namespace Resolution {
  // this class represents source code modules as graph and defines graph operations 
  class ModulesGraph {
    private:
      // contains application modules 
      std::vector<Module*> operatingModules;
      std::vector<Module*> sortedModules;

      // assistant lists
      std::vector<std::string> undiscoveredModulesPaths;
      std::vector<std::string> discoveredModulesPaths;
      std::vector<std::string> analyzedModulesPaths;

      std::vector<int> modulesInDegree = {};

      // depth first search algorithm implementation
      void depthFirstSearch(Module*);
      Module* getModuleByPath(std::string);

    public:
      ModulesGraph();

      std::vector<Module*> getTopologicallySortedModules(std::vector<Module*>);
  };
}
