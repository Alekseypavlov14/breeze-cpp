#include "resolution/resolver.h"
#include "shared/utils.h"

namespace Resolution {
  Resolver::Resolver() {
    this->loader = ModulesLoader();
    this->graph = ModulesGraph();
  }

  void Resolver::loadPathAliases(std::unordered_map<std::string, std::string> aliases) {
    this->loader.loadPathAliases(aliases);
  }

  void Resolver::loadModulesFromEntrypointPath(std::string entrypoint) {
    this->loader.loadModulesFromEntrypointPath(entrypoint);
  }
  
  std::vector<Module*> Resolver::getTopologicallySortedModules() {
    return this->graph.getTopologicallySortedModules(this->loader.getModules());
  }
}
