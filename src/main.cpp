#include <iostream>

#include "lexer/lexer.h"
#include "lexer/exception.h"
#include "specification/specification.h"

int main() {
  Lexer::Lexer lexer;

  try {
    std::vector<Lexer::Token> tokens = lexer.parse("a = 3\nb = 4");
    std::cout << tokens.size() << std::endl;
  }
  catch (Lexer::Exception e) {
    std::cout << e.getMessage() << std::endl;
  }

  return 0;
}
