#pragma once

#include <string>

#include "base/position.h"
#include "specification/specification.h"

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
}