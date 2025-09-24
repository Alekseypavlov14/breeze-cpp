#pragma once

#include <vector>
#include <map>

#include "ast.h"
#include "base.h"

// this module contains parser 
// it converts token list to AST tree
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

  // literal tokens
  inline const std::vector<Specification::TokenType> LITERAL_TOKENS = {
    Specification::TokenType::NUMBER_TOKEN,
    Specification::TokenType::STRING_TOKEN,
    Specification::TokenType::TRUE_KEYWORD_TOKEN,
    Specification::TokenType::FALSE_KEYWORD_TOKEN,
    Specification::TokenType::NULL_KEYWORD_TOKEN,
  };

  bool isLiteralToken(Specification::TokenType type);
  bool isIdentifierToken(Specification::TokenType type);

  class Parser {
    private:
      // list of analyzing tokens
      std::vector<Lexer::Token> tokens;
      // position pointer
      int position;

      // parses single statement
      AST::Statement* parseStatement(std::vector<Specification::TokenType> terminators);
  
      AST::VariableDeclarationStatement* parseVariableDeclaration(std::vector<Specification::TokenType> terminators);
      AST::ConstantDeclarationStatement* parseConstantDeclaration(std::vector<Specification::TokenType> terminators);
      AST::ConditionStatement* parseConditionStatement(std::vector<Specification::TokenType> terminators);
      AST::ForStatement* parseForStatement(std::vector<Specification::TokenType> terminators);
      AST::WhileStatement* parseWhileStatement(std::vector<Specification::TokenType> terminators);
      AST::BreakStatement* parseBreakStatement();
      AST::ContinueStatement* parseContinueStatement();
      AST::FunctionDeclarationStatement* parseFunctionDeclarationStatement();
      AST::FunctionParameterExpression* parseFunctionParameterExpression();
      AST::ReturnStatement* parseReturnStatement(std::vector<Specification::TokenType> terminators);
      AST::ClassDeclarationStatement* parseClassDeclarationStatement();
      AST::ImportStatement* parseImportStatement();
      AST::ExportStatement* parseExportStatement(std::vector<Specification::TokenType> terminators);
      AST::BlockStatement* parseBlockStatement();
      AST::ExpressionStatement* parseExpressionStatement(std::vector<Specification::TokenType> terminators);
      AST::NullStatement* parseComment();

      // statement matching utils
      bool matchVariableDeclarationStatement();
      bool matchConstantDeclarationStatement();
      bool matchConditionStatement();
      bool matchForStatement();
      bool matchWhileStatement();
      bool matchBreakStatement();
      bool matchContinueStatement();
      bool matchFunctionDeclarationStatement();
      bool matchReturnStatement();
      bool matchClassDeclarationStatement();
      bool matchImportStatement();
      bool matchExportStatement();
      bool matchBlockStatement();
      bool matchComment();

      // parses expression methods
      // these methods implement parsing of Token list to AST using recursion
      // AST is composed step-by-step and its nodes are added recursively
      // baseExpression gives a current state of AST fragment
      // basePrecedence contains precedence of the last visited operator. Used to normalize the tree by operator precedence
      // terminators specify the token types that force analyzing to stop if the AST is stable (the fragment is completed)
      AST::Expression* parseExpression(AST::Expression* baseExpression, int basePrecedence, std::vector<Specification::TokenType> terminators);
      // used to compose an expression
      // priority is given to token list
      // otherwise base expression is returned
      AST::Expression* getExpressionFromBaseExpresionOrPassedTokens(AST::Expression*, std::vector<Lexer::Token>);
      // this method parses defined range of token to AST node
      // used only if the amount of tokens is defined
      // usually used for a small amount of tokens
      AST::Expression* parseExpressionFromTokens(std::vector<Lexer::Token>);

      // reusable utils
      void requireNewlineForNextStatement();
      void skipSingleLineSpaceTokens();
      void skipMultilineSpaceTokens();

      // analyzing utils
      void requireToken(Specification::TokenType);

      bool matchToken(Specification::TokenType);
      bool matchTokens(std::vector<Specification::TokenType>);

      void skipToken(Specification::TokenType);
      void skipTokens(std::vector<Specification::TokenType>);

      // utils to operate this.tokens vector
      Lexer::Token consumeCurrentToken();
      Lexer::Token getCurrentToken();
      Lexer::Token getPreviousToken();
  
      // move position pointer
      void incrementPosition();
      void decrementPosition();

      // checks if the end is reached
      bool isEnd();

    public:
      Parser();

      // parses vector of tokens (corresponds to source code module)
      AST::BlockStatement* parse(std::vector<Lexer::Token>);
  };

  class ParserException: public Base::Exception {
    public:
      ParserException(const Base::Position, const std::string);
  };
}
