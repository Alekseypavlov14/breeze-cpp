#include <iostream>

#include "runtime/executor.h"
#include "resolution/loader.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include "shared/utils.h"

int main() {
  std::cout << "=== Execution starts ===" << std::endl;

  Resolution::ModulesLoader loader;
  Runtime::Executor executor(loader);

  loader.loadModulesFromEntrypointPath("C:/Users/User/Desktop/programming-language/cpp/test/app.br");
  std::vector<Resolution::Module*> modules = loader.getModules();

  executor.execute();

  std::cout << "=== Execution ends ===" << std::endl;

  return 0;
}
