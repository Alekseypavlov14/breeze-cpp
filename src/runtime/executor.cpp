#include "executor.h"
#include "runtime/exceptions.h"
#include "runtime/signals.h"
#include "shared/classes.h"
#include "shared/vectors.h"

#include <iostream>

namespace Runtime {
  Executor::Executor() {
    this->loader = Resolution::ModulesLoader();
    this->memory = Memory();
  }

  void Executor::loadModulesFromEntrypoint(std::string absolutePath) {
    this->loader.loadModulesFromEntrypointPath(absolutePath);
    this->memory.prepareStructuresForModules(this->loader.getModules().size());
  }
  void Executor::registerBuiltins(std::vector<Builtins::BuiltinModuleDeclarations> moduleDeclarations) {
    // execute declarations
    for (int m = 0; m < this->loader.getModules().size(); m++) {
      // create builtin scopes in every stack
      this->memory.setCurrentStackByIndex(m);
      this->addScopeInCurrentStack();

      // load all declarations
      for (int i = 0; i < moduleDeclarations.size(); i++) {
        for (int j = 0; j < moduleDeclarations[i].size(); j++) {
          this->addContainerToCurrentStack(this->executeBuiltinDeclaration(moduleDeclarations[i][j]));
        }
      }
    }
  }
  void Executor::execute() {
    for (int i = 0; i < this->loader.getModules().size(); i++) {
      this->memory.setCurrentStackByIndex(i);
      this->memory.setCurrentExportsRegistryByIndex(i);   

      this->executeStatement(this->loader.getModules()[i]->getContent());
    }
  } 

