#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "resolution/registry.h"
#include "parser/parser.h"
#include "lexer/lexer.h"

namespace Resolution {
  // all aliases start with this symbol
  inline const std::string ALIAS_SYMBOL = "@";

  // this module loads source modules from files
  class ModulesLoader {
    private:
      // module instances
      Lexer::Lexer lexer;
      Parser::Parser parser;
      ModulesRegistry registry;

      // define path aliases
      std::unordered_map<std::string, std::string> pathAliases;

      // unit methods to load module
      Module* getModuleByAbsolutePath(std::string);
      std::vector<std::string> getModuleDependencies(std::string, AST::BlockStatement*);
      std::string readModuleSourceCodeByAbsolutePath(std::string);

      // utility list to prevent loops during recursive loading
      std::vector<std::string> loadingModulesPaths;
      
    public:
      ModulesLoader();
      
      // sets path aliases
      void loadPathAliases(std::unordered_map<std::string, std::string>);
      
      // recursively loads modules starting with provided entry module path
      void loadModulesFromEntrypointPath(std::string);
      
      // gets loaded modules
      std::vector<Module*> getModules();
      
      // get loaded module by absolute path
      Module* getLoadedModuleByAbsolutePath(std::string);
      int getLoadedModuleIndexByAbsolutePath(std::string);
      // computes absolute path of dependency by absolute path of dependent module and relative path to dependency
      // returns empty string if alias is not resolved
      std::string resolveAbsolutePath(std::string basePath, std::string path);
  };
}
