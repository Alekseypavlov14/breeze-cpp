#include <filesystem>

#include "resolution/loader.h"
#include "resolution/exception.h"
#include "shared/utils.h"

namespace Resolution {
  ModulesLoader::ModulesLoader() {
    this->lexer = Lexer::Lexer();
    this->parser = Parser::Parser();
    this->registry = ModulesRegistry();
    this->pathAliases = {};
    this->loadingModulesPaths = {};
  }

  Module* ModulesLoader::getModuleByAbsolutePath(std::string absolutePath) {
    std::string sourceCode = this->readModuleSourceCodeByAbsolutePath(absolutePath);

    // procedures to parse source code
    std::vector<Lexer::Token> tokens = this->lexer.parse(sourceCode);
    AST::BlockStatement* content = this->parser.parse(tokens);

    // get dependencies
    std::vector<std::string> dependencies = this->getModuleDependencies(absolutePath, content);

    return new Module(absolutePath, dependencies, content);
  }
  std::vector<std::string> ModulesLoader::getModuleDependencies(std::string absolutePath, AST::BlockStatement* content) {
    // get module content statements
    std::vector<AST::Statement*> statements = content->getStatements();
    // init module dependencies
    std::vector<std::string> dependencies = {};

    for (int i = 0; i < statements.size(); i++) {
      // skip non-import statements
      if (!Shared::isInstanceOf<AST::Statement, AST::ImportStatement>(statements[i])) continue;
    
      // cast statement type
      AST::ImportStatement* importStatement = Shared::cast<AST::Statement, AST::ImportStatement>(statements[i]);

      // get dependency absolute path
      std::string dependencyAbsolutePath = this->resolveAbsolutePath(absolutePath, importStatement->getPath().getCode());
     
      // validate dependency path
      if (!dependencyAbsolutePath.size()) {
        Base::Position position = importStatement->getPath().getPosition();
        std::string message = absolutePath + ": invalid path alias used";
        throw Resolution::PathException(position, message);
      }
      
      // skip duplicated imports
      if (Shared::includes(dependencies, dependencyAbsolutePath)) continue;

      // add new dependency path
      dependencies.push_back(dependencyAbsolutePath);
    }

    return dependencies;
  }
  std::string ModulesLoader::readModuleSourceCodeByAbsolutePath(std::string absolutePath) {
    return Shared::readFileByAbsolutePath(absolutePath);
  }

  std::string ModulesLoader::resolveAbsolutePath(std::string basePath, std::string dependencyPath) {
    // get base directory path
    std::filesystem::path baseDirectoryPath = std::filesystem::path(basePath).parent_path(); 
    // get default full path (join with dependency path)
    std::filesystem::path fullPath = baseDirectoryPath / dependencyPath;

    // check aliases
    if (Shared::startsWith(dependencyPath, ALIAS_SYMBOL)) {
      bool isAliasResolved = false;

      // iterate through aliases
      for (const auto& [key, value]: this->pathAliases) {
        // skip unmatched aliases
        if (!Shared::startsWith(dependencyPath, key)) continue;

        // replace alias with real path
        std::string resolvedDependencyPath = value + dependencyPath.substr(key.size());
        // update full path 
        fullPath = baseDirectoryPath / resolvedDependencyPath;

        // stop iteration
        break;
      }

      // return empty string if alias is not resolved
      if (!isAliasResolved) return "";
    }

    return std::filesystem::weakly_canonical(fullPath).string();
  }

  void ModulesLoader::loadPathAliases(std::unordered_map<std::string, std::string> aliases) {
    this->pathAliases = aliases;
  }

  void ModulesLoader::loadModulesFromEntrypointPath(std::string path) {
    // do not load module again
    if (this->registry.isModuleAddedByPath(path)) return;
    
    // circular dependency found
    if (Shared::includes(this->loadingModulesPaths, path)) {
      std::string message = "Circular dependency includes module: " + path;
      throw Resolution::Exception(message);
    }

    // load module
    Module* parsedModule = this->getModuleByAbsolutePath(path);
    this->registry.addModule(parsedModule);

    this->loadingModulesPaths.push_back(parsedModule->getAbsolutePath());

    // load dependencies
    for (int i = 0; i < parsedModule->getDependenciesPaths().size(); i++) {
      this->loadModulesFromEntrypointPath(parsedModule->getDependenciesPaths()[i]);
    }

    Shared::removeAll(this->loadingModulesPaths, parsedModule->getAbsolutePath());
  }

  std::vector<Module*> ModulesLoader::getModules() {
    return this->registry.getModules();
  }
}
