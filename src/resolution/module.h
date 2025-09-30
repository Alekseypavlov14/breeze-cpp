#pragma once

#include "parser/ast.h"

namespace Resolution {
  // module represents source code file
  class Module {
    private: 
      std::string absolutePath;
      std::vector<std::string> dependenciesPaths;
      AST::BlockStatement* content;

    public:
      Module(std::string, std::vector<std::string>, AST::BlockStatement*);
      ~Module();

      std::string getAbsolutePath();
      std::vector<std::string> getDependenciesPaths();
      AST::BlockStatement* getContent();
  }; 
}
