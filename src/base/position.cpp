#include "base/position.h"

namespace Base {
  Position::Position() {
    this->line = 0;
    this->column = 0;
  }
  Position::Position(int line, int column) {
    this->line = line;
    this->column = column;
  }
  Position::Position(const Position& position) {
    this->line = position.line;
    this->column = position.column;
  }

  int Position::getLine() {
    return this->line;
  }
  int Position::getColumn() {
    return this->column;
  }
  
  std::string Position::serialize() {
    std::string serialized = "";

    serialized += "(";
    serialized += std::to_string(this->line);
    serialized += "; ";
    serialized += std::to_string(this->column);
    serialized += ")";

    return serialized;
  }
}