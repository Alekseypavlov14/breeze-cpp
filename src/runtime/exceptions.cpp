#include "runtime/exceptions.h"

namespace Runtime {
  Exception::Exception(std::string message): Base::Exception(message) {
    this->message = message;
  }

  NameException::NameException(Base::Position position, std::string message): Base::SourceCodeException(position, message) {
    this->position = position;
    this->message = message;
  }
}
