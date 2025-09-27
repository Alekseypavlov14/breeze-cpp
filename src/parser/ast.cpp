#include "ast.h"

namespace AST {
  // expression variants
  NullExpression* NullExpression::clone() const {
    return new NullExpression;
  }
  
  UnaryOperationExpression::UnaryOperationExpression(Lexer::Token operatorToken, Expression* operand): operatorToken(operatorToken) {
    this->operatorToken = operatorToken;
    this->operand = operand;
  }
  UnaryOperationExpression::~UnaryOperationExpression() {
    delete this->operand;
  }
  Lexer::Token UnaryOperationExpression::getOperator() const {
    return this->operatorToken;
  }
  Expression* UnaryOperationExpression::getOperand() const {
    return this->operand;
  }

  PrefixUnaryOperationExpression::PrefixUnaryOperationExpression(Lexer::Token operatorToken, Expression* operand): UnaryOperationExpression(operatorToken, operand) {}
  PrefixUnaryOperationExpression* PrefixUnaryOperationExpression::clone() const {
    return new PrefixUnaryOperationExpression(this->getOperator(), this->getOperand()->clone());
  }

  SuffixUnaryOperationExpression::SuffixUnaryOperationExpression(Lexer::Token operatorToken, Expression* operand): UnaryOperationExpression(operatorToken, operand) {}
  SuffixUnaryOperationExpression* SuffixUnaryOperationExpression::clone() const {
    return new SuffixUnaryOperationExpression(this->getOperator(), this->getOperand()->clone());
  }
  
  AffixUnaryOperationExpression::AffixUnaryOperationExpression(Lexer::Token operatorToken, Expression* operand): UnaryOperationExpression(operatorToken, operand) {}
  AffixUnaryOperationExpression* AffixUnaryOperationExpression::clone() const {
    return new AffixUnaryOperationExpression(this->getOperator(), this->getOperand()->clone());
  }

  BinaryOperationExpression::BinaryOperationExpression(Lexer::Token operatorToken, Expression* left, Expression* right): operatorToken(operatorToken) {
    this->operatorToken = operatorToken;
    this->left = left;
    this->right = right;
  }
  BinaryOperationExpression::~BinaryOperationExpression() {
    delete this->left;
    delete this->right;
  }
  BinaryOperationExpression* BinaryOperationExpression::clone() const {
    return new BinaryOperationExpression(this->getOperator(), this->getLeft()->clone(), this->getRight()->clone());
  }
  Lexer::Token BinaryOperationExpression::getOperator() const {
    return this->operatorToken;
  }
  Expression* BinaryOperationExpression::getLeft() const {
    return this->left;
  }
  Expression* BinaryOperationExpression::getRight() const {
    return this->right;
  }

  LiteralExpression::LiteralExpression(Lexer::Token value): value(value) {
    this->value = value;
  }
  LiteralExpression* LiteralExpression::clone() const {
    return new LiteralExpression(this->getValue());
  }
  Lexer::Token LiteralExpression::getValue() const {
    return this->value;
  }

  IdentifierExpression::IdentifierExpression(Lexer::Token name): name(name) {
    this->name = name;
  }
  IdentifierExpression* IdentifierExpression::clone() const {
    return new IdentifierExpression(this->getName());
  }
  Lexer::Token IdentifierExpression::getName() const {
    return this->name;
  }

  GroupingExpression::GroupingExpression(Lexer::Token operatorToken, std::vector<Expression*> expressions): operatorToken(operatorToken) {
    this->operatorToken = operatorToken;
    this->expressions = expressions;
  }
  GroupingExpression::~GroupingExpression() {
    for (int i = 0; i < this->expressions.size(); i++) {
      delete this->expressions[i];
    }
  }
  GroupingExpression* GroupingExpression::clone() const {
    std::vector<Expression*> expressions = {};

    for (int i = 0; i < this->getExpressions().size(); i++) {
      expressions.push_back(this->getExpressions()[i]->clone());
    }

    return new GroupingExpression(this->getOperator(), expressions);
  }
  Lexer::Token GroupingExpression::getOperator() const {
    return this->operatorToken;
  }
  std::vector<Expression*> GroupingExpression::getExpressions() const {
    return this->expressions;
  }

  GroupingApplicationExpression::GroupingApplicationExpression(Expression* left, GroupingExpression* right) {
    this->left = left;
    this->right = right;
  }
  GroupingApplicationExpression::~GroupingApplicationExpression() {
    delete this->left;
    delete this->right;
  }
  GroupingApplicationExpression* GroupingApplicationExpression::clone() const {
    return new GroupingApplicationExpression(this->getLeft()->clone(), this->getRight()->clone());
  }
  Expression* GroupingApplicationExpression::getLeft() const {
    return this->left;
  }
  GroupingExpression* GroupingApplicationExpression::getRight() const {
    return this->right;
  }

