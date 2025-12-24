#pragma once 

#include "runtime/memory.h"
#include "runtime/stack.h"
#include "runtime/types.h"
#include "builtins/builtins.h"
#include "resolution/loader.h"
#include "resolution/module.h"

namespace Runtime {
  // size of stack at the beginning of execution
  // one scope is for builtins and another one is for root block statement
  inline const int TOP_LEVEL_STACK_SIZE = 2;

  class Executor {
    public:
      Executor(Resolution::ModulesLoader);
      void registerBuiltins(std::vector<Builtins::BuiltinModuleDeclarations>);
      void execute();

    private:
      Resolution::ModulesLoader loader;
      Memory memory;

      // shortcuts
      Resolution::Module* currentModule;

      // general statement execution
      void executeStatement(AST::Statement*);

      void executeBlockStatement(AST::BlockStatement* statement);
      Container* executeVariableDeclarationStatement(AST::VariableDeclarationStatement* statement);
      Container* executeConstantDeclarationStatement(AST::ConstantDeclarationStatement* statement);
      void executeConditionStatement(AST::ConditionStatement* statement);
      void executeWhileStatement(AST::WhileStatement* statement);
      void executeForStatement(AST::ForStatement* statement);
      void executeBreakStatement();
      void executeContinueStatement();
      Container* executeFunctionDeclarationStatement(AST::FunctionDeclarationStatement* statement);
      void executeReturnStatement(AST::ReturnStatement* statement);
      Container* executeClassDeclarationStatement(AST::ClassDeclarationStatement* statement);
      void executeImportStatement(AST::ImportStatement* statement);
      void executeExportStatement(AST::ExportStatement* statement);
      Container* executeExportingStatement(AST::Statement* statement);
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

      // grouping expressions
      Container* evaluateParenthesesExpression(AST::GroupingExpression*);
      Container* evaluateSquareBracketsExpression(AST::GroupingExpression*);

      // grouping application expressions
      Container* evaluateParenthesesApplicationExpression(AST::GroupingApplicationExpression*);
      Container* evaluateSquareBracketsApplicationExpression(AST::GroupingApplicationExpression*);

      // builtin declarations
      Container* Executor::executeBuiltinDeclaration(Builtins::BuiltinDeclaration* statement);
      Container* executeBuiltinConstantDeclaration(Builtins::ConstantBuiltinDeclaration* statement);
      Container* executeBuiltinFunctionDeclaration(Builtins::FunctionBuiltinDeclaration* statement);
      Container* executeBuiltinClassDeclaration(Builtins::ClassBuiltinDeclaration* statement);

      // utils
      Container* createConstantContainer(Value*);
      bool isExecutionOnTopLevel();
  };
}
