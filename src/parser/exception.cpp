#include "parser/exception.h"

namespace Parser {
  Exception::Exception(const Base::Position position, const std::string message): Base::Exception(position, message) {
    this->position = position;
    this->message = message;
  }
}
