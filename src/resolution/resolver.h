#pragma once

#include "resolution/graph.h"
#include "resolution/loader.h"
#include "resolution/registry.h"
#include "resolution/module.h"

// this module resolves module dependencies and creates module graph
namespace Resolution {
  class Resolver {
    private:
      ModulesLoader loader;
      ModulesGraph graph;

    public:
      Resolver();

      // loads configurations
      void loadPathAliases(std::unordered_map<std::string, std::string>);

      // loads modules dependency graph from a given source
      void loadModulesFromEntrypointPath(std::string);
      
      // returns topologically sorted modules 
      std::vector<Module*> getTopologicallySortedModules();
  };
}
