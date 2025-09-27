#pragma once

#include "base/position.h"
#include "base/exception.h"

namespace Runtime {
  class NameException: public Base::Exception {
    public:
      NameException(Base::Position, std::string);
  };
}
