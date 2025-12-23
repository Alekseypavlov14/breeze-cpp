#include "runtime/exceptions.h"

namespace Runtime {
  Exception::Exception(std::string message): Base::Exception(message) {
    this->message = message;
  }

  NameException::NameException(const Base::Position position, std::string message): Base::SourceCodeException(position, message) {
    this->position = position;
    this->message = message;
  }

  TypeException::TypeException(const Base::Position position, std::string message): Base::SourceCodeException(position, message) {
    this->position = position;
    this->message = message;
  }

  StatementException::StatementException(const Base::Position position, std::string message): Base::SourceCodeException(position, message) {
    this->position = position;
    this->message = message;
  }

  ExpressionException::ExpressionException(const Base::Position position, std::string message): Base::SourceCodeException(position, message) {
    this->position = position;
    this->message = message;
  }
}
