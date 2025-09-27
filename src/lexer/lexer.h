#pragma once

#include <string>
#include <vector>

#include "base/specification.h"
#include "base/base.h"
#include "shared/shared.h"

// this module contains lexer logic 
// lexer parses source code to token list
namespace Lexer {
  class Token {
    private:
      // token position
      Base::Position position;
      // type of token by specification
      Specification::TokenType type;
      // slide of source code
      std::string code;

    public:
      Token(const Base::Position, const Specification::TokenType, const std::string);
      Token(const Token&);

      Base::Position getPosition();
      Specification::TokenType getType();
      std::string getCode();

      bool isOfType(Specification::TokenType);
      bool isOfType(std::vector<Specification::TokenType>);
  };

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

  class LexerException: public Base::Exception {
    public:
      LexerException(const Base::Position, const std::string);
  };
}
