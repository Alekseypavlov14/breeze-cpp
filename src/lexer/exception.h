#pragma once

#include <string>

#include "base/exception.h"

namespace Lexer {
  class Exception: public Base::SourceCodeException {
    public:
      Exception(const Base::Position, const std::string);
  };
}

