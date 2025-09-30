#pragma once

#include "base/exception.h"

namespace Resolution {
  class Exception: public Base::Exception {
    public:
      Exception(const Base::Position, const std::string);
  };

  class PathException: public Base::Exception {
    public:
      PathException(const Base::Position, const std::string);
  };
}
