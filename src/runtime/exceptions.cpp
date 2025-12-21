#include "runtime/exceptions.h"

namespace Runtime {
  Exception::Exception(std::string message): Base::Exception(message) {
    this->message = message;
  }

  NameException::NameException(std::string message): Base::Exception(message) {
    this->message = message;
  }

  TypeException::TypeException(std::string message): Base::Exception(message) {
    this->message = message;
  }
}
