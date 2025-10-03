#include "base/exception.h"

namespace Base {
  Exception::Exception(const std::string message) {
    this->message = message;
  }
  std::string Exception::getMessage() {
    return this->message;
  }

  SourceCodeException::SourceCodeException(const Position position, const std::string message): Exception(message) {
    this->position = position;
    this->message = message;
  }
  Position SourceCodeException::getPosition() {
    return this->position;
  }
}
