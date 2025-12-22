#include "executor.h"
#include "runtime/exceptions.h"
#include "shared/utils.h"

#include <iostream>

namespace Runtime {
  Executor::Executor() {
    this->modules = {};
    this->memory = Memory();
  }
  Executor::~Executor() {
    for (int i = 0; i < this->modules.size(); i++) {
      delete this->modules[i];
    }
  }

  // setup
  void Executor::loadModules(std::vector<Resolution::Module*> modules) {
    this->modules = modules;
    this->memory.prepareStructuresForModules(modules.size());
  }

  void Executor::execute() {
    for (int i = 0; i < this->modules.size(); i++) {
      this->memory.setCurrentStackByIndex(i);
      this->memory.setCurrentExportsRegistryByIndex(i);

      this->memory.addScopeToStack();
      this->executeStatement(this->modules[i]->getContent());
    }
  } 

  // statements
  void Executor::executeStatement(AST::Statement* statement) {
    if (Shared::isInstanceOf<AST::Statement, AST::NullStatement>(statement)) return;
    if (Shared::isInstanceOf<AST::Statement, AST::VariableDeclarationStatement>(statement)) {
      return this->executeVariableDeclarationStatement(Shared::cast<AST::Statement, AST::VariableDeclarationStatement>(statement));
    }
    if (Shared::isInstanceOf<AST::Statement, AST::ConstantDeclarationStatement>(statement)) {
      return this->executeConstantDeclarationStatement(Shared::cast<AST::Statement, AST::ConstantDeclarationStatement>(statement));
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
      return this->executeFunctionDeclarationStatement(Shared::cast<AST::Statement, AST::FunctionDeclarationStatement>(statement));
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
      return this->executeExpressionStatement(Shared::cast<AST::Statement, AST::ExpressionStatement>(statement));
    }
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
  }

  Container* Executor::evaluateNullExpression() {}
  Container* Executor::evaluateLiteralExpression(AST::LiteralExpression* expression) {}
  Container* Executor::evaluateIdentifierExpression(AST::IdentifierExpression* expression) {}
  Container* Executor::evaluateUnaryExpression(AST::UnaryOperationExpression* expression) {}
  Container* Executor::evaluateBinaryExpression(AST::BinaryOperationExpression* expression) {}
  Container* Executor::evaluateGroupingExpression(AST::GroupingExpression* expression) {}
  Container* Executor::evaluateGroupingApplicationExpression(AST::GroupingApplicationExpression* expression) {}
  Container* Executor::evaluateAssociationExpression(AST::AssociationExpression* expression) {}

  // binary expressions
  Container* Executor::evaluateAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    leftContainer->setValue(rightContainer->getValue());

    return leftContainer;
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"+\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"-\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"*\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"/\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"**\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"%\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"&\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"|\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"^\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"<<\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \">>\" is used with invalid type pair");
  }
  Container* Executor::evaluateAdditionAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"+=\" is used with invalid type pair");
  }
  Container* Executor::evaluateSubtractionAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left - right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getOperator().getPosition(), "Operator \"-=\" is used with invalid type pair");
  }
  Container* Executor::evaluateMultiplicationAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left * right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getOperator().getPosition(), "Operator \"*=\" is used with invalid type pair");
  }
  Container* Executor::evaluateDivisionAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left / right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getOperator().getPosition(), "Operator \"/=\" is used with invalid type pair");
  }
  Container* Executor::evaluateExponentialAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(std::pow(left, right));
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getOperator().getPosition(), "Operator \"**=\" is used with invalid type pair");
  }
  Container* Executor::evaluateRemainderAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left % right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getOperator().getPosition(), "Operator \"%=\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitAndAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left & right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getOperator().getPosition(), "Operator \"&=\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitOrAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left | right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getOperator().getPosition(), "Operator \"|=\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitXorAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left ^ right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getOperator().getPosition(), "Operator \"^=\" is used with invalid type pair");
  } 
  Container* Executor::evaluateLeftShiftAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left << right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getOperator().getPosition(), "Operator \"<<=\" is used with invalid type pair");
  }
  Container* Executor::evaluateRightShiftAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getOperator().getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left >> right);
      this->memory.addTemporaryValue(result);

      leftContainer->setValue(result);
    }

    throw TypeException(expression->getOperator().getPosition(), "Operator \">>=\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \">\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"<\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \">=\" is used with invalid type pair");
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

    throw TypeException(expression->getOperator().getPosition(), "Operator \"<=\" is used with invalid type pair");
  }
  
  // utils
  Container* Executor::createConstantContainer(Value* value) {
    return new Container("", value, true);
  }
}
