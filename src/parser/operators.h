#pragma once

#include <vector>

#include "specification/specification.h"

namespace Parser {
  // Operator precedence (from most to least important)
  inline const std::vector<Specification::TokenType> OPERATION_PRECEDENCE = {
    Specification::TokenType::LAMBDA_TOKEN,

    Specification::TokenType::BIT_AND_ASSIGN_TOKEN,
    Specification::TokenType::BIT_OR_ASSIGN_TOKEN,
    Specification::TokenType::BIT_XOR_ASSIGN_TOKEN,
    Specification::TokenType::LEFT_SHIFT_ASSIGN_TOKEN,
    Specification::TokenType::RIGHT_SHIFT_ASSIGN_TOKEN,

    Specification::TokenType::REMAINDER_ASSIGN_TOKEN,
    Specification::TokenType::DIVISION_ASSIGN_TOKEN,
    Specification::TokenType::MULTIPLICATION_ASSIGN_TOKEN,
    Specification::TokenType::EXPONENTIAL_ASSIGN_TOKEN,

    Specification::TokenType::MINUS_ASSIGN_TOKEN,
    Specification::TokenType::PLUS_ASSIGN_TOKEN,

    Specification::TokenType::ASSIGN_TOKEN,

    Specification::TokenType::OR_TOKEN,
    Specification::TokenType::AND_TOKEN,

    Specification::TokenType::BIT_OR_TOKEN,
    Specification::TokenType::BIT_XOR_TOKEN,
    Specification::TokenType::BIT_AND_TOKEN,

    Specification::TokenType::NOT_EQUAL_TOKEN,
    Specification::TokenType::EQUAL_TOKEN,

    Specification::TokenType::GREATER_THAN_OR_EQUAL_TOKEN,
    Specification::TokenType::GREATER_THAN_TOKEN,
    Specification::TokenType::LESS_THAN_OR_EQUAL_TOKEN,
    Specification::TokenType::LESS_THAN_TOKEN,

    Specification::TokenType::LEFT_SHIFT_TOKEN,
    Specification::TokenType::RIGHT_SHIFT_TOKEN,

    Specification::TokenType::MINUS_TOKEN,
    Specification::TokenType::PLUS_TOKEN,

    Specification::TokenType::REMAINDER_TOKEN,
    Specification::TokenType::DIVISION_TOKEN,
    Specification::TokenType::MULTIPLICATION_TOKEN,
    Specification::TokenType::EXPONENTIAL_TOKEN,

    Specification::TokenType::BIT_NOT_TOKEN,
    Specification::TokenType::NOT_TOKEN,

    Specification::TokenType::INCREMENT_TOKEN,
    Specification::TokenType::DECREMENT_TOKEN,

    Specification::TokenType::LEFT_SQUARE_BRACKET_TOKEN,
    Specification::TokenType::LEFT_PARENTHESES_TOKEN,

    Specification::TokenType::NEW_KEYWORD_TOKEN,

    Specification::TokenType::DOT_TOKEN,

    Specification::TokenType::LEFT_CURLY_BRACE_TOKEN
  };

  // specifies the default precedence for AST composition
  // this precedence is higher that every other operator has
  inline const int BASE_PRECEDENCE = -1;

  // returns the numerical value of token precedence 
  // returns index of token type in precedence array
  int getOperatorPrecedence(Specification::TokenType type);

