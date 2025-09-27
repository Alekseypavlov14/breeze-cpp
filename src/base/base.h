#pragma once

#include <stdexcept>
#include <string>

// this module contains base classes for interpreter
// they are used in several interpreter modules
namespace Base {
  // describes position in source code
  class Position {
    private:
      int line;
      int column;

    public:
      Position();
      Position(int, int);
      Position(const Position&);

      int getLine();
      int getColumn();

      std::string serialize();
  }; 

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
