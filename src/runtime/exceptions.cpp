#include "runtime/exceptions.h"

namespace Runtime {
  NameException::NameException(Base::Position position, std::string message): Base::Exception(position, message) {
    this->position = position;
    this->message = message;
  }
}
