#include <regex>

#include "lexer/lexer.h"
#include "lexer/exception.h"
#include "shared/strings.h"

namespace Lexer {
  Lexer::Lexer() {
    this->code = "";
    this->position = 0;
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
        std::string slice = Shared::Strings::slice(this->code, this->position, this->code.size());

        // compose regex 
        std::regex regex = std::regex("^" + specification.regex);
        std::smatch match;

        // if specification is not matched - skip specification
        if (!std::regex_search(slice, match, regex, std::regex_constants::match_continuous)) continue;
        
        // shift position by whole match length
        this->movePositionByDelta(match[0].str().size());

        // extract the smallest group (used to extract strings content)
        std::string smallestMatch = match[0].str();
        // check other groups if they contain smaller groups
        for (int i = 1; i < match.size(); i++) {
          if (match[i].str().length() < smallestMatch.length() && match[i].str().size() > 0) {
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

        throw Exception(position, message);
      }
    }   
    
    return tokens;
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
}
