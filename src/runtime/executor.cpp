#include "executor.h"
#include "runtime/exceptions.h"
#include "shared/utils.h"

#include <iostream>

namespace Runtime {
  Executor::Executor(Resolution::ModulesLoader loader) {
    this->loader = loader;
    this->memory = Memory();
  }
  void Executor::registerBuiltins(std::vector<Builtins::BuiltinModuleDeclarations> moduleDeclarations) {
    std::vector<Container*> containers = {}; 
    
    for (int i = 0; i < moduleDeclarations.size(); i++) {
      for (int j = 0; j < moduleDeclarations[i].size(); j++) {
        containers.push_back(this->executeBuiltinDeclaration(moduleDeclarations[i][j]));
      }
    }

    this->memory.loadBuiltinContainers(containers);
  }
  void Executor::execute() {
    for (int i = 0; i < this->loader.getModules().size(); i++) {
      this->memory.setCurrentStackByIndex(i);
      this->memory.setCurrentExportsRegistryByIndex(i);      

      this->currentModule = this->loader.getModules()[i];
      this->executeStatement(this->currentModule->getContent());
    }
  } 

  // statements
  void Executor::executeStatement(AST::Statement* statement) {
    if (Shared::isInstanceOf<AST::Statement, AST::NullStatement>(statement)) return;
    if (Shared::isInstanceOf<AST::Statement, AST::BlockStatement>(statement)) {
      return this->executeBlockStatement(Shared::cast<AST::Statement, AST::BlockStatement>(statement));
    }
    if (Shared::isInstanceOf<AST::Statement, AST::VariableDeclarationStatement>(statement)) {
      this->executeVariableDeclarationStatement(Shared::cast<AST::Statement, AST::VariableDeclarationStatement>(statement));
      return;
    }
    if (Shared::isInstanceOf<AST::Statement, AST::ConstantDeclarationStatement>(statement)) {
      this->executeConstantDeclarationStatement(Shared::cast<AST::Statement, AST::ConstantDeclarationStatement>(statement));
      return;
    }
    if (Shared::isInstanceOf<AST::Statement, AST::ConditionStatement>(statement)) {
      return this->executeConditionStatement(Shared::cast<AST::Statement, AST::ConditionStatement>(statement));
    }
    if (Shared::isInstanceOf<AST::Statement, AST::WhileStatement>(statement)) {
      return this->executeWhileStatement(Shared::cast<AST::Statement, AST::WhileStatement>(statement));
    }
    if (Shared::isInstanceOf<AST::Statement, AST::BreakStatement>(statement)) {
      return this->executeBreakStatement();
    }
    if (Shared::isInstanceOf<AST::Statement, AST::ContinueStatement>(statement)) {
      return this->executeContinueStatement();
    }
    if (Shared::isInstanceOf<AST::Statement, AST::FunctionDeclarationStatement>(statement)) {
      this->executeFunctionDeclarationStatement(Shared::cast<AST::Statement, AST::FunctionDeclarationStatement>(statement));
      return;
    }
    if (Shared::isInstanceOf<AST::Statement, AST::ReturnStatement>(statement)) {
      return this->executeReturnStatement(Shared::cast<AST::Statement, AST::ReturnStatement>(statement));
    }
    if (Shared::isInstanceOf<AST::Statement, AST::ImportStatement>(statement)) {
      return this->executeImportStatement(Shared::cast<AST::Statement, AST::ImportStatement>(statement));
    }
    if (Shared::isInstanceOf<AST::Statement, AST::ExportStatement>(statement)) {
      return this->executeExportStatement(Shared::cast<AST::Statement, AST::ExportStatement>(statement));
    }
    if (Shared::isInstanceOf<AST::Statement, AST::ExpressionStatement>(statement)) {
      this->executeExpressionStatement(Shared::cast<AST::Statement, AST::ExpressionStatement>(statement));
      return;
    }

    throw StatementException(statement->getPosition(), "Invalid statement");
  }

