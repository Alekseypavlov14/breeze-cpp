#pragma once

#include <string>

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
}