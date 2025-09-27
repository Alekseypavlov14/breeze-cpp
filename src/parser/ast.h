#pragma once

#include <vector>
#include <map>
#include <string>

#include "lexer/lexer.h"
#include "base/specification.h"

// this module declares hierarchy of classes for AST tree
// because the structure is tree, parent nodes are responsible for children memory
// destructors free memory for their children
namespace AST {
  class Node {
    public:
      // makes class polymorphic
      virtual ~Node() = default;
      // every node implements clone method
      virtual Node* clone() const = 0;
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
      NullExpression* clone() const;
  };

  class OperationExpression: public Expression {
    public: 
      virtual OperationExpression* clone() const = 0;
  };
  
  class UnaryOperationExpression: public OperationExpression {
    private:
      Lexer::Token operatorToken;
      Expression* operand;

    public:
      UnaryOperationExpression(Lexer::Token operatorToken, Expression* operand);
      ~UnaryOperationExpression();

      virtual UnaryOperationExpression* clone() const = 0;

      Lexer::Token getOperator() const;
      Expression* getOperand() const;
  };

  class PrefixUnaryOperationExpression: public UnaryOperationExpression {
    public:
      PrefixUnaryOperationExpression(Lexer::Token operatorToken, Expression* operand);
      PrefixUnaryOperationExpression* clone() const;
  };

  class SuffixUnaryOperationExpression: public UnaryOperationExpression {
    public:
      SuffixUnaryOperationExpression(Lexer::Token operatorToken, Expression* operand);
      SuffixUnaryOperationExpression* clone() const;
  };

  class AffixUnaryOperationExpression: public UnaryOperationExpression {
    public:
      AffixUnaryOperationExpression(Lexer::Token operatorToken, Expression* operand);
      AffixUnaryOperationExpression* clone() const;
  };

  class BinaryOperationExpression: public OperationExpression {
    private:
      Lexer::Token operatorToken;
      Expression* left;
      Expression* right;

    public:
      BinaryOperationExpression(Lexer::Token operatorToken, Expression* left, Expression* right);
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
      LiteralExpression(Lexer::Token value);

      LiteralExpression* clone() const;

      Lexer::Token getValue() const;
  };

  class IdentifierExpression: public Expression {
    private:
      Lexer::Token name;

    public:
      IdentifierExpression(Lexer::Token name);

      IdentifierExpression* clone() const;

      Lexer::Token getName() const;
  };

  // for (), []
  class GroupingExpression: public Expression {
    private:
      Lexer::Token operatorToken;
      std::vector<Expression*> expressions;

    public:
      GroupingExpression(Lexer::Token operatorToken, std::vector<Expression*> expressions);
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
      GroupingApplicationExpression(Expression* left, GroupingExpression* right);
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
      AssociationExpression(std::vector<std::pair<Expression*, Expression*>> entries);
      ~AssociationExpression();

      AssociationExpression* clone() const;

      std::vector<std::pair<Expression*, Expression*>> getEntries() const;
  };

  class FunctionParameterExpression: public Expression {
    private:
      Lexer::Token name;
      Expression* defaultValue;

    public:
      FunctionParameterExpression(Lexer::Token name, Expression* defaultValue);
      ~FunctionParameterExpression();

      FunctionParameterExpression* clone() const;

      Lexer::Token getName() const;
      Expression* getDefaultValue() const;
  };

  // statement variants
  class NullStatement: public Statement {
    public:
      NullStatement* clone() const;
  };

  class ExpressionStatement: public Statement {
    private:
      Expression* expression; 

    public:
      ExpressionStatement(Expression* expression);
      ~ExpressionStatement();

      ExpressionStatement* clone() const;

      Expression* getExpression() const;
  };

  class BlockStatement: public Statement {
    private:
      std::vector<Statement*> statements;

    public:
      BlockStatement(std::vector<Statement*> statements);
      ~BlockStatement();

      BlockStatement* clone() const;

      std::vector<Statement*> getStatements() const;
  };

  class VariableDeclarationStatement: public Statement {
    private:
      Lexer::Token name;
      Expression* initializer;

    public: 
      VariableDeclarationStatement(Lexer::Token name, Expression* initializer);
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
      ConstantDeclarationStatement(Lexer::Token name, Expression* initializer);
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
      ConditionStatement(Expression* condition, Statement* thenBranch, Statement* elseBranch);
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
      WhileStatement(Expression* condition, Statement* body);
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
      ForStatement(Statement* initializer, Expression* condition, Expression* increment, Statement* body);
      ~ForStatement();

      ForStatement* clone() const;

      Statement* getInitializer() const;
      Expression* getCondition() const;
      Expression* getIncrement() const;
      Statement* getBody() const;
  };

  class BreakStatement: public Statement {
    public:
      BreakStatement* clone() const;
  };
  class ContinueStatement: public Statement {
    public:
      ContinueStatement* clone() const;
  };

  class FunctionDeclarationStatement: public Statement {
    private:
      Lexer::Token name;
      std::vector<FunctionParameterExpression*> params;
      BlockStatement* body;

    public:
      FunctionDeclarationStatement(Lexer::Token name, std::vector<FunctionParameterExpression*> params, BlockStatement* body);
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
      ReturnStatement(Expression* returns);
      ~ReturnStatement();

      ReturnStatement* clone() const;

      Expression* getReturns() const;
  };

  // TODO: implement class declaration statement
  class ClassDeclarationStatement: public Statement {
    private:
      Lexer::Token name;

    public:
      ClassDeclarationStatement(Lexer::Token name);

      ClassDeclarationStatement* clone() const;

      Lexer::Token getName() const;
  };

  class ImportStatement: public Statement {
    private:
      Lexer::Token path;
      std::vector<Lexer::Token> imports;

    public:
      ImportStatement(Lexer::Token path, std::vector<Lexer::Token> imports);

      ImportStatement* clone() const;

      Lexer::Token getPath() const;
      std::vector<Lexer::Token> getImports() const;
  };

  class ExportStatement: public Statement {
    private:
      Statement* exports;

    public:
      ExportStatement(Statement* exports);
      ~ExportStatement();

      ExportStatement* clone() const;

      Statement* getExports() const;
  };
}