  AssociationExpression::AssociationExpression(std::vector<std::pair<Expression*, Expression*>> entries) {
    this->entries = entries;
  }
  AssociationExpression::~AssociationExpression() {
    for (int i = 0; i < this->entries.size(); i++) {
      delete this->entries[i].first;
      delete this->entries[i].second;
    }
  }
  AssociationExpression* AssociationExpression::clone() const {
    std::vector<std::pair<Expression*, Expression*>> entries = {};

    for (int i = 0; i < this->getEntries().size(); i++) {
      entries.push_back({
        this->getEntries()[i].first->clone(),
        this->getEntries()[i].second->clone(),
      });
    }

    return new AssociationExpression(entries);
  }
  std::vector<std::pair<Expression*, Expression*>> AssociationExpression::getEntries() const {
    return this->entries;
  }

  FunctionParameterExpression::FunctionParameterExpression(Lexer::Token name, Expression* defaultValue): name(name) {
    this->name = name;
    this->defaultValue = defaultValue;
  }
  FunctionParameterExpression::~FunctionParameterExpression() {
    delete this->defaultValue;
  }
  FunctionParameterExpression* FunctionParameterExpression::clone() const {
    return new FunctionParameterExpression(this->getName(), this->getDefaultValue()->clone());
  }
  Lexer::Token FunctionParameterExpression::getName() const {
    return this->name;
  }
  Expression* FunctionParameterExpression::getDefaultValue() const {
    return this->defaultValue;
  }

  // statement variants
  NullStatement* NullStatement::clone() const {
    return new NullStatement;
  }

  ExpressionStatement::ExpressionStatement(Expression* expression) {
    this->expression = expression;
  }
  ExpressionStatement::~ExpressionStatement() {
    delete this->expression;
  }
  ExpressionStatement* ExpressionStatement::clone() const {
    return new ExpressionStatement(this->getExpression()->clone());
  }
  Expression* ExpressionStatement::getExpression() const {
    return this->expression;
  }

  BlockStatement::BlockStatement(std::vector<Statement*> statements) {
    this->statements = statements;
  }
  BlockStatement::~BlockStatement() {
    for (int i = 0; i < this->statements.size(); i++) {
      delete this->statements[i];
    }
  }
  BlockStatement* BlockStatement::clone() const {
    std::vector<Statement*> statements = {};

    for (int i = 0; i < this->getStatements().size(); i++) {
      statements.push_back(this->getStatements()[i]->clone());
    }

    return new BlockStatement(statements);
  }
  std::vector<Statement*> BlockStatement::getStatements() const {
    return this->statements;
  }

  VariableDeclarationStatement::VariableDeclarationStatement(Lexer::Token name, Expression* initializer): name(name) {
    this->name = name;
    this->initializer = initializer;
  }
  VariableDeclarationStatement::~VariableDeclarationStatement() {
    delete this->initializer;
  }
  VariableDeclarationStatement* VariableDeclarationStatement::clone() const {
    return new VariableDeclarationStatement(this->getName(), this->getInitializer()->clone());
  }
  Lexer::Token VariableDeclarationStatement::getName() const {
    return this->name;
  }
  Expression* VariableDeclarationStatement::getInitializer() const {
    return this->initializer;
  }

  ConstantDeclarationStatement::ConstantDeclarationStatement(Lexer::Token name, Expression* initializer): name(name) {
    this->name = name;
    this->initializer = initializer;
  }
  ConstantDeclarationStatement::~ConstantDeclarationStatement() {
    delete this->initializer;
  }
  ConstantDeclarationStatement* ConstantDeclarationStatement::clone() const {
    return new ConstantDeclarationStatement(this->getName(), this->getInitializer()->clone());
  }
  Lexer::Token ConstantDeclarationStatement::getName() const {
    return this->name;
  }
  Expression* ConstantDeclarationStatement::getInitializer() const {
    return this->initializer;
  }

  ConditionStatement::ConditionStatement(Expression* condition, Statement* thenBranch, Statement* elseBranch) {
    this->condition = condition;
    this->thenBranch = thenBranch;
    this->elseBranch = elseBranch;
  }
  ConditionStatement::~ConditionStatement() {
    delete this->condition;
    delete this->thenBranch;
    delete this->elseBranch;
  }
  ConditionStatement* ConditionStatement::clone() const {
    return new ConditionStatement(this->getCondition()->clone(), this->getThenBranch()->clone(), this->getElseBranch()->clone());
  }
  Expression* ConditionStatement::getCondition() const {
    return this->condition;
  }
  Statement* ConditionStatement::getThenBranch() const {
    return this->thenBranch;
  }
  Statement* ConditionStatement::getElseBranch() const {
    return this->elseBranch;
  }

