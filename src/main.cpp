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
  
    executor.loadModulesFromEntrypoint("C:/Users/User/Desktop/programming-language/cpp/test/app.br");
    executor.registerBuiltins(Builtins::declarations);

    executor.execute();
  } catch(Parser::Exception e) {
    std::cout << "ParserException: " << e.getMessage() << e.getPosition().serialize() << std::endl;
  } catch(Base::Exception e) {
    std::cout << "Message: " << e.getMessage() << std::endl;
  } catch(Base::SourceCodeException e) {
    std::cout << "Message: " << e.getMessage() << ", position: " << e.getPosition().serialize() << std::endl;
  } catch(Runtime::Exception e) {
    std::cout << "Runtime exception" << std::endl;
  } catch(Resolution::Exception e) {
    std::cout << "Resolution exception" << std::endl;
  } catch(std::exception e) {
    std::cout << "Oops" << e.what() << std::endl;
  }

  std::cout << "=== Execution ends ===" << std::endl;

  return 0;
}
