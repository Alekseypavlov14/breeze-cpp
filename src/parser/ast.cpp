#include "ast.h"

namespace AST {
  Base::Position Node::getPosition() const {
    return this->position;
  }

  // expression variants
  NullExpression::NullExpression(Base::Position position) {
    this->position = position;
  }
  NullExpression* NullExpression::clone() const {
    return new NullExpression(this->getPosition());
  }
  
  UnaryOperationExpression::UnaryOperationExpression(Base::Position position, Lexer::Token operatorToken, Expression* operand): operatorToken(operatorToken) {
    this->position = position;
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

  PrefixUnaryOperationExpression::PrefixUnaryOperationExpression(Base::Position position, Lexer::Token operatorToken, Expression* operand): UnaryOperationExpression(position, operatorToken, operand) {}
  PrefixUnaryOperationExpression* PrefixUnaryOperationExpression::clone() const {
    return new PrefixUnaryOperationExpression(this->position, this->getOperator(), this->getOperand()->clone());
  }

  SuffixUnaryOperationExpression::SuffixUnaryOperationExpression(Base::Position position, Lexer::Token operatorToken, Expression* operand): UnaryOperationExpression(position, operatorToken, operand) {}
  SuffixUnaryOperationExpression* SuffixUnaryOperationExpression::clone() const {
    return new SuffixUnaryOperationExpression(this->position, this->getOperator(), this->getOperand()->clone());
  }
  
  AffixUnaryOperationExpression::AffixUnaryOperationExpression(Base::Position position, Lexer::Token operatorToken, Expression* operand): UnaryOperationExpression(position, operatorToken, operand) {}
  AffixUnaryOperationExpression* AffixUnaryOperationExpression::clone() const {
    return new AffixUnaryOperationExpression(this->position, this->getOperator(), this->getOperand()->clone());
  }

  BinaryOperationExpression::BinaryOperationExpression(Base::Position position, Lexer::Token operatorToken, Expression* left, Expression* right): operatorToken(operatorToken) {
    this->position = position;
    this->operatorToken = operatorToken;
    this->left = left;
    this->right = right;
  }
  BinaryOperationExpression::~BinaryOperationExpression() {
    delete this->left;
    delete this->right;
  }
  BinaryOperationExpression* BinaryOperationExpression::clone() const {
    return new BinaryOperationExpression(this->position, this->getOperator(), this->getLeft()->clone(), this->getRight()->clone());
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

  LiteralExpression::LiteralExpression(Base::Position position, Lexer::Token value): value(value) {
    this->value = value;
  }
  LiteralExpression* LiteralExpression::clone() const {
    return new LiteralExpression(this->position, this->getValue());
  }
  Lexer::Token LiteralExpression::getValue() const {
    return this->value;
  }

  IdentifierExpression::IdentifierExpression(Base::Position position, Lexer::Token name): name(name) {
    this->name = name;
  }
  IdentifierExpression* IdentifierExpression::clone() const {
    return new IdentifierExpression(this->position, this->getName());
  }
  Lexer::Token IdentifierExpression::getName() const {
    return this->name;
  }

  GroupingExpression::GroupingExpression(Base::Position position, Lexer::Token operatorToken, std::vector<Expression*> expressions): operatorToken(operatorToken) {
    this->position = position;
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

    return new GroupingExpression(this->position, this->getOperator(), expressions);
  }
  Lexer::Token GroupingExpression::getOperator() const {
    return this->operatorToken;
  }
  std::vector<Expression*> GroupingExpression::getExpressions() const {
    return this->expressions;
  }

  GroupingApplicationExpression::GroupingApplicationExpression(Base::Position position, Expression* left, GroupingExpression* right) {
    this->position = position;
    this->left = left;
    this->right = right;
  }
  GroupingApplicationExpression::~GroupingApplicationExpression() {
    delete this->left;
    delete this->right;
  }
  GroupingApplicationExpression* GroupingApplicationExpression::clone() const {
    return new GroupingApplicationExpression(this->position, this->getLeft()->clone(), this->getRight()->clone());
  }
  Expression* GroupingApplicationExpression::getLeft() const {
    return this->left;
  }
  GroupingExpression* GroupingApplicationExpression::getRight() const {
    return this->right;
  }

  AssociationExpression::AssociationExpression(Base::Position position, std::vector<std::pair<Expression*, Expression*>> entries) {
    this->position = position;
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

    return new AssociationExpression(this->position, entries);
  }
  std::vector<std::pair<Expression*, Expression*>> AssociationExpression::getEntries() const {
    return this->entries;
  }

  FunctionParameterExpression::FunctionParameterExpression(Base::Position position, Lexer::Token name, Expression* defaultValue): name(name) {
    this->name = name;
    this->defaultValue = defaultValue;
  }
  FunctionParameterExpression::~FunctionParameterExpression() {
    delete this->defaultValue;
  }
  FunctionParameterExpression* FunctionParameterExpression::clone() const {
    return new FunctionParameterExpression(this->position, this->getName(), this->getDefaultValue()->clone());
  }
  Lexer::Token FunctionParameterExpression::getName() const {
    return this->name;
  }
  Expression* FunctionParameterExpression::getDefaultValue() const {
    return this->defaultValue;
  }

  // statement variants
  NullStatement::NullStatement(Base::Position position) {
    this->position = position;
  }
  NullStatement* NullStatement::clone() const {
    return new NullStatement(this->position);
  }

  ExpressionStatement::ExpressionStatement(Base::Position position, Expression* expression) {
    this->position = position;
    this->expression = expression;
  }
  ExpressionStatement::~ExpressionStatement() {
    delete this->expression;
  }
  ExpressionStatement* ExpressionStatement::clone() const {
    return new ExpressionStatement(this->position, this->getExpression()->clone());
  }
  Expression* ExpressionStatement::getExpression() const {
    return this->expression;
  }

  BlockStatement::BlockStatement(Base::Position position, std::vector<Statement*> statements) {
    this->position = position;
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

    return new BlockStatement(this->position, statements);
  }
  std::vector<Statement*> BlockStatement::getStatements() const {
    return this->statements;
  }

  VariableDeclarationStatement::VariableDeclarationStatement(Base::Position position, Lexer::Token name, Expression* initializer): name(name) {
    this->position = position;
    this->name = name;
    this->initializer = initializer;
  }
  VariableDeclarationStatement::~VariableDeclarationStatement() {
    delete this->initializer;
  }
  VariableDeclarationStatement* VariableDeclarationStatement::clone() const {
    return new VariableDeclarationStatement(this->position, this->getName(), this->getInitializer()->clone());
  }
  Lexer::Token VariableDeclarationStatement::getName() const {
    return this->name;
  }
  Expression* VariableDeclarationStatement::getInitializer() const {
    return this->initializer;
  }

  ConstantDeclarationStatement::ConstantDeclarationStatement(Base::Position position, Lexer::Token name, Expression* initializer): name(name) {
    this->position = position;
    this->name = name;
    this->initializer = initializer;
  }
  ConstantDeclarationStatement::~ConstantDeclarationStatement() {
    delete this->initializer;
  }
  ConstantDeclarationStatement* ConstantDeclarationStatement::clone() const {
    return new ConstantDeclarationStatement(this->position, this->getName(), this->getInitializer()->clone());
  }
  Lexer::Token ConstantDeclarationStatement::getName() const {
    return this->name;
  }
  Expression* ConstantDeclarationStatement::getInitializer() const {
    return this->initializer;
  }

  ConditionStatement::ConditionStatement(Base::Position position, Expression* condition, Statement* thenBranch, Statement* elseBranch) {
    this->position = position;
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
    return new ConditionStatement(this->position, this->getCondition()->clone(), this->getThenBranch()->clone(), this->getElseBranch()->clone());
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

  WhileStatement::WhileStatement(Base::Position position, Expression* condition, Statement* body) {
    this->position = position;
    this->condition = condition;
    this->body = body;
  }
  WhileStatement::~WhileStatement() {
    delete this->condition;
    delete this->body;
  }
  WhileStatement* WhileStatement::clone() const {
    return new WhileStatement(this->position, this->getCondition()->clone(), this->getBody()->clone());
  }
  Expression* WhileStatement::getCondition() const {
    return this->condition;
  }
  Statement* WhileStatement::getBody() const {
    return this->body;
  }

  ForStatement::ForStatement(Base::Position position, Statement* initializer, Expression* condition, Expression* increment, Statement* body) {
    this->position = position;
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
      this->position, 
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

  BreakStatement::BreakStatement(Base::Position position) {
    this->position = position;
  }
  BreakStatement* BreakStatement::clone() const {
    return new BreakStatement(this->position);
  }
  ContinueStatement::ContinueStatement(Base::Position position) {
    this->position = position;
  }
  ContinueStatement* ContinueStatement::clone() const {
    return new ContinueStatement(this->position);
  }

  FunctionDeclarationStatement::FunctionDeclarationStatement(Base::Position position, Lexer::Token name, std::vector<FunctionParameterExpression*> params, BlockStatement* body) : name(name) {
    this->position = position;
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

    return new FunctionDeclarationStatement(this->position, this->getName(), parameters, this->getBody()->clone());
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

  ReturnStatement::ReturnStatement(Base::Position position, Expression* returns) {
    this->position = position;
    this->returns = returns;
  }
  ReturnStatement::~ReturnStatement() {
    delete this->returns;
  }
  ReturnStatement* ReturnStatement::clone() const {
    return new ReturnStatement(this->position, this->getReturns()->clone());
  }
  Expression* ReturnStatement::getReturns() const {
    return this->returns;
  }

  ImportStatement::ImportStatement(Base::Position position, Lexer::Token path, std::vector<Lexer::Token> imports): path(path) {
    this->position = position;
    this->path = path;
    this->imports = imports;
  }
  ImportStatement* ImportStatement::clone() const {
    return new ImportStatement(this->position, this->getPath(), this->getImports());
  }
  Lexer::Token ImportStatement::getPath() const {
    return this->path;
  }
  std::vector<Lexer::Token> ImportStatement::getImports() const {
    return this->imports;
  }

  ExportStatement::ExportStatement(Base::Position position, Statement* exports) {
    this->position = position;
    this->exports = exports;
  }
  ExportStatement::~ExportStatement() {
    delete this->exports;
  }
  ExportStatement* ExportStatement::clone() const {
    return new ExportStatement(this->position, this->getExports()->clone());
  }
  Statement* ExportStatement::getExports() const {
    return this->exports;
  }

  ClassMemberDeclarationStatement::ClassMemberDeclarationStatement(Base::Position position, Specification::TokenType accessModifier, bool isStatic, bool isConstant, Lexer::Token name): name(name) {
    this->position = position;
    this->accessModifier = accessModifier;
    this->isStatic = isStatic;
    this->isConstant = isConstant;
    this->name = name;
  }
  ClassMemberDeclarationStatement* ClassMemberDeclarationStatement::clone() const {
    return new ClassMemberDeclarationStatement(this->position, this->accessModifier, this->isStatic, this->isConstant, this->name);
  }
  Specification::TokenType ClassMemberDeclarationStatement::getAccessModifier() const {
    return this->accessModifier;
  }
  bool ClassMemberDeclarationStatement::getIsStatic() const {
    return this->isStatic;
  }
  bool ClassMemberDeclarationStatement::getIsConstant() const {
    return this->isConstant;
  }
  Lexer::Token ClassMemberDeclarationStatement::getName() const {
    return this->name;
  }

  ClassFieldDeclarationStatement::ClassFieldDeclarationStatement(Base::Position position, Specification::TokenType accessModifier, bool isStatic, bool isConstant, Lexer::Token name, Expression* initialization): ClassMemberDeclarationStatement(position, accessModifier, isStatic, isConstant, name) {
    this->initialization = initialization;
  }
  ClassFieldDeclarationStatement::~ClassFieldDeclarationStatement() {
    delete this->initialization;
  }
  ClassFieldDeclarationStatement* ClassFieldDeclarationStatement::clone() const {
    return new ClassFieldDeclarationStatement(this->position, this->accessModifier, this->isStatic, this->isConstant, this->name, this->initialization->clone());
  }
  Expression* ClassFieldDeclarationStatement::getInitialization() const {
    return this->initialization;
  }

  ClassMethodDeclarationStatement::ClassMethodDeclarationStatement(Base::Position position, Specification::TokenType accessModifier, bool isStatic, Lexer::Token name, std::vector<FunctionParameterExpression*> params, BlockStatement* body): ClassMemberDeclarationStatement(position, accessModifier, isStatic, isConstant, name) {
    this->params = params;
    this->body = body;
  }
  ClassMethodDeclarationStatement::~ClassMethodDeclarationStatement() {
    for (int i = 0; i < this->params.size(); i++) {
      delete this->params[i];
    }
    delete this->body;
  }
  ClassMethodDeclarationStatement* ClassMethodDeclarationStatement::clone() const {
    std::vector<FunctionParameterExpression*> clonedParams = {};

    for (int i = 0; i < this->params.size(); i++) {
      clonedParams.push_back(this->params[i]->clone());
    }

    return new ClassMethodDeclarationStatement(this->position, this->accessModifier, this->isStatic, this->name, clonedParams, this->body->clone());
  }
  std::vector<FunctionParameterExpression*> ClassMethodDeclarationStatement::getParams() const {
    return this->params;
  }
  BlockStatement* ClassMethodDeclarationStatement::getBody() const {
    return this->body;
  }

  ClassDeclarationStatement::ClassDeclarationStatement(Base::Position position, Lexer::Token name, std::vector<Expression*> extensionExpressions, std::vector<ClassMemberDeclarationStatement*> declarations): name(name) {
    this->position = position;
    this->name = name;
    this->extensionExpressions = extensionExpressions;
    this->declarations = declarations;
  }
  ClassDeclarationStatement::~ClassDeclarationStatement() {
    for (int i = 0; i < this->declarations.size(); i++) {
      delete this->declarations[i];
    }
    for (int i = 0; i < this->extensionExpressions.size(); i++) {
      delete this->extensionExpressions[i];
    }
  }
  ClassDeclarationStatement* ClassDeclarationStatement::clone() const {
    std::vector<ClassMemberDeclarationStatement*> clonedDeclarations = {};    
    for (int i = 0; i < this->declarations.size(); i++) {
      clonedDeclarations.push_back(this->declarations[i]->clone());
    }

    std::vector<Expression*> clonedExtensionExpressions = {};
    for (int i = 0; i < this->extensionExpressions.size(); i++) {
      clonedExtensionExpressions.push_back(this->extensionExpressions[i]->clone());
    }

    return new ClassDeclarationStatement(this->position, this->getName(), clonedExtensionExpressions, clonedDeclarations);
  }
  Lexer::Token ClassDeclarationStatement::getName() const {
    return this->name;
  }
  std::vector<Expression*> ClassDeclarationStatement::getExtensionExpressions() const {
    return this->extensionExpressions;
  }
  std::vector<ClassMemberDeclarationStatement*> ClassDeclarationStatement::getDeclarations() const {
    return this->declarations;
  }
}
