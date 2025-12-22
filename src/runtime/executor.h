#pragma once 

#include "runtime/memory.h"
#include "runtime/stack.h"
#include "runtime/types.h"
#include "resolution/module.h"

namespace Runtime {
  class Executor {
    public:
      Executor();
      ~Executor();

      void loadModules(std::vector<Resolution::Module*>);
      void registerBuiltins();
      void execute();

    private:
      std::vector<Resolution::Module*> modules;
      Memory memory;

      // general statement execution
      void executeStatement(AST::Statement*);

      void executeBlockStatement(AST::BlockStatement* statement);
      void executeVariableDeclarationStatement(AST::VariableDeclarationStatement* statement);
      void executeConstantDeclarationStatement(AST::ConstantDeclarationStatement* statement);
      void executeConditionStatement(AST::ConditionStatement* statement);
      void executeWhileStatement(AST::WhileStatement* statement);
      void executeForStatement(AST::ForStatement* statement);
      void executeBreakStatement();
      void executeContinueStatement();
      void executeFunctionDeclarationStatement(AST::FunctionDeclarationStatement* statement);
      void executeReturnStatement(AST::ReturnStatement* statement);
      void executeImportStatement(AST::ImportStatement* statement);
      void executeExportStatement(AST::ExportStatement* statement);
      void executeExpressionStatement(AST::ExpressionStatement* statement);

      // general expression evaluation
      Container* evaluateExpression(AST::Expression*);

      Container* evaluateNullExpression();
      Container* evaluateLiteralExpression(AST::LiteralExpression*);
      Container* evaluateIdentifierExpression(AST::IdentifierExpression*);
      Container* evaluateUnaryExpression(AST::UnaryOperationExpression*);
      Container* evaluateBinaryExpression(AST::BinaryOperationExpression*);
      Container* evaluateGroupingExpression(AST::GroupingExpression*);
      Container* evaluateGroupingApplicationExpression(AST::GroupingApplicationExpression*);
      Container* evaluateAssociationExpression(AST::AssociationExpression*);

      // unary expressions
      Container* evaluateNotExpression(AST::UnaryOperationExpression*);
      Container* evaluateBitNotExpression(AST::UnaryOperationExpression*);
      Container* evaluateIncrementExpression(AST::UnaryOperationExpression*);
      Container* evaluateDecrementExpression(AST::UnaryOperationExpression*);

      // binary expressions
      Container* evaluateAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateMemberAccessExpression(AST::BinaryOperationExpression*);
      Container* evaluateAdditionExpression(AST::BinaryOperationExpression*);
      Container* evaluateSubtractionExpression(AST::BinaryOperationExpression*);
      Container* evaluateMultiplicationExpression(AST::BinaryOperationExpression*);
      Container* evaluateDivisionExpression(AST::BinaryOperationExpression*);
      Container* evaluateExponentialExpression(AST::BinaryOperationExpression*);
      Container* evaluateRemainderExpression(AST::BinaryOperationExpression*);
      Container* evaluateBitAndExpression(AST::BinaryOperationExpression*);
      Container* evaluateBitOrExpression(AST::BinaryOperationExpression*);
      Container* evaluateBitXorExpression(AST::BinaryOperationExpression*);
      Container* evaluateLeftShiftExpression(AST::BinaryOperationExpression*);
      Container* evaluateRightShiftExpression(AST::BinaryOperationExpression*);
      Container* evaluateAdditionAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateSubtractionAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateMultiplicationAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateDivisionAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateExponentialAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateRemainderAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateBitAndAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateBitOrAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateBitXorAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateLeftShiftAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateRightShiftAssignExpression(AST::BinaryOperationExpression*);
      Container* evaluateAndExpression(AST::BinaryOperationExpression*);
      Container* evaluateOrExpression(AST::BinaryOperationExpression*);
      Container* evaluateEqualExpression(AST::BinaryOperationExpression*);
      Container* evaluateNotEqualExpression(AST::BinaryOperationExpression*);
      Container* evaluateGreaterThanExpression(AST::BinaryOperationExpression*);
      Container* evaluateLessThanExpression(AST::BinaryOperationExpression*);
      Container* evaluateGreaterThanOrEqualExpression(AST::BinaryOperationExpression*);
      Container* evaluateLessThanOrEqualExpression(AST::BinaryOperationExpression*);

      // utils
      Container* createConstantContainer(Value*);
  };
}
