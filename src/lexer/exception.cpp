#include "lexer/exception.h"

namespace Lexer {
  Exception::Exception(const Base::Position position, const std::string message): Base::SourceCodeException(position, message) {
    this->position = position;
    this->message = message;
  }
}