  void Executor::executeBlockStatement(AST::BlockStatement* statement) {
    // TODO: add containers management
    this->memory.addScopeToStack();

    for (int i = 0; i < statement->getStatements().size(); i++) {
      this->executeStatement(statement->getStatements()[i]);
    }

    this->memory.removeScopeFromStack();
  }
  Container* Executor::executeVariableDeclarationStatement(AST::VariableDeclarationStatement* statement) {
    Container* initialization = this->evaluateExpression(statement->getInitializer());
    Container* variableContainer = new Container(statement->getName().getCode(), initialization->getValue());
    
    this->memory.retainContainer(variableContainer);
    this->memory.retainValue(variableContainer->getValue());

    this->memory.addContainerToStack(variableContainer);

    return variableContainer;
  }
  Container* Executor::executeConstantDeclarationStatement(AST::ConstantDeclarationStatement* statement) {
    Container* initialization = this->evaluateExpression(statement->getInitializer());
    Container* constantContainer = new Container(statement->getName().getCode(), initialization->getValue(), true);

    this->memory.retainContainer(constantContainer);
    this->memory.retainValue(constantContainer->getValue());

    this->memory.addContainerToStack(constantContainer);

    return constantContainer;
  }
  void Executor::executeConditionStatement(AST::ConditionStatement *statement) {
    Container* condition = this->evaluateExpression(statement->getCondition());
    Value* conditionValue = condition->getValue();

    if (getBoolean(conditionValue)) {
      this->executeStatement(statement->getThenBranch());
    } else {
      this->executeStatement(statement->getElseBranch());
    }
  }
  void Executor::executeWhileStatement(AST::WhileStatement *statement) {
    while(true) {
      Container* condition = this->evaluateExpression(statement->getCondition());
      Value* conditionValue = condition->getValue();

      if (!getBoolean(conditionValue)) break;

      this->executeStatement(statement->getBody());
    } 
  }
  void Executor::executeForStatement(AST::ForStatement *statement) {
    this->memory.addScopeToStack();

    this->executeStatement(statement->getInitializer());

    while(true) {
      Container* condition = this->evaluateExpression(statement->getCondition());
      Value* conditionValue = condition->getValue();

      if (!getBoolean(conditionValue)) break;

      this->executeStatement(statement->getBody());
      this->evaluateExpression(statement->getIncrement());
    } 

    this->memory.removeScopeFromStack();
  }
  void Executor::executeBreakStatement() {
    throw Exception("Not implemented");
  }
  void Executor::executeContinueStatement() {
    throw Exception("Not implemented");
  }
  Container* Executor::executeFunctionDeclarationStatement(AST::FunctionDeclarationStatement *statement) {
    throw Exception("Not implemented");
  }
  void Executor::executeReturnStatement(AST::ReturnStatement *statement) {
    throw Exception("Not implemented");
  }
  Container* Executor::executeClassDeclarationStatement(AST::ClassDeclarationStatement* statement) {
    throw Exception("Not implemented");
  }
  void Executor::executeImportStatement(AST::ImportStatement *statement) {
    if (!this->isExecutionOnTopLevel()) {
      throw StatementException(statement->getPosition(), "Import statement can be used only on top level");
    }

    // find dependency module
    std::string dependencyAbsolutePath = this->loader.resolveAbsolutePath(this->currentModule->getAbsolutePath(), statement->getPath().getCode());
    int moduleIndex = this->loader.getLoadedModuleIndexByAbsolutePath(dependencyAbsolutePath);

    // put dependency as current exporting module
    this->memory.setCurrentExportsRegistryByIndex(moduleIndex);

    // import each symbol
    for (int i = 0; i < statement->getImports().size(); i++) {
      // TODO: implement asterisk import

      Lexer::Token currentImportSymbol = statement->getImports()[i];
      Container* currentImportContainer = this->memory.getContainerFromExports(currentImportSymbol.getCode());

      Container* constantSymbol = new Container(currentImportSymbol.getCode(), currentImportContainer->getValue(), true);
     
      this->memory.retainContainer(constantSymbol);
      this->memory.retainValue(constantSymbol->getValue());

      this->memory.addContainerToStack(constantSymbol);
    }
  }
  void Executor::executeExportStatement(AST::ExportStatement *statement) {
    throw Exception("Not implemented");
  }
  Container* Executor::executeExportingStatement(AST::Statement* statement) {
    throw Exception("Not implemented");
  }
  void Executor::executeExpressionStatement(AST::ExpressionStatement *statement) {
    this->evaluateExpression(statement->getExpression());
    
    this->memory.clearTemporaryContainers();
    this->memory.clearTemporaryValues();
  }

