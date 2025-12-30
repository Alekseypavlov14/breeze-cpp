#include <iostream>

#include "runtime/executor.h"
#include "resolution/exception.h"
#include "resolution/loader.h"
#include "parser/parser.h"
#include "parser/exception.h"
#include "lexer/lexer.h"
#include "base/exception.h"

int main() {
  std::cout << "=== Execution starts ===" << std::endl;

  try {
    Runtime::Executor executor;
  
    executor.loadModulesFromEntrypoint("C:/Users/User/Desktop/programming-language/cpp/test/functions-declarations.br");
    executor.registerBuiltins(Builtins::declarations);

    executor.execute();
  } catch(Parser::Exception e) {
    std::cout << "ParserException: " << e.getMessage() << e.getPosition().serialize() << std::endl;
  } catch(Base::Exception e) {
    std::cout << "Message: " << e.getMessage() << std::endl;
  } 

  std::cout << "=== Execution ends ===" << std::endl;

  return 0;
}
