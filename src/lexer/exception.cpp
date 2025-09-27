#include "lexer/exception.h"

namespace Lexer {
  Exception::Exception(const Base::Position position, const std::string message): Base::Exception(position, message) {
    this->position = position;
    this->message = message;
  }
}