  // statements
  void Executor::executeStatement(AST::Statement* statement) {
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::NullStatement>(statement)) return;
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::BlockStatement>(statement)) {
      return this->executeBlockStatement(Shared::Classes::cast<AST::Statement, AST::BlockStatement>(statement));
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::VariableDeclarationStatement>(statement)) {
      this->executeVariableDeclarationStatement(Shared::Classes::cast<AST::Statement, AST::VariableDeclarationStatement>(statement));
      return;
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::ConstantDeclarationStatement>(statement)) {
      this->executeConstantDeclarationStatement(Shared::Classes::cast<AST::Statement, AST::ConstantDeclarationStatement>(statement));
      return;
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::ConditionStatement>(statement)) {
      return this->executeConditionStatement(Shared::Classes::cast<AST::Statement, AST::ConditionStatement>(statement));
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::WhileStatement>(statement)) {
      return this->executeWhileStatement(Shared::Classes::cast<AST::Statement, AST::WhileStatement>(statement));
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::ForStatement>(statement)) {
      return this->executeForStatement(Shared::Classes::cast<AST::Statement, AST::ForStatement>(statement));
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::BreakStatement>(statement)) {
      return this->executeBreakStatement(Shared::Classes::cast<AST::Statement, AST::BreakStatement>(statement));
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::ContinueStatement>(statement)) {
      return this->executeContinueStatement(Shared::Classes::cast<AST::Statement, AST::ContinueStatement>(statement));
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::FunctionDeclarationStatement>(statement)) {
      this->executeFunctionDeclarationStatement(Shared::Classes::cast<AST::Statement, AST::FunctionDeclarationStatement>(statement));
      return;
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::ReturnStatement>(statement)) {
      return this->executeReturnStatement(Shared::Classes::cast<AST::Statement, AST::ReturnStatement>(statement));
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::ClassDeclarationStatement>(statement)) {
      this->executeClassDeclarationStatement(Shared::Classes::cast<AST::Statement, AST::ClassDeclarationStatement>(statement));
      return;
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::ImportStatement>(statement)) {
      return this->executeImportStatement(Shared::Classes::cast<AST::Statement, AST::ImportStatement>(statement));
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::ExportStatement>(statement)) {
      return this->executeExportStatement(Shared::Classes::cast<AST::Statement, AST::ExportStatement>(statement));
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::ExpressionStatement>(statement)) {
      this->executeExpressionStatement(Shared::Classes::cast<AST::Statement, AST::ExpressionStatement>(statement));
      return;
    }

    throw StatementException(statement->getPosition(), "Invalid statement");
  }
  void Executor::executeBlockStatement(AST::BlockStatement* statement) {
    this->addScopeInCurrentStack();

    for (int i = 0; i < statement->getStatements().size(); i++) {
      this->executeStatement(statement->getStatements()[i]);

      this->memory.clearTemporaryContainers();
      this->memory.clearTemporaryValues();
    }

    this->memory.removeUnreachableValues();
    this->removeScopeFromCurrentStack();
  }
  Container* Executor::executeVariableDeclarationStatement(AST::VariableDeclarationStatement* statement) {
    Container* initialization = this->evaluateExpression(statement->getInitializer());
    Container* variableContainer = new Container(statement->getName().getCode(), initialization->getValue());

    this->addContainerToCurrentStack(variableContainer);
    return variableContainer;
  }
  Container* Executor::executeConstantDeclarationStatement(AST::ConstantDeclarationStatement* statement) {
    Container* initialization = this->evaluateExpression(statement->getInitializer());
    Container* constantContainer = new Container(statement->getName().getCode(), initialization->getValue(), true);

    this->addContainerToCurrentStack(constantContainer);
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
    this->addScopeInCurrentStack();

    this->executeStatement(statement->getInitializer());

    while(true) {
      Container* condition = this->evaluateExpression(statement->getCondition());
      Value* conditionValue = condition->getValue();

      if (!getBoolean(conditionValue)) break;

      this->executeStatement(statement->getBody());
      this->evaluateExpression(statement->getIncrement());
    } 

    this->removeScopeFromCurrentStack();
  }
  void Executor::executeBreakStatement(AST::BreakStatement* statement) {
    throw BreakSignal(statement);
  }
  void Executor::executeContinueStatement(AST::ContinueStatement* statement) {
    throw ContinueSignal(statement);
  }
  Container* Executor::executeFunctionDeclarationStatement(AST::FunctionDeclarationStatement *statement) {
    // retain all containers
    std::vector<Container*> containersInCurrentStack = this->memory.getCurrentStack()->getContainers();
    for (int i = 0; i < containersInCurrentStack.size(); i++) {
      this->memory.retainContainer(containersInCurrentStack[i]);
    }
    
    // parse arguments amount
    int totalArgumentsAmount = 0;
    int optionalArgumentsAmount = 0;
    bool isOptionalParamReached = false;

    for (int i = 0; i < statement->getParams().size(); i++) {
      if (statement->getParams()[i]->getDefaultValue() != NULL) {
        isOptionalParamReached = true;
        optionalArgumentsAmount++;
      } else if (isOptionalParamReached) {
        throw ExpressionException(statement->getParams()[i]->getPosition(), "Required argument goes after optional");
      }
      
      totalArgumentsAmount++;
    }

    // compose function arguments
    FunctionArgumentsAmount argumentsAmount(totalArgumentsAmount, optionalArgumentsAmount);
    
    // create closure
    Stack* functionClosure = new Stack(this->copyCurrentStack());

    // construct callable
    Callable callable = [this, functionClosure, statement](std::vector<Value*> arguments) -> Value* {
      // TODO: set current context
      
      // assign context
      // TODO: assign "this" value

      // initialize argument values
      std::vector<Value*> argumentValues = {};
      for (int i = 0; i < statement->getParams().size(); i++) {
        // if an argument is passed: use it
        if (i < arguments.size()) {
          argumentValues.push_back(arguments[i]);
        }
        // otherwise check default ones 
        else {
          argumentValues.push_back(this->evaluateExpression(statement->getParams()[i]->getDefaultValue())->getValue());
        }  
      }
      
      // go to function stack
      this->memory.setCurrentStack(functionClosure);
      this->addScopeInCurrentStack();
      
      // add argument containers to function stack
      for (int i = 0; i < argumentValues.size(); i++) {
        Container* argumentContainer = new Container(statement->getParams()[i]->getName().getCode(), argumentValues[i]);
        this->addContainerToCurrentStack(argumentContainer);
      }

      // execute function body
      try {
        this->executeStatement(statement->getBody());
      }
      catch(ReturnSignal returnSignal) {
        // leave function stack
        this->removeScopeFromCurrentStack();
        this->memory.setCurrentStackByIndex(this->memory.getCurrentStackIndex());

        return returnSignal.getValue();
      }

      // leave function stack
      this->removeScopeFromCurrentStack();
      this->memory.setCurrentStackByIndex(this->memory.getCurrentStackIndex());
      
      // default return
      return new NullValue;
    };

    FunctionValue* functionValue = new FunctionValue(functionClosure, this->currentContentObject, callable, argumentsAmount);
    Container* functionContainer = new Container(statement->getName().getCode(), functionValue, true);
    this->addContainerToCurrentStack(functionContainer);

    return functionContainer;
  }
  void Executor::executeReturnStatement(AST::ReturnStatement *statement) {
    Container* returnContainer = this->evaluateExpression(statement->getReturns());
    this->addContainerToCurrentStack(returnContainer);

    throw ReturnSignal(statement, returnContainer->getValue());
  }
  void Executor::executeImportStatement(AST::ImportStatement *statement) {
    if (!this->isExecutionOnTopLevel()) {
      throw StatementException(statement->getPosition(), "Import statement can be used only on top level");
    }

    // get current module path
    Resolution::Module* currentModule = this->loader.getModules()[this->memory.getCurrentStackIndex()];

    // find dependency module
    std::string dependencyAbsolutePath = this->loader.resolveAbsolutePath(currentModule->getAbsolutePath(), statement->getPath().getCode());
    int moduleIndex = this->loader.getLoadedModuleIndexByAbsolutePath(dependencyAbsolutePath);

    // put dependency as current exporting module
    this->memory.setCurrentExportsRegistryByIndex(moduleIndex);

    // get exporting tokens
    std::vector<Specification::TokenType> importTokenTypes = {};
    for (int i = 0; i < statement->getImports().size(); i++) {
      importTokenTypes.push_back(statement->getImports()[i].getType());
    }

    // check for exporting all symbols
    if (Shared::Vectors::includes(importTokenTypes, Specification::TokenType::MULTIPLICATION_TOKEN)) {
      std::vector<Container*> exports = this->memory.getCurrentExportsRegistry()->getContainers();

      for (int i = 0; i < exports.size(); i++) {
        Container* constantSymbol = new Container(exports[i]->getName(), exports[i]->getValue(), true);
        this->addContainerToCurrentStack(constantSymbol);
      }
    } 
    // otherwise export each symbol
    else {
      for (int i = 0; i < statement->getImports().size(); i++) {
        Lexer::Token currentImportSymbol = statement->getImports()[i];
        Container* currentImportContainer = this->memory.getCurrentExportsRegistry()->getContainerByName(currentImportSymbol.getCode());
  
        Container* constantSymbol = new Container(currentImportSymbol.getCode(), currentImportContainer->getValue(), true);
        this->addContainerToCurrentStack(constantSymbol);
      }
    }

    // return pointer to current exports
    this->memory.setCurrentExportsRegistryByIndex(this->memory.getCurrentStackIndex());
  }
  void Executor::executeExportStatement(AST::ExportStatement *statement) {
    Container* exportingSymbol = this->executeExportingStatement(statement);
    this->memory.getCurrentExportsRegistry()->addContainer(exportingSymbol);
    this->memory.retainContainer(exportingSymbol);
  }
  Container* Executor::executeExportingStatement(AST::Statement* statement) {
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::ConstantDeclarationStatement>(statement)) {
      return this->executeConstantDeclarationStatement(Shared::Classes::cast<AST::Statement, AST::ConstantDeclarationStatement>(statement));
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::FunctionDeclarationStatement>(statement)) {
      return this->executeFunctionDeclarationStatement(Shared::Classes::cast<AST::Statement, AST::FunctionDeclarationStatement>(statement));
    }
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::ClassDeclarationStatement>(statement)) {
      return this->executeClassDeclarationStatement(Shared::Classes::cast<AST::Statement, AST::ClassDeclarationStatement>(statement));
    }

    throw StatementException(statement->getPosition(), "Invalid exporting statement");
  }
  Container* Executor::executeClassDeclarationStatement(AST::ClassDeclarationStatement* statement) {
    // TODO: set current context
    throw Exception("Not implemented");
  }
  Container* Executor::executeClassMemberDeclarationStatement(AST::ClassMemberDeclarationStatement *statement) {
    throw Exception("Not implemented");
  }
  Container* Executor::executeClassFieldDeclarationStatement(AST::ClassFieldDeclarationStatement *statement) {
    // TODO: set current context
    throw Exception("Not implemented");
  }
  Container* Executor::executeClassMethodDeclarationStatement(AST::ClassMethodDeclarationStatement *statement) {
    throw Exception("Not implemented");
  }
  void Executor::executeExpressionStatement(AST::ExpressionStatement *statement) {
    this->evaluateExpression(statement->getExpression());
  }

  // expressions
  Container* Executor::evaluateExpression(AST::Expression* expression) {
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(expression)) {
      return this->evaluateNullExpression();
    }
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::LiteralExpression>(expression)) {
      return this->evaluateLiteralExpression(Shared::Classes::cast<AST::Expression, AST::LiteralExpression>(expression));
    }
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::IdentifierExpression>(expression)) {
      return this->evaluateIdentifierExpression(Shared::Classes::cast<AST::Expression, AST::IdentifierExpression>(expression));
    }
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::UnaryOperationExpression>(expression)) {
      return this->evaluateUnaryExpression(Shared::Classes::cast<AST::Expression, AST::UnaryOperationExpression>(expression));
    }
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::BinaryOperationExpression>(expression)) {
      return this->evaluateBinaryExpression(Shared::Classes::cast<AST::Expression, AST::BinaryOperationExpression>(expression));
    }
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::GroupingExpression>(expression)) {
      return this->evaluateGroupingExpression(Shared::Classes::cast<AST::Expression, AST::GroupingExpression>(expression));
    }
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::GroupingApplicationExpression>(expression)) {
      return this->evaluateGroupingApplicationExpression(Shared::Classes::cast<AST::Expression, AST::GroupingApplicationExpression>(expression));
    }
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::AssociationExpression>(expression)) {
      return this->evaluateAssociationExpression(Shared::Classes::cast<AST::Expression, AST::AssociationExpression>(expression));
    }

    throw ExpressionException(expression->getPosition(), "Invalid expression");
  }

  Container* Executor::evaluateNullExpression() {
    NullValue* nullValue = new NullValue();
    return this->createExpressionEvaluationContainer(nullValue);
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

    return this->createExpressionEvaluationContainer(value);
  }
  Container* Executor::evaluateIdentifierExpression(AST::IdentifierExpression* expression) {
    return this->memory.getCurrentStack()->getContainerByName(expression->getName().getCode());
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

  // special expression types
  Container* Executor::evaluateAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());
    this->handleContainerValueReassignment(leftContainer, rightContainer->getValue());

    return leftContainer;
  }
  Container* Executor::evaluateMemberAccessExpression(AST::BinaryOperationExpression* expression) {
    Container* structure = this->evaluateExpression(expression->getLeft());
    Container* member = this->evaluateExpression(expression->getRight());

    Value* structureValue = structure->getValue();
    Value* memberValue = member->getValue();

    Container* result = NULL;

    // choose search type
    if (Shared::Classes::isInstanceOf<Value, ClassValue>(structureValue)) {
      result = this->evaluateStaticMemberAccessExpression(Shared::Classes::cast<Value, ClassValue>(structureValue), memberValue);
    } else {
      result = this->evaluateInstanceMemberAccessExpression(structureValue, memberValue);
    }

    // if result is found - return it
    if (result != NULL) return result;
    
    throw ExpressionException(expression->getPosition(), "Member cannot be resolved");
  }
  Container* Executor::evaluateStaticMemberAccessExpression(ClassValue* structure, Value* member) {
    // get fields
    std::vector<Field> fields = structure->getFields();

    // check class fields (static fields)
    // all the static fields are put to fields list
    // values of static fields are the pointers for base class fields (same values)
    for (int i = 0; i < fields.size(); i++) {
      // only static fields can be accessed through class name
      if (fields[i].getType() != FieldType::STATIC) continue;

      // check the member identifier
      if (!compareValues(fields[i].getKey(), member)) continue;

      // private can be used only if the current context is a class
      if (fields[i].getAccess() == FieldAccess::PRIVATE && !compareValues(fields[i].getClassOwner(), this->currentContextClass)) continue;
      // protected can be used only if the current context is a super class of a class
      if (fields[i].getAccess() == FieldAccess::PROTECTED && !isInstanceOf(fields[i].getClassOwner(), this->currentContextClass)) continue;

      // if all the filters are passed 
      Container* result = this->createTemporaryConstantContainer(fields[i].getValue());
      return result;
    }

    // TODO: check prototype

    return NULL;
  }
  Container* Executor::evaluateInstanceMemberAccessExpression(Value* structure, Value* member) {
    // check if a structure is object
    if (Shared::Classes::isInstanceOf<Value, ObjectValue>(structure)) {
      ObjectValue* castedValue = Shared::Classes::cast<Value, ObjectValue>(structure);
      std::vector<Field> entries = castedValue->getEntries();

      for (int i = 0; i < entries.size(); i++) {
        // check the member identifier
        if (!compareValues(entries[i].getKey(), member)) continue;

        // private fields can be accessed only if the context is a constructor class
        if (entries[i].getAccess() == FieldAccess::PRIVATE && !compareValues(entries[i].getClassOwner(), this->currentContextClass)) continue;
        // protected fields can be accessed only if the context is super class of constructor
        if (entries[i].getAccess() == FieldAccess::PROTECTED && !isInstanceOf(entries[i].getClassOwner(), this->currentContextClass)) continue;

        // if all the filters are passed 
        Container* result = this->createTemporaryConstantContainer(entries[i].getValue());
        return result;
      }
    }

    // otherwise the field is only in prototype
    // TODO: check prototype

    return NULL;
  }
  Container* Executor::evaluatePrototypeMemberAccessExpression(Value* prototype, Value* member) {
    return NULL;
  }

  // unary expression
  Container* Executor::evaluateNotExpression(AST::UnaryOperationExpression* expression) {
    Container* originalExpression = this->evaluateExpression(expression->getOperand());
    
    BooleanValue* complementedValue = new BooleanValue(!getBoolean(originalExpression->getValue()));
    return this->createExpressionEvaluationContainer(complementedValue);
  }
  Container* Executor::evaluateBitNotExpression(AST::UnaryOperationExpression* expression) {
    Container* operandContainer = this->evaluateExpression(expression->getOperand());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(operandContainer->getValue())) {
      long long operandValue = NumberValue::getDataOf(operandContainer->getValue());

      Value* result = new NumberValue(~operandValue);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"~\" is used with invalid type");
  }
  Container* Executor::evaluateIncrementExpression(AST::UnaryOperationExpression* expression) {
    Container* operandContainer = this->evaluateExpression(expression->getOperand());
    if (operandContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(operandContainer->getValue())) {
      double operandValue = NumberValue::getDataOf(operandContainer->getValue());

      Value* result = new NumberValue(operandValue + 1);
      this->handleContainerValueReassignment(operandContainer, result);

      return operandContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"++\" is used with invalid type");
  }
  Container* Executor::evaluateDecrementExpression(AST::UnaryOperationExpression* expression) {
    Container* operandContainer = this->evaluateExpression(expression->getOperand());
    if (operandContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(operandContainer->getValue())) {
      double operandValue = NumberValue::getDataOf(operandContainer->getValue());

      Value* result = new NumberValue(operandValue - 1);
      this->handleContainerValueReassignment(operandContainer, result);

      return operandContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"--\" is used with invalid type");
  }

  // binary expressions
  Container* Executor::evaluateAdditionExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    // number + number
    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double leftValue = NumberValue::getDataOf(leftContainer->getValue());
      double rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue + rightValue);
      return this->createExpressionEvaluationContainer(result);
    }

    // string + string
    if (Shared::Classes::isInstanceOf<Value, StringValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, StringValue>(rightContainer->getValue())) {
      std::string leftValue = StringValue::getDataOf(leftContainer->getValue());
      std::string rightValue = StringValue::getDataOf(rightContainer->getValue());

      StringValue* result = new StringValue(leftValue + rightValue);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"+\" is used with invalid type pair");
  }
  Container* Executor::evaluateSubtractionExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    // number - number
    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double leftValue = NumberValue::getDataOf(leftContainer->getValue());
      double rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue - rightValue);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"-\" is used with invalid type pair");
  }
  Container* Executor::evaluateMultiplicationExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    // number * number
    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double leftValue = NumberValue::getDataOf(leftContainer->getValue());
      double rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue * rightValue);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"*\" is used with invalid type pair");
  }
  Container* Executor::evaluateDivisionExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    // number / number
    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double leftValue = NumberValue::getDataOf(leftContainer->getValue());
      double rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue / rightValue);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"/\" is used with invalid type pair");
  }
  Container* Executor::evaluateExponentialExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double leftValue = NumberValue::getDataOf(leftContainer->getValue());
      double rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(std::pow(leftValue, rightValue));
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"**\" is used with invalid type pair");
  }
  Container* Executor::evaluateRemainderExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = (long long)NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = (long long)NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue % rightValue);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"%\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitAndExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue & rightValue);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"&\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitOrExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue | rightValue);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"|\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitXorExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue ^ rightValue);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"^\" is used with invalid type pair");
  }
  Container* Executor::evaluateLeftShiftExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue << rightValue);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"<<\" is used with invalid type pair");
  }
  Container* Executor::evaluateRightShiftExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long leftValue = NumberValue::getDataOf(leftContainer->getValue());
      long long rightValue = NumberValue::getDataOf(rightContainer->getValue());

      NumberValue* result = new NumberValue(leftValue >> rightValue);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \">>\" is used with invalid type pair");
  }
  Container* Executor::evaluateAdditionAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left + right);
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
    }
    if (Shared::Classes::isInstanceOf<Value, StringValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, StringValue>(rightContainer->getValue())) {
      std::string left = StringValue::getDataOf(leftContainer->getValue());
      std::string right = StringValue::getDataOf(rightContainer->getValue());

      Value* result = new StringValue(left + right);
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"+=\" is used with invalid type pair");
  }
  Container* Executor::evaluateSubtractionAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left - right);
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"-=\" is used with invalid type pair");
  }
  Container* Executor::evaluateMultiplicationAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left * right);
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"*=\" is used with invalid type pair");
  }
  Container* Executor::evaluateDivisionAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left / right);
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"/=\" is used with invalid type pair");
  }
  Container* Executor::evaluateExponentialAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(std::pow(left, right));
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"**=\" is used with invalid type pair");
  }
  Container* Executor::evaluateRemainderAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left % right);
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"%=\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitAndAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left & right);
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"&=\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitOrAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left | right);
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"|=\" is used with invalid type pair");
  }
  Container* Executor::evaluateBitXorAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left ^ right);
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"^=\" is used with invalid type pair");
  } 
  Container* Executor::evaluateLeftShiftAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left << right);
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
    }

    throw TypeException(expression->getPosition(), "Operator \"<<=\" is used with invalid type pair");
  }
  Container* Executor::evaluateRightShiftAssignExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    if (leftContainer->getIsConstant()) throw ExpressionException(expression->getPosition(), "Assignment to constant");

    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      long long left = NumberValue::getDataOf(leftContainer->getValue());
      long long right = NumberValue::getDataOf(rightContainer->getValue());

      Value* result = new NumberValue(left >> right);
      this->handleContainerValueReassignment(leftContainer, result);

      return leftContainer;
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
    return this->createExpressionEvaluationContainer(result);
  }
  Container* Executor::evaluateNotEqualExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    BooleanValue* result = new BooleanValue(!compareValues(leftContainer->getValue(), rightContainer->getValue()));
    return this->createExpressionEvaluationContainer(result);
  }
  Container* Executor::evaluateGreaterThanExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());
      
      BooleanValue* result = new BooleanValue(left > right);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \">\" is used with invalid type pair");
  }
  Container* Executor::evaluateLessThanExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());
      
      BooleanValue* result = new BooleanValue(left < right);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"<\" is used with invalid type pair");
  }
  Container* Executor::evaluateGreaterThanOrEqualExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());
      
      BooleanValue* result = new BooleanValue(left >= right);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \">=\" is used with invalid type pair");
  }
  Container* Executor::evaluateLessThanOrEqualExpression(AST::BinaryOperationExpression* expression) {
    Container* leftContainer = this->evaluateExpression(expression->getLeft());
    Container* rightContainer = this->evaluateExpression(expression->getRight());

    if (Shared::Classes::isInstanceOf<Value, NumberValue>(leftContainer->getValue()) && Shared::Classes::isInstanceOf<Value, NumberValue>(rightContainer->getValue())) {
      double left = NumberValue::getDataOf(leftContainer->getValue());
      double right = NumberValue::getDataOf(rightContainer->getValue());
      
      BooleanValue* result = new BooleanValue(left <= right);
      return this->createExpressionEvaluationContainer(result);
    }

    throw TypeException(expression->getPosition(), "Operator \"<=\" is used with invalid type pair");
  }

  // grouping expressions
  Container* Executor::evaluateParenthesesExpression(AST::GroupingExpression* expression) {
    if (!expression->getExpressions().size()) {
      NullValue* value = new NullValue();
      return this->createExpressionEvaluationContainer(value);
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

    for (int i = 0; i < expression->getExpressions().size(); i++) {
      list->push(this->evaluateExpression(expression->getExpressions()[i])->getValue());
    }

    return this->createExpressionEvaluationContainer(list);
  }

  // grouping application expressions
  Container* Executor::evaluateParenthesesApplicationExpression(AST::GroupingApplicationExpression* expression) {
    // validate function type
    Container* functionContainer = this->evaluateExpression(expression->getLeft());
    if (!Shared::Classes::isInstanceOf<Value, FunctionValue>(functionContainer->getValue())) {
      throw ExpressionException(expression->getPosition(), "Not a function");
    }
    FunctionValue* functionValue = Shared::Classes::cast<Value, FunctionValue>(functionContainer->getValue());
    
    // get arguments
    std::vector<AST::Expression*> argumentExpressions = expression->getRight()->getExpressions();
    std::vector<Value*> arguments = {};
    
    for (int i = 0; i < argumentExpressions.size(); i++) {
      arguments.push_back(this->evaluateExpression(argumentExpressions[i])->getValue());
    }
    
    // validate arguments amount
    FunctionArgumentsAmount functionArgumentsAmount = functionValue->getArgumentsAmount();
    if (arguments.size() < functionArgumentsAmount.getRequiredArgumentsAmount() || arguments.size() > functionArgumentsAmount.getTotalArgumentsAmount()) {
      throw ExpressionException(expression->getPosition(), "Invalid arguments amount");
    }

    // execute function
    Value* result = functionValue->execute(arguments);
    return this->createExpressionEvaluationContainer(result);
  }
  Container* Executor::evaluateSquareBracketsApplicationExpression(AST::GroupingApplicationExpression*) {
    throw Exception("Not implemented");
  }
 
  // builtins
  Container* Executor::executeBuiltinDeclaration(Builtins::BuiltinDeclaration* statement) {
    if (Shared::Classes::isInstanceOf<Builtins::BuiltinDeclaration, Builtins::ConstantBuiltinDeclaration>(statement)) {
      return this->executeBuiltinConstantDeclaration(Shared::Classes::cast<Builtins::BuiltinDeclaration, Builtins::ConstantBuiltinDeclaration>(statement));
    }
    if (Shared::Classes::isInstanceOf<Builtins::BuiltinDeclaration, Builtins::FunctionBuiltinDeclaration>(statement)) {
      return this->executeBuiltinFunctionDeclaration(Shared::Classes::cast<Builtins::BuiltinDeclaration, Builtins::FunctionBuiltinDeclaration>(statement));
    }

    throw Exception("Invalid builtin statement found");
  }
  Container* Executor::executeBuiltinConstantDeclaration(Builtins::ConstantBuiltinDeclaration* statement) {
    Container* constantContainer = new Container(statement->getName(), statement->getValue(), true);
    return constantContainer;
  }
  Container* Executor::executeBuiltinFunctionDeclaration(Builtins::FunctionBuiltinDeclaration* statement) {
    Runtime::Callable callable = [this, statement](std::vector<Value*> arguments) -> Value* {
      int argumentsLength = arguments.size();
      Runtime::FunctionArgumentsAmount callableArgumentsAmount = statement->getArgumentsAmount();

      if (argumentsLength > callableArgumentsAmount.getTotalArgumentsAmount()) {
        throw Exception("Too much arguments passed");
      }
      if (argumentsLength < callableArgumentsAmount.getRequiredArgumentsAmount()) {
        throw Exception("Not enough arguments passed");
      }

      Value* result = statement->getCallable()(arguments);
      this->memory.addTemporaryValue(result);

      return result;
    };

    Stack* functionClosure = new Stack(this->copyCurrentStack());
    FunctionValue* functionValue = new FunctionValue(functionClosure, NULL, callable, statement->getArgumentsAmount());

    Container* functionContainer = new Container(statement->getName(), functionValue, true);
    return functionContainer;
  }

  // memory management
  Stack Executor::copyCurrentStack() {
    return Stack(*this->memory.getCurrentStack());
  }
  void Executor::addScopeInCurrentStack() {
    this->memory.getCurrentStack()->addScope(Scope());
  }
  void Executor::removeScopeFromCurrentStack() {
    std::vector<Container*> containers = this->memory.getCurrentStack()->getContainersFromCurrentScope();
    
    for (int i = 0; i < containers.size(); i++) {
      this->memory.releaseContainer(containers[i]);
    }
    
    this->memory.getCurrentStack()->removeScope();
  }
  void Executor::addContainerToCurrentStack(Container* container) {
    this->memory.getCurrentStack()->addContainer(container);
    this->memory.retainContainer(container);
  }
  Container* Executor::createExpressionEvaluationContainer(Value* value) {
    this->memory.addTemporaryValue(value);
    return this->createTemporaryConstantContainer(value);
  }
  Container* Executor::createTemporaryConstantContainer(Value* value) {
    Container* container = new Container("", value, true);
    this->memory.addTemporaryContainer(container);
    return container;
  }
  void Executor::handleContainerValueReassignment(Container* container, Value* newValue) {
    this->memory.retainValue(newValue);
    this->memory.releaseValue(container->getValue());
    container->setValue(newValue);
  }
  
  // utils
  bool Executor::isExecutionOnTopLevel() {
    return this->memory.getCurrentStack()->getSize() <= TOP_LEVEL_STACK_SIZE;
  }
}
