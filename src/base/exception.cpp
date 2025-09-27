#include "base/exception.h"

namespace Base {
  // exception implementations
  Exception::Exception(const Position position, const std::string message) {
    this->position = position;
    this->message = message;
  }
  
  Position Exception::getPosition() {
    return this->position;
  }
  std::string Exception::getMessage() {
    return this->message;
  }
}