  // expressions
  Container* Executor::evaluateExpression(AST::Expression* expression) {
    if (Shared::isInstanceOf<AST::Expression, AST::NullExpression>(expression)) {
      return this->evaluateNullExpression();
    }
    if (Shared::isInstanceOf<AST::Expression, AST::LiteralExpression>(expression)) {
      return this->evaluateLiteralExpression(Shared::cast<AST::Expression, AST::LiteralExpression>(expression));
    }
    if (Shared::isInstanceOf<AST::Expression, AST::IdentifierExpression>(expression)) {
      return this->evaluateIdentifierExpression(Shared::cast<AST::Expression, AST::IdentifierExpression>(expression));
    }
    if (Shared::isInstanceOf<AST::Expression, AST::UnaryOperationExpression>(expression)) {
      return this->evaluateUnaryExpression(Shared::cast<AST::Expression, AST::UnaryOperationExpression>(expression));
    }
    if (Shared::isInstanceOf<AST::Expression, AST::BinaryOperationExpression>(expression)) {
      return this->evaluateBinaryExpression(Shared::cast<AST::Expression, AST::BinaryOperationExpression>(expression));
    }
    if (Shared::isInstanceOf<AST::Expression, AST::GroupingExpression>(expression)) {
      return this->evaluateGroupingExpression(Shared::cast<AST::Expression, AST::GroupingExpression>(expression));
    }
    if (Shared::isInstanceOf<AST::Expression, AST::GroupingApplicationExpression>(expression)) {
      return this->evaluateGroupingApplicationExpression(Shared::cast<AST::Expression, AST::GroupingApplicationExpression>(expression));
    }
    if (Shared::isInstanceOf<AST::Expression, AST::AssociationExpression>(expression)) {
      return this->evaluateAssociationExpression(Shared::cast<AST::Expression, AST::AssociationExpression>(expression));
    }

    throw ExpressionException(expression->getPosition(), "Invalid expression");
  }

