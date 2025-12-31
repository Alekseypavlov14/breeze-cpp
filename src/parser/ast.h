#pragma once

#include <vector>
#include <map>
#include <string>

#include "lexer/token.h"
#include "specification/specification.h"

// this module declares hierarchy of classes for AST tree
// because the structure is tree, parent nodes are responsible for children memory
// destructors free memory for their children
namespace AST {
  class Node {
    protected:
      Base::Position position;

    public:
      // makes class polymorphic
      virtual ~Node() = default;
      virtual Node* clone() const = 0;

      Base::Position getPosition() const;
  };
  class Expression: public Node {
    public:
      virtual Expression* clone() const = 0;
  };
  class Statement: public Node {
    public:
      virtual Statement* clone() const = 0;
  };

  // expression variants
  class NullExpression: public Expression {
    public:
      NullExpression(Base::Position position);
      NullExpression* clone() const;
  };

  class OperationExpression: public Expression {};
  
  class UnaryOperationExpression: public OperationExpression {
    private:
      Lexer::Token operatorToken;
      Expression* operand;

    public:
      UnaryOperationExpression(Base::Position position, Lexer::Token operatorToken, Expression* operand);
      ~UnaryOperationExpression();

      Lexer::Token getOperator() const;
      Expression* getOperand() const;
  };

  class PrefixUnaryOperationExpression: public UnaryOperationExpression {
    public:
      PrefixUnaryOperationExpression(Base::Position position, Lexer::Token operatorToken, Expression* operand);
      PrefixUnaryOperationExpression* clone() const;
  };

  class SuffixUnaryOperationExpression: public UnaryOperationExpression {
    public:
      SuffixUnaryOperationExpression(Base::Position position, Lexer::Token operatorToken, Expression* operand);
      SuffixUnaryOperationExpression* clone() const;
  };

  class AffixUnaryOperationExpression: public UnaryOperationExpression {
    public:
      AffixUnaryOperationExpression(Base::Position position, Lexer::Token operatorToken, Expression* operand);
      AffixUnaryOperationExpression* clone() const;
  };

  class BinaryOperationExpression: public OperationExpression {
    private:
      Lexer::Token operatorToken;
      Expression* left;
      Expression* right;

    public:
      BinaryOperationExpression(Base::Position position, Lexer::Token operatorToken, Expression* left, Expression* right);
      ~BinaryOperationExpression();

      BinaryOperationExpression* clone() const;

      Lexer::Token getOperator() const;
      Expression* getLeft() const;
      Expression* getRight() const;
  };

  class LiteralExpression: public Expression {
    private:
      Lexer::Token value;

    public:
      LiteralExpression(Base::Position position, Lexer::Token value);

      LiteralExpression* clone() const;

      Lexer::Token getValue() const;
  };

  class IdentifierExpression: public Expression {
    private:
      Lexer::Token name;

    public:
      IdentifierExpression(Base::Position position, Lexer::Token name);

      IdentifierExpression* clone() const;

      Lexer::Token getName() const;
  };

  // for (), []
  class GroupingExpression: public Expression {
    private:
      Lexer::Token operatorToken;
      std::vector<Expression*> expressions;

    public:
      GroupingExpression(Base::Position position, Lexer::Token operatorToken, std::vector<Expression*> expressions);
      ~GroupingExpression();

      GroupingExpression* clone() const;

      Lexer::Token getOperator() const;
      std::vector<Expression*> getExpressions() const;
  };

  // used when expression is followed by grouping expression 
  class GroupingApplicationExpression: public Expression {
    private:
      Expression* left;
      GroupingExpression* right;

    public:
      GroupingApplicationExpression(Base::Position position, Expression* left, GroupingExpression* right);
      ~GroupingApplicationExpression();

      GroupingApplicationExpression* clone() const;

      Expression* getLeft() const;
      GroupingExpression* getRight() const;
  };

  // for {} maps
  class AssociationExpression: public Expression {
    private:
      std::vector<std::pair<Expression*, Expression*>> entries;

    public:
      AssociationExpression(Base::Position position, std::vector<std::pair<Expression*, Expression*>> entries);
      ~AssociationExpression();

      AssociationExpression* clone() const;

      std::vector<std::pair<Expression*, Expression*>> getEntries() const;
  };

  class FunctionParameterExpression: public Expression {
    private:
      Lexer::Token name;
      // NULL if no default value
      Expression* defaultValue;

    public:
      FunctionParameterExpression(Base::Position position, Lexer::Token name, Expression* defaultValue);
      ~FunctionParameterExpression();

      FunctionParameterExpression* clone() const;

      Lexer::Token getName() const;
      Expression* getDefaultValue() const;
  };

  // statement variants
  class NullStatement: public Statement {
    public:
      NullStatement(Base::Position position);

      NullStatement* clone() const;
  };

  class ExpressionStatement: public Statement {
    private:
      Expression* expression; 

    public:
      ExpressionStatement(Base::Position position, Expression* expression);
      ~ExpressionStatement();

      ExpressionStatement* clone() const;

      Expression* getExpression() const;
  };

  class BlockStatement: public Statement {
    private:
      std::vector<Statement*> statements;

    public:
      BlockStatement(Base::Position position, std::vector<Statement*> statements);
      ~BlockStatement();

      BlockStatement* clone() const;

      std::vector<Statement*> getStatements() const;
  };

  class VariableDeclarationStatement: public Statement {
    private:
      Lexer::Token name;
      Expression* initializer;

    public: 
      VariableDeclarationStatement(Base::Position position, Lexer::Token name, Expression* initializer);
      ~VariableDeclarationStatement();

