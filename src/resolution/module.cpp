#include "resolution/module.h"

namespace Resolution {
  Module::Module(std::string absolutePath, std::vector<std::string> dependenciesPath, AST::BlockStatement* content) {
    this->absolutePath = absolutePath;
    this->dependenciesPaths = dependenciesPath;
    this->content = content;
  }
  Module::~Module() {
    delete this->content;
  }
  std::string Module::getAbsolutePath() {
    return this->absolutePath;
  }
  std::vector<std::string> Module::getDependenciesPaths() {
    return this->dependenciesPaths;
  }
  AST::BlockStatement* Module::getContent() {
    return this->content;
  }
}
