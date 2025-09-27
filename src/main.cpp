#include <iostream>

#include "lexer/lexer.h"
#include "base/specification.h"

int main() {
  Lexer::Lexer lexer;

  try {
    std::vector<Lexer::Token> tokens = lexer.parse("a = 3\nb = 4");
    std::cout << tokens.size() << std::endl;
  }
  catch (Lexer::LexerException e) {
    std::cout << e.getMessage() << std::endl;
  }

  return 0;
}
