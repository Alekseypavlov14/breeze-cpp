#pragma once

#include <stdexcept>
#include <string>

#include "base/position.h"

namespace Base {
  // exception class
  // all interpreter exceptions instantiate this class
  class Exception: public std::exception {  
    protected:
      std::string message;
    
    public:
      Exception(const std::string);
      // makes class polymorphic
      virtual ~Exception() = default;

      std::string getMessage();
  };
    
  // source code exceptions have position
  class SourceCodeException: public Exception {
    protected:
      Position position;
    
    public:
      SourceCodeException(const Position, const std::string);

      Position getPosition();
  };
}
