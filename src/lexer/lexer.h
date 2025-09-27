#pragma once

#include <string>
#include <vector>

#include "lexer/token.h"
#include "base/position.h"
#include "specification/specification.h"

// this module contains lexer logic 
// lexer parses source code to token list
namespace Lexer {
  class Lexer {
    private:
      // source code
      std::string code;
      // analyzing position
      int position;

      // computes token position based on analyzing position
      Base::Position computeCurrentTokenPosition();

      // updates code and position fields
      void loadCode(std::string code);

      // moves analyzing position
      void movePositionByDelta(int delta);

    public:
      Lexer();
    
      std::vector<Token> parse(std::string code);
  };
}
