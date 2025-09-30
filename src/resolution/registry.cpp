#pragma once

#include "resolution/registry.h"
#include "resolution/module.h"

#include "shared/utils.h"

namespace Resolution {
  ModulesRegistry::ModulesRegistry() {
    this->modules = {};
  }
  ModulesRegistry::~ModulesRegistry() {
    for (int i = 0; i < this->modules.size(); i++) {
      delete this->modules[i];
    }
  }

  std::vector<Module*> ModulesRegistry::getModules() {
    return this->modules;
  }
  
  void ModulesRegistry::addModule(Module* module) {
    // do not add duplications
    if (Shared::includes(this->modules, module)) return;

    this->modules.push_back(module);
  }

  bool ModulesRegistry::isModuleAddedByPath(std::string absolutePath) {
    return this->getModuleByAbsolutePath(absolutePath) != NULL;
  }
  
  Module* ModulesRegistry::getModuleByAbsolutePath(std::string absolutePath) {
    for (int i = 0; i < this->modules.size(); i++) {
      if (this->modules[i]->getAbsolutePath() == absolutePath) {
        return this->modules[i];
      }
    }

    return NULL;
  }
}
