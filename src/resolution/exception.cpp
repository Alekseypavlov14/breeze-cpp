#include "resolution/exception.h"

namespace Resolution {
  Exception::Exception(const Base::Position position, const std::string message): Base::Exception(position, message) {
    this->position = position;
    this->message = message;
  }
  PathException::PathException(const Base::Position position, const std::string message): Base::Exception(position, message) {
    this->position = position;
    this->message = message;
  }
}