  // types of operators
  inline const std::vector<Specification::TokenType> PREFIX_UNARY_OPERATORS = {
    Specification::TokenType::BIT_NOT_TOKEN,
    Specification::TokenType::NOT_TOKEN,
  };
  inline const std::vector<Specification::TokenType> SUFFIX_UNARY_OPERATORS = {
    Specification::TokenType::NEW_KEYWORD_TOKEN,
  };
  inline const std::vector<Specification::TokenType> AFFIX_UNARY_OPERATORS = {
    Specification::TokenType::INCREMENT_TOKEN,
    Specification::TokenType::DECREMENT_TOKEN,
  };
  inline const std::vector<Specification::TokenType> BINARY_OPERATORS = {
    Specification::TokenType::LAMBDA_TOKEN,

    Specification::TokenType::BIT_AND_ASSIGN_TOKEN,
    Specification::TokenType::BIT_OR_ASSIGN_TOKEN,
    Specification::TokenType::BIT_XOR_ASSIGN_TOKEN,
    Specification::TokenType::LEFT_SHIFT_ASSIGN_TOKEN,
    Specification::TokenType::RIGHT_SHIFT_ASSIGN_TOKEN,

    Specification::TokenType::REMAINDER_ASSIGN_TOKEN,
    Specification::TokenType::DIVISION_ASSIGN_TOKEN,
    Specification::TokenType::MULTIPLICATION_ASSIGN_TOKEN,
    Specification::TokenType::EXPONENTIAL_ASSIGN_TOKEN,
    
    Specification::TokenType::MINUS_ASSIGN_TOKEN,
    Specification::TokenType::PLUS_ASSIGN_TOKEN,

    Specification::TokenType::ASSIGN_TOKEN,

    Specification::TokenType::OR_TOKEN,
    Specification::TokenType::AND_TOKEN,

    Specification::TokenType::BIT_OR_TOKEN,
    Specification::TokenType::BIT_XOR_TOKEN,
    Specification::TokenType::BIT_AND_TOKEN,

    Specification::TokenType::NOT_EQUAL_TOKEN,
    Specification::TokenType::EQUAL_TOKEN,

    Specification::TokenType::GREATER_THAN_OR_EQUAL_TOKEN,
    Specification::TokenType::GREATER_THAN_TOKEN,
    Specification::TokenType::LESS_THAN_OR_EQUAL_TOKEN,
    Specification::TokenType::LESS_THAN_TOKEN,

    Specification::TokenType::LEFT_SHIFT_TOKEN,
    Specification::TokenType::RIGHT_SHIFT_TOKEN,

    Specification::TokenType::MINUS_TOKEN,
    Specification::TokenType::PLUS_TOKEN,

    Specification::TokenType::REMAINDER_TOKEN,
    Specification::TokenType::DIVISION_TOKEN,
    Specification::TokenType::MULTIPLICATION_TOKEN,

    Specification::TokenType::EXPONENTIAL_TOKEN,
    Specification::TokenType::DOT_TOKEN,
  };
  inline const std::vector<Specification::TokenType> GROUPING_OPERATORS = {
    Specification::TokenType::LEFT_SQUARE_BRACKET_TOKEN,
    Specification::TokenType::LEFT_PARENTHESES_TOKEN,
  };
  inline const std::vector<Specification::TokenType> ASSOCIATION_OPERATORS = {
    Specification::TokenType::LEFT_CURLY_BRACE_TOKEN,
  };

  // utils to check operator type
  bool isPrefixOperator(Specification::TokenType type);
  bool isSuffixOperator(Specification::TokenType type);
  bool isAffixUnaryOperator(Specification::TokenType type);
  bool isUnaryOperator(Specification::TokenType type);
  bool isBinaryOperator(Specification::TokenType type);
  bool isGroupingOperator(Specification::TokenType type);
  bool isAssociationOperator(Specification::TokenType type);

  // pairs of tokens
  inline const std::map<Specification::TokenType, Specification::TokenType> GROUPING_OPERATOR_PAIRS = {
    { Specification::TokenType::LEFT_PARENTHESES_TOKEN, Specification::TokenType::RIGHT_PARENTHESES_TOKEN },
    { Specification::TokenType::LEFT_SQUARE_BRACKET_TOKEN, Specification::TokenType::RIGHT_SQUARE_BRACKET_TOKEN },
  };

  Specification::TokenType getGroupingClosingTokenType(Specification::TokenType);

  // associativity of operators
  inline const std::vector<Specification::TokenType> RIGHT_ASSOCIATIVE_OPERATORS = {
    Specification::TokenType::LAMBDA_TOKEN,

    Specification::TokenType::ASSIGN_TOKEN,
    Specification::TokenType::PLUS_ASSIGN_TOKEN,
    Specification::TokenType::MINUS_ASSIGN_TOKEN,
    Specification::TokenType::MULTIPLICATION_ASSIGN_TOKEN,
    Specification::TokenType::DIVISION_ASSIGN_TOKEN,
    Specification::TokenType::REMAINDER_ASSIGN_TOKEN,
    Specification::TokenType::EXPONENTIAL_ASSIGN_TOKEN,

    Specification::TokenType::BIT_AND_ASSIGN_TOKEN,
    Specification::TokenType::BIT_OR_ASSIGN_TOKEN,
    Specification::TokenType::BIT_XOR_ASSIGN_TOKEN,
    Specification::TokenType::LEFT_SHIFT_ASSIGN_TOKEN,
    Specification::TokenType::RIGHT_SHIFT_ASSIGN_TOKEN,

    Specification::TokenType::EXPONENTIAL_TOKEN,

    Specification::TokenType::NOT_TOKEN,
    Specification::TokenType::BIT_NOT_TOKEN,
    Specification::TokenType::NEW_KEYWORD_TOKEN
  };

  bool isRightAssociativeOperator(Specification::TokenType);
  bool isLeftAssociativeOperator(Specification::TokenType);
}
