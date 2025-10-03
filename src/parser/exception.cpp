#include "parser/exception.h"

namespace Parser {
  Exception::Exception(const Base::Position position, const std::string message): Base::SourceCodeException(position, message) {
    this->position = position;
    this->message = message;
  }
}
