#pragma once

#include <stdexcept>
#include <string>

#include "base/position.h"

namespace Base {
  // exception class
  // all interpreter exceptions instantiate this class
  class Exception: public std::exception {  
    protected:
      Position position;
      std::string message;
    
    public:
      Exception(const Position, const std::string);
      // makes class polymorphic
      virtual ~Exception() = default;

      Position getPosition();
      std::string getMessage();
  };
}
