#include "runtime/signals.h"

namespace Runtime {
  AST::Statement* Signal::getCause() {
    return this->cause;
  }

  BreakSignal::BreakSignal(AST::Statement* cause) {
    this->cause = cause;
  }

  ContinueSignal::ContinueSignal(AST::Statement* cause) {
    this->cause = cause;
  }

  ReturnSignal::ReturnSignal(AST::Statement* cause, Value* value) {
    this->cause = cause;
    this->value = value;
  }
  Value* ReturnSignal::getValue() {
    return this->value;
  }
}
