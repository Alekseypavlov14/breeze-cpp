#include <iostream>

#include "runtime/executor.h"
#include "resolution/loader.h"
#include "parser/parser.h"
#include "parser/exception.h"
#include "lexer/lexer.h"
#include "base/exception.h"
#include "shared/utils.h"

int main() {
  std::cout << "=== Execution starts ===" << std::endl;

  try {
    Resolution::ModulesLoader loader;
    Runtime::Executor executor(loader);
  
    loader.loadModulesFromEntrypointPath("C:/Users/User/Desktop/programming-language/cpp/test/app.br");
    std::vector<Resolution::Module*> modules = loader.getModules();
  
    executor.execute();
  } catch(Parser::Exception e) {
    std::cout << "ParserException: " << e.getMessage() << e.getPosition().serialize() << std::endl;
  } catch(Base::Exception e) {
    std::cout << "Message: " << e.getMessage() << std::endl;
  } catch(Base::SourceCodeException e) {
    std::cout << "Message: " << e.getMessage() << ", position: " << e.getPosition().serialize() << std::endl;
  } catch(...) {
    std::cout << "Oops" << std::endl;
  }

  std::cout << "=== Execution ends ===" << std::endl;

  return 0;
}
