#pragma once

#include "runtime/types.h"
#include "parser/ast.h"

#include <stdexcept>

namespace Runtime {
  class Signal: public std::exception {
    protected:
      AST::Statement* cause;

    public:
      virtual ~Signal() = default;

      AST::Statement* getCause();
  };

  // break signal
  class BreakSignal: public Signal {
    public:
      BreakSignal(AST::Statement*);
  };

  // continue signal
  class ContinueSignal: public Signal {
    public:
      ContinueSignal(AST::Statement*);
  };

  // return signal
  class ReturnSignal: public Signal {
    private:
      Value* value;

    public:
      ReturnSignal(AST::Statement*, Value*);
      Value* getValue();
  };
}
