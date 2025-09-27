#include <regex>

#include "lexer.h"
#include "shared/shared.h"

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
    return Shared::includes(tokenTypes, this->type);
  }

  Lexer::Lexer() {
    this->code = "";
    this->position = 0;
  }
  void Lexer::Lexer::loadCode(std::string code) {
    this->code = code;
    this->position = 0;
  } 
  void Lexer::Lexer::movePositionByDelta(int delta) {
    this->position += delta;
  }
  Base::Position Lexer::computeCurrentTokenPosition() {
    int line = 1;
    int column = 1;

    for (int i = 0; i < this->position; i++) {
      if (this->code[i] == '\n') {
        line += 1;
        column = 1;
        continue;
      }

      column++;
    }

    Base::Position position(line, column);

    return position;
  }
  std::vector<Token> Lexer::parse(std::string code) {
    // load source code string
    this->loadCode(code);

    // create tokens vector
    std::vector<Token> tokens = {};

    // iterate through given code string
    while (this->position < this->code.length()) {
      bool isTokenSpecificationFound = false;

      // check every specification
      for (int i = 0; i < Specification::TOKEN_SPECIFICATIONS.size(); i++) {
        Specification::TokenSpecification specification = Specification::TOKEN_SPECIFICATIONS[i];

        // slice of code that is not analyzed
        std::string slice = Shared::slice(this->code, this->position, this->code.size());

        // compose regex 
        std::regex regex = std::regex("^" + specification.regex);
        std::smatch match;

        // if specification is not matched - skip specification
        if (!std::regex_search(slice, match, regex)) continue;

        // shift position by whole match length
        this->movePositionByDelta(match[0].str().size());

        // extract the smallest group (used to extract strings content)
        std::string smallestMatch = match[0].str();
        // check other groups if they contain smaller groups
        for (int i = 1; i < match.size(); i++) {
          if (match[i].str().length() < smallestMatch.length()) {
            smallestMatch = match[i].str();
          }
        }

        // initialize token type
        Specification::TokenType tokenType = specification.type;

        // check if keyword is parsed
        for (int i = 0; i < Specification::KEYWORDS.size(); i++) {
          if (smallestMatch == Specification::KEYWORDS[i]) {
            tokenType = Specification::MAP_KEYWORD_TO_TOKEN_TYPE.at(Specification::KEYWORDS[i]);
            break;
          }
        }

        // create token instance
        Base::Position position = this->computeCurrentTokenPosition();
        Token token(position, tokenType, smallestMatch);

        // add token to list
        tokens.push_back(token);

        // update flags
        isTokenSpecificationFound = true;

        // stop analyzing code on this position
        break;
      }

      // check if token is not specified
      if (!isTokenSpecificationFound) {
        Base::Position position = this->computeCurrentTokenPosition();
        std::string message = "Invalid token is found";

        throw LexerException(position, message);
      }
    }   
    
    return tokens;
  }

  LexerException::LexerException(const Base::Position position, const std::string message): Base::Exception(position, message) {
    this->position = position;
    this->message = message;
  }
}