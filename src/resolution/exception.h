#pragma once

#include "base/exception.h"

namespace Resolution {
  class Exception: public Base::Exception {
    public:
      Exception(const std::string);
  };

  class PathException: public Base::SourceCodeException {
    public:
      PathException(const Base::Position, const std::string);
  };
}