  Container* Executor::evaluateNullExpression() {
    NullValue* nullValue = new NullValue();
    this->memory.addTemporaryValue(nullValue);

    Container* container = this->createConstantContainer(nullValue);
    this->memory.addTemporaryContainer(container);

    return container;
  }
  Container* Executor::evaluateLiteralExpression(AST::LiteralExpression* expression) {
    Value* value = nullptr;

    if (expression->getValue().isOfType(Specification::TokenType::NULL_KEYWORD_TOKEN)) {
      value = new NullValue();
    }
    if (expression->getValue().isOfType(Specification::TokenType::TRUE_KEYWORD_TOKEN)) {
      value = new BooleanValue(true);
    }
    if (expression->getValue().isOfType(Specification::TokenType::FALSE_KEYWORD_TOKEN)) {
      value = new BooleanValue(false);
    }
    if (expression->getValue().isOfType(Specification::TokenType::NUMBER_TOKEN)) {
      value = new NumberValue(std::stod(expression->getValue().getCode()));
    }
    if (expression->getValue().isOfType(Specification::TokenType::STRING_TOKEN)) {
      value = new StringValue(expression->getValue().getCode());
    }

    if (value == nullptr) {
      throw TypeException(expression->getPosition(), "Invalid literal expression");
    }

    this->memory.addTemporaryValue(value);

    Container* container = this->createConstantContainer(value);
    this->memory.addTemporaryContainer(container);

    return container;
  }
  Container* Executor::evaluateIdentifierExpression(AST::IdentifierExpression* expression) {
    return this->memory.getContainerFromStack(expression->getName().getCode());
  }
  Container* Executor::evaluateUnaryExpression(AST::UnaryOperationExpression* expression) {
    if (expression->getOperator().isOfType(Specification::TokenType::NOT_TOKEN)) {
      return this->evaluateNotExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::BIT_NOT_TOKEN)) {
      return this->evaluateBitNotExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::INCREMENT_TOKEN)) {
      return this->evaluateIncrementExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::DECREMENT_TOKEN)) {
      return this->evaluateDecrementExpression(expression);
    }

    throw ExpressionException(expression->getPosition(), "Invalid unary expression");
  }
  Container* Executor::evaluateBinaryExpression(AST::BinaryOperationExpression* expression) {
    if (expression->getOperator().isOfType(Specification::TokenType::ASSIGN_TOKEN)) {
      return this->evaluateAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::DOT_TOKEN)) {
      return this->evaluateMemberAccessExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::PLUS_TOKEN)) {
      return this->evaluateAdditionExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::MINUS_TOKEN)) {
      return this->evaluateSubtractionExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::MULTIPLICATION_TOKEN)) {
      return this->evaluateMultiplicationExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::DIVISION_TOKEN)) {
      return this->evaluateDivisionExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::EXPONENTIAL_TOKEN)) {
      return this->evaluateExponentialExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::REMAINDER_TOKEN)) {
      return this->evaluateRemainderExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::BIT_AND_TOKEN)) {
      return this->evaluateBitAndExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::BIT_OR_TOKEN)) {
      return this->evaluateBitOrExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::BIT_XOR_TOKEN)) {
      return this->evaluateBitXorExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::LEFT_SHIFT_TOKEN)) {
      return this->evaluateLeftShiftExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::RIGHT_SHIFT_TOKEN)) {
      return this->evaluateRightShiftExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::PLUS_ASSIGN_TOKEN)) {
      return this->evaluateAdditionAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::MINUS_ASSIGN_TOKEN)) {
      return this->evaluateSubtractionAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::MULTIPLICATION_ASSIGN_TOKEN)) {
      return this->evaluateMultiplicationAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::DIVISION_ASSIGN_TOKEN)) {
      return this->evaluateDivisionAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::EXPONENTIAL_ASSIGN_TOKEN)) {
      return this->evaluateExponentialAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::REMAINDER_ASSIGN_TOKEN)) {
      return this->evaluateRemainderAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::BIT_AND_ASSIGN_TOKEN)) {
      return this->evaluateBitAndAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::BIT_OR_ASSIGN_TOKEN)) {
      return this->evaluateBitOrAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::BIT_XOR_ASSIGN_TOKEN)) {
      return this->evaluateBitXorAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::LEFT_SHIFT_ASSIGN_TOKEN)) {
      return this->evaluateLeftShiftAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::RIGHT_SHIFT_ASSIGN_TOKEN)) {
      return this->evaluateRightShiftAssignExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::AND_TOKEN)) {
      return this->evaluateAndExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::OR_TOKEN)) {
      return this->evaluateOrExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::EQUAL_TOKEN)) {
      return this->evaluateEqualExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::NOT_EQUAL_TOKEN)) {
      return this->evaluateNotEqualExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::GREATER_THAN_TOKEN)) {
      return this->evaluateGreaterThanExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::LESS_THAN_TOKEN)) {
      return this->evaluateLessThanExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::GREATER_THAN_OR_EQUAL_TOKEN)) {
      return this->evaluateGreaterThanOrEqualExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::LESS_THAN_OR_EQUAL_TOKEN)) {
      return this->evaluateLessThanOrEqualExpression(expression);
    }

    throw ExpressionException(expression->getPosition(), "Invalid binary expression");
  }
  Container* Executor::evaluateGroupingExpression(AST::GroupingExpression* expression) {
    if (expression->getOperator().isOfType(Specification::TokenType::LEFT_PARENTHESES_TOKEN)) {
      return this->evaluateParenthesesExpression(expression);
    }
    if (expression->getOperator().isOfType(Specification::TokenType::LEFT_SQUARE_BRACKET_TOKEN)) {
      return this->evaluateSquareBracketsExpression(expression);
    }

    throw ExpressionException(expression->getPosition(), "Invalid grouping expression");
  }
  Container* Executor::evaluateGroupingApplicationExpression(AST::GroupingApplicationExpression* expression) {
    if (expression->getRight()->getOperator().isOfType(Specification::TokenType::LEFT_PARENTHESES_TOKEN)) {
      return this->evaluateParenthesesApplicationExpression(expression);
    }
    if (expression->getRight()->getOperator().isOfType(Specification::TokenType::LEFT_SQUARE_BRACKET_TOKEN)) {
      return this->evaluateSquareBracketsApplicationExpression(expression);
    }

    throw ExpressionException(expression->getRight()->getOperator().getPosition(), "Invalid grouping application expression");
  }
  Container* Executor::evaluateAssociationExpression(AST::AssociationExpression* expression) {
    throw Exception("Not implemented");
  }

  // unary expression
  Container* Executor::evaluateNotExpression(AST::UnaryOperationExpression* expression) {
    throw Exception("Not implemented");
  }
  Container* Executor::evaluateBitNotExpression(AST::UnaryOperationExpression*) {
    throw Exception("Not implemented");
  }
  Container* Executor::evaluateIncrementExpression(AST::UnaryOperationExpression*) {
    throw Exception("Not implemented");
  }
  Container* Executor::evaluateDecrementExpression(AST::UnaryOperationExpression*) {
    throw Exception("Not implemented");
  }

  // binary expressions
  Container* Executor::evaluateAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    this->memory.releaseValue(leftContainer->getValue());
    this->memory.retainValue(rightContainer->getValue());

    leftContainer->setValue(rightContainer->getValue());
    
    return leftContainer;
  }
  Container* Executor::evaluateMemberAccessExpression(AST::BinaryOperationExpression* expression) {
    throw Exception("Not implemented");
  }
  Container* Executor::evaluateAdditionExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    // number + number
    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double leftValue = NumberValue::getDataOf(leftContainer->getValue());
      double rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue + rightValue);
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    // string + string
    if (Shared::isInstanceOf<Value, StringValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, StringValue>(rightContainer->getValue())) {
      std::string leftValue = StringValue::getDataOf(leftContainer->getValue());
      std::string rightValue = StringValue::getDataOf(rightContainer->getValue());

      StringValue* result = new StringValue(leftValue + rightValue);
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"+\" is used with invalid type pair");
  }
  Container* Executor::evaluateSubtractionExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    // number - number
    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double leftValue = NumberValue::getDataOf(leftContainer->getValue());
      double rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue - rightValue);
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"-\" is used with invalid type pair");
  }
  Container* Executor::evaluateMultiplicationExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    // number * number
    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double leftValue = NumberValue::getDataOf(leftContainer->getValue());
      double rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue * rightValue);
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"*\" is used with invalid type pair");
  }
  Container* Executor::evaluateDivisionExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    // number / number
    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double leftValue = NumberValue::getDataOf(leftContainer->getValue());
      double rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue / rightValue);
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"/\" is used with invalid type pair");
  }
  Container* Executor::evaluateExponentialExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double leftValue = NumberValue::getDataOf(leftContainer->getValue());
      double rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(std::pow(leftValue, rightValue));
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"**\" is used with invalid type pair");
  }
  Container* Executor::evaluateRemainderExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = (long long)NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = (long long)NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue % rightValue);
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"%\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitAndExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue & rightValue);
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"&\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitOrExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue | rightValue);
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"|\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitXorExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue ^ rightValue);
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"^\" is used with invalid type pair");
  }
  Container* Executor::evaluateLeftShiftExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue << rightValue);
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"<<\" is used with invalid type pair");
  }
  Container* Executor::evaluateRightShiftExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue >> rightValue);
      this->memory.addTemporaryValue(result);

      Container* resultContainer = this->createConstantContainer(result);
      this->memory.addTemporaryContainer(resultContainer);

      return resultContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \">>\" is used with invalid type pair");
  }
  Container* Executor::evaluateAdditionAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left + right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }
    if (Shared::isInstanceOf<Value, StringValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, StringValue>(rightContainer->getValue())) {
      std::string left = StringValue::getDataOf(leftContainer->getValue());
      std::string right = StringValue::getDataOf(rightContainer->getValue());

      Value* result = new StringValue(left + right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"+=\" is used with invalid type pair");
  }
  Container* Executor::evaluateSubtractionAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left - right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"-=\" is used with invalid type pair");
  }
  Container* Executor::evaluateMultiplicationAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left * right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"*=\" is used with invalid type pair");
  }
  Container* Executor::evaluateDivisionAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left / right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"/=\" is used with invalid type pair");
  }
  Container* Executor::evaluateExponentialAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(std::pow(left, right));
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"**=\" is used with invalid type pair");
  }
  Container* Executor::evaluateRemainderAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left % right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"%=\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitAndAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left & right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"&=\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitOrAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left | right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"|=\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitXorAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left ^ right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"^=\" is used with invalid type pair");
  } 
  Container* Executor::evaluateLeftShiftAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left << right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"<<=\" is used with invalid type pair");
  }
  Container* Executor::evaluateRightShiftAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left >> right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getPosition(), "Operator \">>=\" is used with invalid type pair");
  }
  Container* Executor::evaluateAndExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (!getBoolean(leftContainer->getValue())) return leftContainer;

    Container* rightContainer = this->evaluateExpression(expression->getRight());
    return rightContainer;
  }
  Container* Executor::evaluateOrExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (getBoolean(leftContainer->getValue())) return leftContainer;

    Container* rightContainer = this->evaluateExpression(expression->getRight());
    return rightContainer;
  }
  Container* Executor::evaluateEqualExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    BooleanValue* result = new BooleanValue(compareValues(leftContainer->getValue(), rightContainer->getValue()));
    this->memory.addTemporaryValue(result);
    
    return this->createConstantContainer(result);
  }
  Container* Executor::evaluateNotEqualExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    BooleanValue* result = new BooleanValue(!compareValues(leftContainer->getValue(), rightContainer->getValue()));
    this->memory.addTemporaryValue(result);
    
    return this->createConstantContainer(result);
  }
  Container* Executor::evaluateGreaterThanExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());
      
      BooleanValue* result = new BooleanValue(left > right);
      this->memory.addTemporaryValue(result);
      
      return this->createConstantContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \">\" is used with invalid type pair");
  }
  Container* Executor::evaluateLessThanExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());
      
      BooleanValue* result = new BooleanValue(left < right);
      this->memory.addTemporaryValue(result);
      
      return this->createConstantContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"<\" is used with invalid type pair");
  }
  Container* Executor::evaluateGreaterThanOrEqualExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());
      
      BooleanValue* result = new BooleanValue(left >= right);
      this->memory.addTemporaryValue(result);
      
      return this->createConstantContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \">=\" is used with invalid type pair");
  }
  Container* Executor::evaluateLessThanOrEqualExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());
      
      BooleanValue* result = new BooleanValue(left <= right);
      this->memory.addTemporaryValue(result);
      
      return this->createConstantContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"<=\" is used with invalid type pair");
  }

  // grouping expressions
  Container* Executor::evaluateParenthesesExpression(AST::GroupingExpression* expression) {
    if (!expression->getExpressions().size()) {
      NullValue* value = new NullValue();
      this->memory.addTemporaryValue(value);

      return this->createConstantContainer(value);
    }

    Container* result = nullptr;
    this->memory.addTemporaryContainer(result);

    for (int i = 0; i < expression->getExpressions().size(); i++) {
      result = this->evaluateExpression(expression->getExpressions()[i]);
    }

    return result;
  }
  Container* Executor::evaluateSquareBracketsExpression(AST::GroupingExpression* expression) {
    VectorValue* list = new VectorValue({});
    this->memory.addTemporaryValue(list);

    for (int i = 0; i < expression->getExpressions().size(); i++) {
      list->push(this->evaluateExpression(expression->getExpressions()[i])->getValue());
    }

    Container* result = this->createConstantContainer(list);
    this->memory.addTemporaryContainer(result);

    return result;
  }

  // grouping application expressions
  Container* Executor::evaluateParenthesesApplicationExpression(AST::GroupingApplicationExpression*) {
    throw Exception("Not implemented");
  }
  Container* Executor::evaluateSquareBracketsApplicationExpression(AST::GroupingApplicationExpression*) {
    throw Exception("Not implemented");
  }
 
  // builtins
  Container* Executor::executeBuiltinDeclaration(const Builtins::BuiltinDeclaration* statement) {
    if (Shared::isInstanceOf<const Builtins::BuiltinDeclaration, Builtins::ConstantBuiltinDeclaration>(statement)) {
      return this->executeBuiltinConstantDeclaration(Shared::cast<const Builtins::BuiltinDeclaration, Builtins::ConstantBuiltinDeclaration>(statement));
    }
    if (Shared::isInstanceOf<const Builtins::BuiltinDeclaration, Builtins::FunctionBuiltinDeclaration>(statement)) {
      return this->executeBuiltinFunctionDeclaration(Shared::cast<const Builtins::BuiltinDeclaration, Builtins::FunctionBuiltinDeclaration>(statement));
    }
    if (Shared::isInstanceOf<const Builtins::BuiltinDeclaration, Builtins::ClassBuiltinDeclaration>(statement)) {
      return this->executeBuiltinClassDeclaration(Shared::cast<const Builtins::BuiltinDeclaration, Builtins::ClassBuiltinDeclaration>(statement));
    }

    throw Exception("Invalid builtin statement found");
  }
  Container* Executor::executeBuiltinConstantDeclaration(Builtins::ConstantBuiltinDeclaration* statement) {
    throw Exception("Not implemented");
  }
  Container* Executor::executeBuiltinFunctionDeclaration(Builtins::FunctionBuiltinDeclaration* statement) {
    throw Exception("Not implemented");
  }
  Container* Executor::executeBuiltinClassDeclaration(Builtins::ClassBuiltinDeclaration* statement) {
    throw Exception("Not implemented");
  }

  // utils
  Container* Executor::createConstantContainer(Value* value) {
    return new Container("", value, true);
  }
  bool Executor::isExecutionOnTopLevel() {
    return this->memory.getCurrentStackIndex() <= TOP_LEVEL_STACK_SIZE;
  }
}