  WhileStatement::WhileStatement(Expression* condition, Statement* body) {
    this->condition = condition;
    this->body = body;
  }
  WhileStatement::~WhileStatement() {
    delete this->condition;
    delete this->body;
  }
  WhileStatement* WhileStatement::clone() const {
    return new WhileStatement(this->getCondition()->clone(), this->getBody()->clone());
  }
  Expression* WhileStatement::getCondition() const {
    return this->condition;
  }
  Statement* WhileStatement::getBody() const {
    return this->body;
  }

  ForStatement::ForStatement(Statement* initializer, Expression* condition, Expression* increment, Statement* body) {
    this->initializer = initializer;
    this->condition = condition;
    this->increment = increment;
    this->body = body;
  }
  ForStatement::~ForStatement() {
    delete this->initializer;
    delete this->condition;
    delete this->increment;
    delete this->body;
  }
  ForStatement* ForStatement::clone() const {
    return new ForStatement(
      this->getInitializer()->clone(),
      this->getCondition()->clone(),
      this->getIncrement()->clone(),
      this->getBody()->clone()
    );
  }
  Statement* ForStatement::getInitializer() const {
    return this->initializer;
  }
  Expression* ForStatement::getCondition() const {
    return this->condition;
  }
  Expression* ForStatement::getIncrement() const {
    return this->increment;
  }
  Statement* ForStatement::getBody() const {
    return this->body;
  }

  BreakStatement* BreakStatement::clone() const {
    return new BreakStatement;
  }
  ContinueStatement* ContinueStatement::clone() const {
    return new ContinueStatement;
  }

  FunctionDeclarationStatement::FunctionDeclarationStatement(Lexer::Token name, std::vector<FunctionParameterExpression*> params, BlockStatement* body) : name(name) {
    this->name = name;
    this->params = params;
    this->body = body;
  }
  FunctionDeclarationStatement::~FunctionDeclarationStatement() {
    for (int i = 0; i < this->params.size(); i++) {
      delete this->params[i];
    }
    delete this->body;
  }
  FunctionDeclarationStatement* FunctionDeclarationStatement::clone() const {
    std::vector<FunctionParameterExpression*> parameters = {};

    for (int i = 0; i < this->getParams().size(); i++) {
      parameters.push_back(this->getParams()[i]->clone());
    }

    return new FunctionDeclarationStatement(this->getName(), parameters, this->getBody()->clone());
  }
  Lexer::Token FunctionDeclarationStatement::getName() const {
    return this->name;
  }
  std::vector<FunctionParameterExpression*> FunctionDeclarationStatement::getParams() const {
    return this->params;
  }
  BlockStatement* FunctionDeclarationStatement::getBody() const {
    return this->body;
  }

  ReturnStatement::ReturnStatement(Expression* returns) {
    this->returns = returns;
  }
  ReturnStatement::~ReturnStatement() {
    delete this->returns;
  }
  ReturnStatement* ReturnStatement::clone() const {
    return new ReturnStatement(this->getReturns()->clone());
  }
  Expression* ReturnStatement::getReturns() const {
    return this->returns;
  }

  // TODO: implement class declaration statement
  ClassDeclarationStatement::ClassDeclarationStatement(Lexer::Token name): name(name) {
    this->name = name;
  }
  ClassDeclarationStatement* ClassDeclarationStatement::clone() const {
    return new ClassDeclarationStatement(this->getName());
  }
  Lexer::Token ClassDeclarationStatement::getName() const {
    return this->name;
  }

  ImportStatement::ImportStatement(Lexer::Token path, std::vector<Lexer::Token> imports): path(path) {
    this->path = path;
    this->imports = imports;
  }
  ImportStatement* ImportStatement::clone() const {
    return new ImportStatement(this->getPath(), this->getImports());
  }
  Lexer::Token ImportStatement::getPath() const {
    return this->path;
  }
  std::vector<Lexer::Token> ImportStatement::getImports() const {
    return this->imports;
  }

  ExportStatement::ExportStatement(Statement* exports) {
    this->exports = exports;
  }
  ExportStatement::~ExportStatement() {
    delete this->exports;
  }
  ExportStatement* ExportStatement::clone() const {
    return new ExportStatement(this->getExports()->clone());
  }
  Statement* ExportStatement::getExports() const {
    return this->exports;
  }
}
