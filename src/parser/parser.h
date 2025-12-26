#pragma once

#include <vector>
#include <map>

#include "parser/ast.h"

// this module contains parser 
// it converts token list to AST tree
namespace Parser {
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
      std::vector<AST::FunctionParameterExpression*> parseFunctionParameterExpressionList();
      AST::FunctionParameterExpression* parseFunctionParameterExpression();
      AST::ReturnStatement* parseReturnStatement(std::vector<Specification::TokenType> terminators);
      AST::ImportStatement* parseImportStatement();
      AST::ExportStatement* parseExportStatement(std::vector<Specification::TokenType> terminators);
      AST::BlockStatement* parseBlockStatement();
      AST::ExpressionStatement* parseExpressionStatement(std::vector<Specification::TokenType> terminators);
      AST::ClassDeclarationStatement* parseClassDeclarationStatement();
      AST::ClassMemberDeclarationStatement* parseClassMemberDeclarationStatement();
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
      // utils to get tokens type
      bool isLiteralToken(Specification::TokenType type);
      bool isIdentifierToken(Specification::TokenType type);

      // reusable utils
      void requireNewlineForNextStatement();
      void skipSingleLineSpaceTokens();
      void skipMultilineSpaceTokens();

      // analyzing utils
      void requireToken(Specification::TokenType);
      void requireTokens(std::vector<Specification::TokenType>);

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
}
