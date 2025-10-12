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
      NameException(Base::Position, std::string);
  };
}
