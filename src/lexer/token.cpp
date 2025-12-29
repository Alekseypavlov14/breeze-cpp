#include "lexer/token.h"
#include "shared/vectors.h"

namespace Lexer {
  Token::Token(const Base::Position position, const Specification::TokenType type, const std::string code) {
    this->position = position;
    this->type = type;
    this->code = code;
  }
  Token::Token(const Token& token) {
    this->position = token.position;
    this->type = token.type;
    this->code = token.code;
  }

  Base::Position Token::getPosition() {
    return this->position;
  }
  Specification::TokenType Token::getType() {
    return this->type;
  }
  std::string Token::getCode() {
    return this->code;
  }
  
  bool Token::isOfType(Specification::TokenType tokenType) {
    return tokenType == this->type;
  }
  bool Token::isOfType(std::vector<Specification::TokenType> tokenTypes) {
    return Shared::Vectors::includes(tokenTypes, this->type);
  }
}
