#include "resolution/graph.h"
#include "resolution/exception.h"

#include "shared/utils.h"

namespace Resolution {
  ModulesGraph::ModulesGraph() {
    this->operatingModules = {};
    this->sortedModules = {};
    this->undiscoveredModulesPaths = {};
    this->discoveredModulesPaths = {};
    this->analyzedModulesPaths = {};
    this->modulesInDegree = {};
  }

  std::vector<Module*> ModulesGraph::getTopologicallySortedModules(std::vector<Module*> modules) {
    this->operatingModules = modules;
    this->sortedModules = {};
    this->undiscoveredModulesPaths = {};
    this->discoveredModulesPaths = {};
    this->analyzedModulesPaths = {};

    // compute modules in-degree
    for (int i = 0; i < this->operatingModules.size(); i++) {
      int inDegree = 0;

      for (int j = 0; j < this->operatingModules.size(); j++) {
        if (Shared::includes(this->operatingModules[j]->getDependenciesPaths(), this->operatingModules[i]->getAbsolutePath())) {
          inDegree++;
        }
      }

      this->modulesInDegree.push_back(inDegree);
    }

    // search starting from 0 in-degree modules
    for (int i = 0; i < this->operatingModules.size(); i++) {
      if (this->modulesInDegree[i] > 0) continue;

      depthFirstSearch(this->operatingModules[i]);
    }

    // check sorting
    if (this->sortedModules.size() != this->operatingModules.size()) {
      throw Resolution::Exception("Circular dependency indicated");
    }

    return this->sortedModules;
  }

  void ModulesGraph::depthFirstSearch(Module* module) {
    // skip analyzed nodes
    if (Shared::includes(this->analyzedModulesPaths, module->getAbsolutePath())) return;

    if (Shared::includes(this->discoveredModulesPaths, module->getAbsolutePath())) {
      std::string message = "Circular dependency includes module: " + module->getAbsolutePath();
      throw Resolution::Exception(message);
    }

    // discover node
    this->discoveredModulesPaths.push_back(module->getAbsolutePath());

    // analyze dependencies
    for (int i = 0; i < module->getDependenciesPaths().size(); i++) {
      Module* dependencyModule = this->getModuleByPath(module->getDependenciesPaths()[i]);

      if (dependencyModule == NULL) {
        std::string message = "Impossible to load module " + module->getDependenciesPaths()[i];
        throw Resolution::Exception(message);
      }

      this->depthFirstSearch(dependencyModule);
    }

    // added sorted module
    this->sortedModules.push_back(module);

    // analyze node
    Shared::removeAll(this->discoveredModulesPaths, module->getAbsolutePath());
    this->analyzedModulesPaths.push_back(module->getAbsolutePath());
  }

  Module* ModulesGraph::getModuleByPath(std::string path) {
    for (int i = 0; i < this->operatingModules.size(); i++) {
      if (this->operatingModules[i]->getAbsolutePath() == path) {
        return this->operatingModules[i];
      }
    }

    return NULL;
  }
}
