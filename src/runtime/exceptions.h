#pragma once

#include "base/position.h"
#include "base/exception.h"

namespace Runtime {
  class Exception: public Base::Exception {
    public:
      Exception(std::string message);
  };
  
  class NameException: public Base::SourceCodeException {
    public:
      NameException(const Base::Position, std::string);
  };

  class TypeException: public Base::SourceCodeException {
    public:
      TypeException(const Base::Position, std::string);
  };

  class StatementException: public Base::SourceCodeException {
    public:
      StatementException(const Base::Position, std::string);
  };

  class ExpressionException: public Base::SourceCodeException {
    public:
      ExpressionException(const Base::Position, std::string);
  };
}
