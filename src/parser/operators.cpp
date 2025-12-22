#include "parser/operators.h"
#include "shared/utils.h"

namespace Parser {
  // returns relative numerical value for operator precedence
  int getOperatorPrecedence(Specification::TokenType type) {
    for (int i = 0; i < OPERATION_PRECEDENCE.size(); i++) {
      if (type == OPERATION_PRECEDENCE[i]) {
        return i;
      }
    }

    return BASE_PRECEDENCE;
  }

  // utilities for checking operator type
  bool isPrefixOperator(Specification::TokenType type) {
    return Shared::includes<Specification::TokenType>(PREFIX_UNARY_OPERATORS, type);
  }
  bool isSuffixOperator(Specification::TokenType type) {
    return Shared::includes<Specification::TokenType>(SUFFIX_UNARY_OPERATORS, type);
  }
  bool isAffixUnaryOperator(Specification::TokenType type) {
    return Shared::includes<Specification::TokenType>(AFFIX_UNARY_OPERATORS, type);
  }
  bool isUnaryOperator(Specification::TokenType type) {
    return isPrefixOperator(type) || isSuffixOperator(type) || isAffixUnaryOperator(type);
  }
  bool isBinaryOperator(Specification::TokenType type) {
    return Shared::includes<Specification::TokenType>(BINARY_OPERATORS, type);
  }
  bool isGroupingOperator(Specification::TokenType type) {
    return Shared::includes<Specification::TokenType>(GROUPING_OPERATORS, type);
  }
  bool isAssociationOperator(Specification::TokenType type) {
    return Shared::includes<Specification::TokenType>(ASSOCIATION_OPERATORS, type);
  }

  // gets the closing token for groupings
  Specification::TokenType getGroupingClosingTokenType(Specification::TokenType tokenType) {
    return GROUPING_OPERATOR_PAIRS.at(tokenType);
  }

  bool isRightAssociativeOperator(Specification::TokenType op) {
    return Shared::includes(RIGHT_ASSOCIATIVE_OPERATORS, op);
  }
  bool isLeftAssociativeOperator(Specification::TokenType op) {
    return !isRightAssociativeOperator(op);
  }
}