      VariableDeclarationStatement* clone() const;

      Lexer::Token getName() const;
      Expression* getInitializer() const;
  };

  class ConstantDeclarationStatement: public Statement {
    private:
      Lexer::Token name;
      Expression* initializer;

    public:
      ConstantDeclarationStatement(Base::Position position, Lexer::Token name, Expression* initializer);
      ~ConstantDeclarationStatement();

      ConstantDeclarationStatement* clone() const;

      Lexer::Token getName() const;
      Expression* getInitializer() const;
  };

  class ConditionStatement: public Statement {
    private:
      Expression* condition;
      Statement* thenBranch;
      Statement* elseBranch;

    public:
      ConditionStatement(Base::Position position, Expression* condition, Statement* thenBranch, Statement* elseBranch);
      ~ConditionStatement();

      ConditionStatement* clone() const;

      Expression* getCondition() const;
      Statement* getThenBranch() const;
      Statement* getElseBranch() const;
  };

  class WhileStatement: public Statement {
    private:
      Expression* condition;
      Statement* body;

    public:
      WhileStatement(Base::Position position, Expression* condition, Statement* body);
      ~WhileStatement();

      WhileStatement* clone() const;
      
      Expression* getCondition() const;
      Statement* getBody() const;
  };

  class ForStatement: public Statement {
    private:
      Statement* initializer;
      Expression* condition;
      Expression* increment;
      Statement* body;

    public:
      ForStatement(Base::Position position, Statement* initializer, Expression* condition, Expression* increment, Statement* body);
      ~ForStatement();

      ForStatement* clone() const;

      Statement* getInitializer() const;
      Expression* getCondition() const;
      Expression* getIncrement() const;
      Statement* getBody() const;
  };

  class BreakStatement: public Statement {
    public:
      BreakStatement(Base::Position position);

      BreakStatement* clone() const;
  };
  class ContinueStatement: public Statement {
    public:
      ContinueStatement(Base::Position position);

      ContinueStatement* clone() const;
  };

  class FunctionDeclarationStatement: public Statement {
    private:
      Lexer::Token name;
      std::vector<FunctionParameterExpression*> params;
      BlockStatement* body;

    public:
      FunctionDeclarationStatement(Base::Position position, Lexer::Token name, std::vector<FunctionParameterExpression*> params, BlockStatement* body);
      ~FunctionDeclarationStatement();

      FunctionDeclarationStatement* clone() const;

      Lexer::Token getName() const;
      std::vector<FunctionParameterExpression*> getParams() const;
      BlockStatement* getBody() const;
  };

  class ReturnStatement: public Statement {
    private:
      Expression* returns;

    public:
      ReturnStatement(Base::Position position, Expression* returns);
      ~ReturnStatement();

      ReturnStatement* clone() const;

      Expression* getReturns() const;
  };

  class ImportStatement: public Statement {
    private:
      Lexer::Token path;
      std::vector<Lexer::Token> imports;

    public:
      ImportStatement(Base::Position position, Lexer::Token path, std::vector<Lexer::Token> imports);

      ImportStatement* clone() const;

      Lexer::Token getPath() const;
      std::vector<Lexer::Token> getImports() const;
  };

  class ExportStatement: public Statement {
    private:
      Statement* exports;

    public:
      ExportStatement(Base::Position position, Statement* exports);
      ~ExportStatement();

      ExportStatement* clone() const;

      Statement* getExports() const;
  };

  class ClassMemberDeclarationStatement: public Statement {
    protected:
      Specification::TokenType accessModifier;
      bool isStatic;
      bool isConstant;
      Lexer::Token name;

    public:
      ClassMemberDeclarationStatement(Base::Position, Specification::TokenType, bool, bool, Lexer::Token);

      ClassMemberDeclarationStatement* clone() const;

      Specification::TokenType getAccessModifier() const;
      bool getIsStatic() const;
      bool getIsConstant() const;
      Lexer::Token getName() const;
  };

  class ClassFieldDeclarationStatement: public ClassMemberDeclarationStatement {
    private:
      Expression* initialization;

    public:
      ClassFieldDeclarationStatement(Base::Position, Specification::TokenType, bool, bool, Lexer::Token, Expression*);
      ~ClassFieldDeclarationStatement();

      ClassFieldDeclarationStatement* clone() const;

      Expression* getInitialization() const;
  };

  class ClassMethodDeclarationStatement: public ClassMemberDeclarationStatement {
    protected:
      std::vector<FunctionParameterExpression*> params;
      BlockStatement* body;

    public:
      ClassMethodDeclarationStatement(Base::Position, Specification::TokenType, bool, Lexer::Token, std::vector<FunctionParameterExpression*>, BlockStatement*);
      ~ClassMethodDeclarationStatement();

      ClassMethodDeclarationStatement* clone() const;

      std::vector<FunctionParameterExpression*> getParams() const;
      AST::BlockStatement* getBody() const;
  };

  class ClassDeclarationStatement: public Statement {
    private:
      Lexer::Token name;
      std::vector<Expression*> extensionExpressions;
      std::vector<ClassMemberDeclarationStatement*> declarations;

    public:
      ClassDeclarationStatement(Base::Position position, Lexer::Token name, std::vector<Expression*> extensionExpressions, std::vector<ClassMemberDeclarationStatement*> declarations);
      ~ClassDeclarationStatement();

      ClassDeclarationStatement* clone() const;

      Lexer::Token getName() const;
      std::vector<Expression*> getExtensionExpressions() const;
      std::vector<ClassMemberDeclarationStatement*> getDeclarations() const;
  };
}
