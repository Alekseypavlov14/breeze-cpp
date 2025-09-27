#pragma once 

#include "base/exception.h"

namespace Parser {
  class Exception: public Base::Exception {
    public:
      Exception(const Base::Position, const std::string);
  };
}
