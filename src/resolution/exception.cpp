#include "resolution/exception.h"

namespace Resolution {
  Exception::Exception(const std::string message): Base::Exception(message) {
    this->message = message;
  }

  PathException::PathException(const Base::Position position, const std::string message): Base::SourceCodeException(position, message) {
    this->position = position;
    this->message = message;
  }
}
