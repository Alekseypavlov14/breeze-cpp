#include "parser/parser.h"
#include "parser/exception.h"
#include "parser/operators.h"
#include "specification/specification.h"
#include "shared/classes.h"
#include "shared/vectors.h"

#include <iostream>

namespace Parser {
  // parser class implementation
  Parser::Parser() {
    this->tokens = {};
    this->position = 0;
  }
  
  AST::BlockStatement* Parser::parse(std::vector<Lexer::Token> tokens) {
    this->tokens = tokens;
    this->position = 0;

    std::vector<AST::Statement*> statements = {};
    std::vector<Specification::TokenType> terminators = {};

    Base::Position position = this->getCurrentToken().getPosition();

    while (!this->isEnd()) {
      statements.push_back(this->parseStatement(terminators));
    }

    return new AST::BlockStatement(position, statements);
  }
  AST::Statement* Parser::parseStatement(std::vector<Specification::TokenType> terminators) {
    this->skipMultilineSpaceTokens();

    if (this->matchVariableDeclarationStatement()) {
      return this->parseVariableDeclaration(terminators);
    } 
    if (this->matchConstantDeclarationStatement()) {
      return this->parseConstantDeclaration(terminators);
    }
    if (this->matchConditionStatement()) {
      return this->parseConditionStatement(terminators);
    } 
    if (this->matchForStatement()) {
      return this->parseForStatement(terminators);
    }
    if (this->matchWhileStatement()) {
      return this->parseWhileStatement(terminators);
    }
    if (this->matchBreakStatement()) {
      return this->parseBreakStatement();
    }
    if (this->matchContinueStatement()) {
      return this->parseContinueStatement();
    }
    if (this->matchFunctionDeclarationStatement()) {
      return this->parseFunctionDeclarationStatement();
    }
    if (this->matchReturnStatement()) {
      return this->parseReturnStatement(terminators);
    }
    if (this->matchClassDeclarationStatement()) {
      return this->parseClassDeclarationStatement();
    }
    if (this->matchImportStatement()) {
      return this->parseImportStatement();
    }
    if (this->matchExportStatement()) {
      return this->parseExportStatement(terminators);
    }
    if (this->matchBlockStatement()) {
      return this->parseBlockStatement();
    }
    if (this->matchComment()) {
      return this->parseComment();
    }

    // by default, parse expression statement
    return this->parseExpressionStatement(terminators);
  }

  AST::VariableDeclarationStatement* Parser::parseVariableDeclaration(std::vector<Specification::TokenType> terminators) {
    // get variable keyword
    this->requireToken(Specification::TokenType::VARIABLE_KEYWORD_TOKEN);
    Lexer::Token variableToken = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // get variable name
    this->requireToken(Specification::TokenType::IDENTIFIER_TOKEN);
    Lexer::Token identifier = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // check if variable is not initialized with value
    if (this->matchToken(Specification::TokenType::NEWLINE_TOKEN)) {
      Lexer::Token newlineToken = this->consumeCurrentToken();

      AST::NullExpression* initializer = new AST::NullExpression(newlineToken.getPosition());
      AST::VariableDeclarationStatement* variableDeclarationStatement = new AST::VariableDeclarationStatement(variableToken.getPosition(), identifier, initializer);

      return variableDeclarationStatement;
    }

    // check if variable is initialized
    if (this->matchToken(Specification::TokenType::ASSIGN_TOKEN)) {
      // consume assign token
      this->consumeCurrentToken();
      // skip spaces
      this->skipSingleLineSpaceTokens();
    
      // add newline to terminators list
      std::vector<Specification::TokenType> newTerminators = terminators;
      newTerminators.push_back(Specification::TokenType::NEWLINE_TOKEN);
      
      // parse initializer
      AST::Expression* initializer = this->parseExpression(NULL, BASE_PRECEDENCE, newTerminators);
      
      // check if it is not NullExpression
      if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(initializer)) {
        Base::Position position = this->getCurrentToken().getPosition();
        throw Exception(position, "Invalid variable initialization");
      }

      this->skipMultilineSpaceTokens();

      // compose variable declaration statement
      AST::VariableDeclarationStatement* variableDeclarationStatement = new AST::VariableDeclarationStatement(variableToken.getPosition(), identifier, initializer);
      return variableDeclarationStatement;
    }

    // if nothing matched then invalid case is obtained
    Base::Position position = this->getCurrentToken().getPosition();
    throw Exception(position, "Invalid variable initialization");
  }
  AST::ConstantDeclarationStatement* Parser::parseConstantDeclaration(std::vector<Specification::TokenType> terminators) {
    // require const keyword
    this->requireToken(Specification::TokenType::CONSTANT_KEYWORD_TOKEN);
    Lexer::Token constantToken = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // require identifier
    this->requireToken(Specification::TokenType::IDENTIFIER_TOKEN);
    Lexer::Token identifier = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();
    
    // check assignment
    if (!this->matchToken(Specification::TokenType::ASSIGN_TOKEN)) {
      Base::Position position = this->getCurrentToken().getPosition();
      throw Exception(position, "Constant declaration requires initialization");
    }

    // consume assign token
    this->requireToken(Specification::TokenType::ASSIGN_TOKEN);
    this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // update terminators list
    std::vector<Specification::TokenType> newTerminators = terminators;
    newTerminators.push_back(Specification::TokenType::NEWLINE_TOKEN);

    AST::Expression* initializer = this->parseExpression(NULL, BASE_PRECEDENCE, newTerminators);

    // check if it is not NullExpression
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(initializer)) {
      Base::Position position = this->getCurrentToken().getPosition();
      throw Exception(position, "Invalid constant initialization");
    }

    this->skipMultilineSpaceTokens();

    // compose declaration statement
    AST::ConstantDeclarationStatement* constantDeclarationStatement = new AST::ConstantDeclarationStatement(constantToken.getPosition(), identifier, initializer);
    return constantDeclarationStatement;
  }
  AST::ConditionStatement* Parser::parseConditionStatement(std::vector<Specification::TokenType> terminators) {
    // get if keyword token
    this->requireToken(Specification::TokenType::IF_KEYWORD_TOKEN);
    Lexer::Token ifToken = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // require left parentheses
    this->requireToken(Specification::TokenType::LEFT_PARENTHESES_TOKEN);
    this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // compose condition terminators
    std::vector<Specification::TokenType> conditionTerminators = {};
    conditionTerminators.push_back(Specification::TokenType::RIGHT_PARENTHESES_TOKEN);

    AST::Expression* conditionExpression = this->parseExpression(NULL, BASE_PRECEDENCE, conditionTerminators);
    
    // check if is null expression
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(conditionExpression)) {
      Base::Position position = this->getCurrentToken().getPosition();
      throw Exception(position, "Invalid condition expression");
    } 

    this->skipMultilineSpaceTokens();

    // get closing parentheses 
    this->requireToken(Specification::TokenType::RIGHT_PARENTHESES_TOKEN);
    this->consumeCurrentToken();

    this->skipMultilineSpaceTokens();

    // check if file is finished
    if (this->isEnd()) {
      Base::Position position = this->getPreviousToken().getPosition();
      throw Exception(position, "Invalid condition then branch");
    }

    // parse then statement
    AST::Statement* thenStatement = this->parseStatement(terminators);

    this->skipMultilineSpaceTokens();

    // check if null statement
    if (Shared::Classes::isInstanceOf<AST::Statement, AST::NullStatement>(thenStatement)) {
      Base::Position position = this->getCurrentToken().getPosition();
      throw Exception(position, "Invalid condition then branch");
    }
    
    // parse else statement
    AST::Statement* elseStatement = NULL;

    if (this->matchToken(Specification::TokenType::ELSE_KEYWORD_TOKEN)) {
      this->consumeCurrentToken();
      this->skipSingleLineSpaceTokens();

      elseStatement = this->parseStatement(terminators);

      // check if null statement
      if (Shared::Classes::isInstanceOf<AST::Statement, AST::NullStatement>(elseStatement)) {
        Base::Position position = this->getCurrentToken().getPosition();
        throw Exception(position, "Invalid condition else branch");
      }
    } else {
      elseStatement = new AST::NullStatement(this->getCurrentToken().getPosition());
    }

    AST::ConditionStatement* conditionStatement = new AST::ConditionStatement(ifToken.getPosition(), conditionExpression, thenStatement, elseStatement);
    return conditionStatement;
  }
  AST::WhileStatement* Parser::parseWhileStatement(std::vector<Specification::TokenType> terminators) {
    // require while keyword
    this->requireToken(Specification::TokenType::WHILE_KEYWORD_TOKEN);
    Lexer::Token whileToken = this->consumeCurrentToken();
  
    this->skipSingleLineSpaceTokens();

    this->requireToken(Specification::TokenType::LEFT_PARENTHESES_TOKEN);
    this->consumeCurrentToken();

    this->skipMultilineSpaceTokens();

    std::vector<Specification::TokenType> conditionTerminators = {};
    conditionTerminators.push_back(Specification::TokenType::RIGHT_PARENTHESES_TOKEN);

    // parse condition
    AST::Expression* condition = this->parseExpression(NULL, BASE_PRECEDENCE, conditionTerminators);
  
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(condition)) {
      Base::Position position = this->getCurrentToken().getPosition();
      throw Exception(position, "Invalid while loop condition");
    }

    this->skipMultilineSpaceTokens();

    // require right parentheses
    this->requireToken(Specification::TokenType::RIGHT_PARENTHESES_TOKEN);
    this->consumeCurrentToken();

    this->skipMultilineSpaceTokens();

    // check if file is finished
    if (this->isEnd()) {
      Base::Position position = this->getPreviousToken().getPosition();
      throw Exception(position, "Invalid while loop body");
    }

    // parse body
    AST::Statement* body = this->parseStatement(terminators);

    if (Shared::Classes::isInstanceOf<AST::Statement, AST::NullStatement>(body)) {
      Base::Position position = this->getCurrentToken().getPosition();
      throw Exception(position, "Invalid while loop body");
    }

    AST::WhileStatement* whileStatement = new AST::WhileStatement(whileToken.getPosition(), condition, body);
    return whileStatement;
  }
  AST::ForStatement* Parser::parseForStatement(std::vector<Specification::TokenType> terminators) {
    // require FOR keyword
    this->requireToken(Specification::TokenType::FOR_KEYWORD_TOKEN);
    Lexer::Token forToken = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();
    
    // require left parentheses
    this->requireToken(Specification::TokenType::LEFT_PARENTHESES_TOKEN);
    this->consumeCurrentToken();

    this->skipMultilineSpaceTokens();

    std::vector<Specification::TokenType> initializerTerminators = {};
    initializerTerminators.push_back(Specification::TokenType::SEMICOLON_TOKEN);

    // parse loop initializer
    AST::Statement* initializer = this->parseStatement(initializerTerminators);

    if (Shared::Classes::isInstanceOf<AST::Statement, AST::NullStatement>(initializer)) {
      Base::Position position = this->getCurrentToken().getPosition();
      throw Exception(position, "Invalid for loop initializer");
    }

    this->skipMultilineSpaceTokens();

    // pass semicolon separator
    this->requireToken(Specification::TokenType::SEMICOLON_TOKEN);
    this->consumeCurrentToken();

    this->skipMultilineSpaceTokens();

    std::vector<Specification::TokenType> conditionTerminators = {};
    conditionTerminators.push_back(Specification::TokenType::SEMICOLON_TOKEN);

    // parse loop condition
    AST::Expression* condition = this->parseExpression(NULL, BASE_PRECEDENCE, conditionTerminators);

    if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(condition)) {
      Base::Position position = this->getCurrentToken().getPosition();
      throw Exception(position, "Invalid for loop condition");
    }

    this->skipMultilineSpaceTokens();

    // pass semicolon separator
    this->requireToken(Specification::TokenType::SEMICOLON_TOKEN);
    this->consumeCurrentToken();

    this->skipMultilineSpaceTokens();

    std::vector<Specification::TokenType> incrementTerminators = {};
    incrementTerminators.push_back(Specification::TokenType::RIGHT_PARENTHESES_TOKEN);

    // parse loop increment
    AST::Expression* increment = this->parseExpression(NULL, BASE_PRECEDENCE, incrementTerminators);
  
    if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(increment)) {
      Base::Position position = this->getCurrentToken().getPosition();
      throw Exception(position, "Invalid for loop increment");
    }

    this->skipMultilineSpaceTokens();

    this->requireToken(Specification::TokenType::RIGHT_PARENTHESES_TOKEN);
    this->consumeCurrentToken();

    this->skipMultilineSpaceTokens();

    // check if file is finished
    if (this->isEnd()) {
      Base::Position position = this->getPreviousToken().getPosition();
      throw Exception(position, "Invalid for loop body");
    }

    // parse body loop
    AST::Statement* body = this->parseStatement(terminators);

    if (Shared::Classes::isInstanceOf<AST::Statement, AST::NullStatement>(body)) {
      Base::Position position = this->getCurrentToken().getPosition();
      throw Exception(position, "Invalid for loop body");
    }

    // compose for loop
    AST::ForStatement* forStatement = new AST::ForStatement(forToken.getPosition(), initializer, condition, increment, body);
    return forStatement;
  }
  AST::BreakStatement* Parser::parseBreakStatement() {
    this->requireToken(Specification::TokenType::BREAK_KEYWORD_TOKEN);
    Lexer::Token breakToken = this->consumeCurrentToken();

    this->requireNewlineForNextStatement();

    AST::BreakStatement* breakStatement = new AST::BreakStatement(breakToken.getPosition());
    return breakStatement;
  }
  AST::ContinueStatement* Parser::parseContinueStatement() {
    this->requireToken(Specification::TokenType::CONTINUE_KEYWORD_TOKEN);
    Lexer::Token continueToken = this->consumeCurrentToken();

    this->requireNewlineForNextStatement();

    AST::ContinueStatement* continueStatement = new AST::ContinueStatement(continueToken.getPosition());
    return continueStatement;
  }
  AST::FunctionDeclarationStatement* Parser::parseFunctionDeclarationStatement() {
    // get function keyword
    this->requireToken(Specification::TokenType::FUNCTION_KEYWORD_TOKEN);
    Lexer::Token functionToken = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // get function name
    this->requireToken(Specification::TokenType::IDENTIFIER_TOKEN);
    Lexer::Token name = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // parse parameters in parentheses
    std::vector<AST::FunctionParameterExpression*> parameters = this->parseFunctionParameterExpressionList();

    this->skipMultilineSpaceTokens();

    // check if file is finished
    if (this->isEnd()) {
      Base::Position position = this->getPreviousToken().getPosition();
      throw Exception(position, "Invalid function body");
    }

    // parse body
    AST::BlockStatement* body = this->parseBlockStatement();
    
    AST::FunctionDeclarationStatement* functionDeclarationStatement = new AST::FunctionDeclarationStatement(functionToken.getPosition(), name, parameters, body); 
    return functionDeclarationStatement;
  }
  std::vector<AST::FunctionParameterExpression*> Parser::parseFunctionParameterExpressionList() {
    this->requireToken(Specification::TokenType::LEFT_PARENTHESES_TOKEN);
    this->consumeCurrentToken();

    this->skipMultilineSpaceTokens();

    // parameters list
    std::vector<AST::FunctionParameterExpression*> parameters = {};

    // get parameters
    while (!this->isEnd() && !this->matchToken(Specification::TokenType::RIGHT_PARENTHESES_TOKEN)) {
      this->skipMultilineSpaceTokens();

      // parse function
      AST::FunctionParameterExpression* parameter = this->parseFunctionParameterExpression();
      parameters.push_back(parameter);

      this->skipMultilineSpaceTokens();

      // check if the end of parameters is reached
      if (this->matchToken(Specification::TokenType::RIGHT_PARENTHESES_TOKEN)) break;

      // if not, comma is required
      this->requireToken(Specification::TokenType::COMMA_TOKEN);
      this->consumeCurrentToken();

      this->skipMultilineSpaceTokens();
    }

    this->requireToken(Specification::TokenType::RIGHT_PARENTHESES_TOKEN);
    this->consumeCurrentToken();

    return parameters;
  }
  AST::FunctionParameterExpression* Parser::parseFunctionParameterExpression() {
    this->skipMultilineSpaceTokens();

    // get param name
    this->requireToken(Specification::TokenType::IDENTIFIER_TOKEN);
    Lexer::Token name = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();
  
    // compute default value expression
    AST::Expression* defaultValue = NULL;

    if (this->matchToken(Specification::TokenType::ASSIGN_TOKEN)) {
      this->consumeCurrentToken();
      this->skipSingleLineSpaceTokens();

      std::vector<Specification::TokenType> terminators = {};
      terminators.push_back(Specification::TokenType::RIGHT_PARENTHESES_TOKEN);
      terminators.push_back(Specification::TokenType::COMMA_TOKEN);
    
      // compute default value
      defaultValue = this->parseExpression(NULL, BASE_PRECEDENCE, terminators);
    }

    AST::FunctionParameterExpression* parameter = new AST::FunctionParameterExpression(name.getPosition(), name, defaultValue);
    return parameter;
  }
  AST::ReturnStatement* Parser::parseReturnStatement(std::vector<Specification::TokenType> terminators) {
    // consume return keyword
    this->requireToken(Specification::TokenType::RETURN_KEYWORD_TOKEN);
    Lexer::Token returnToken = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    std::vector<Specification::TokenType> newTerminators = terminators;
    newTerminators.push_back(Specification::TokenType::NEWLINE_TOKEN);

    AST::Expression* expression = this->parseExpression(NULL, BASE_PRECEDENCE, newTerminators);

    AST::ReturnStatement* returnStatement = new AST::ReturnStatement(returnToken.getPosition(), expression);
    return returnStatement;
  }
  AST::ImportStatement* Parser::parseImportStatement() {
    this->requireToken(Specification::TokenType::IMPORT_KEYWORD_TOKEN);
    Lexer::Token importToken = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // list of imports
    std::vector<Lexer::Token> imports = {};

    // handle import list case
    if (this->matchToken(Specification::TokenType::LEFT_CURLY_BRACE_TOKEN)) {
      this->consumeCurrentToken();
      
      this->skipMultilineSpaceTokens();

      while (!this->isEnd() && !this->matchToken(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN)) {
        this->skipMultilineSpaceTokens();

        // parse import token
        this->requireToken(Specification::TokenType::IDENTIFIER_TOKEN);
        Lexer::Token importToken = this->consumeCurrentToken();

        imports.push_back(importToken);

        this->skipMultilineSpaceTokens();
      
        if (this->matchToken(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN)) break;

        this->requireToken(Specification::TokenType::COMMA_TOKEN);
        this->consumeCurrentToken();

        this->skipMultilineSpaceTokens();
      }

      this->requireToken(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN);
      this->consumeCurrentToken();
    }
    // handle import all case
    else if (this->matchToken(Specification::TokenType::MULTIPLICATION_TOKEN)) {
      Lexer::Token asterisk = this->consumeCurrentToken();
      imports.push_back(asterisk);
    }

    this->skipSingleLineSpaceTokens();

    this->requireToken(Specification::TokenType::FROM_KEYWORD_TOKEN);
    this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    this->requireToken(Specification::TokenType::STRING_TOKEN);
    Lexer::Token path = this->consumeCurrentToken();

    this->requireNewlineForNextStatement();

    AST::ImportStatement* importStatement = new AST::ImportStatement(importToken.getPosition(), path, imports);
    return importStatement;
  }
  AST::ExportStatement* Parser::parseExportStatement(std::vector<Specification::TokenType> terminators) {
    this->requireToken(Specification::TokenType::EXPORT_KEYWORD_TOKEN);
    Lexer::Token exportToken = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    std::vector<Specification::TokenType> newTerminators = terminators;
    newTerminators.push_back(Specification::TokenType::NEWLINE_TOKEN);

    // parse exports statement
    AST::Statement* exports = this->parseStatement(newTerminators);

    // validate exports statement type
    if (
      !Shared::Classes::isInstanceOf<AST::Statement, AST::ConstantDeclarationStatement>(exports) &&
      !Shared::Classes::isInstanceOf<AST::Statement, AST::FunctionDeclarationStatement>(exports) &&
      !Shared::Classes::isInstanceOf<AST::Statement, AST::ClassDeclarationStatement>(exports)
    ) {
      Base::Position position = exportToken.getPosition();
      throw Exception(position, "Invalid export statement");
    }

    AST::ExportStatement* exportStatement = new AST::ExportStatement(exportToken.getPosition(), exports);
    return exportStatement;
  }
  AST::BlockStatement* Parser::parseBlockStatement() {
    this->requireToken(Specification::TokenType::LEFT_CURLY_BRACE_TOKEN);
    Lexer::Token blockToken = this->consumeCurrentToken();

    std::vector<AST::Statement*> statements = {};

    std::vector<Specification::TokenType> terminators = {};
    terminators.push_back(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN);

    while (!this->isEnd() && !this->matchToken(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN)) {
      this->skipMultilineSpaceTokens();

      AST::Statement* statement = this->parseStatement(terminators);
      statements.push_back(statement);
    }

    this->skipMultilineSpaceTokens();

    this->requireToken(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN);
    this->consumeCurrentToken();

    this->requireNewlineForNextStatement();
   
    AST::BlockStatement* blockStatement = new AST::BlockStatement(blockToken.getPosition(), statements);
    return blockStatement;
  }
  AST::ExpressionStatement* Parser::parseExpressionStatement(std::vector<Specification::TokenType> terminators) {
    std::vector<Specification::TokenType> newTerminators = terminators;
    newTerminators.push_back(Specification::TokenType::NEWLINE_TOKEN);

    AST::Expression* expression = this->parseExpression(NULL, BASE_PRECEDENCE, newTerminators);

    AST::ExpressionStatement* statement = new AST::ExpressionStatement(expression->getPosition(), expression);
    return statement;
  }
  AST::ClassDeclarationStatement* Parser::parseClassDeclarationStatement() {
    // validate statement type
    this->requireToken(Specification::TokenType::CLASS_KEYWORD_TOKEN);
    Lexer::Token classToken = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // get class name
    this->requireToken(Specification::TokenType::IDENTIFIER_TOKEN);
    Lexer::Token className = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // check for extensions
    AST::Expression* extendedExpression;

    if (this->matchToken(Specification::TokenType::EXTENDS_KEYWORD_TOKEN)) {
      this->consumeCurrentToken();
      this->skipSingleLineSpaceTokens();

      std::vector<Specification::TokenType> terminators = {
        Specification::TokenType::NEWLINE_TOKEN,
        Specification::TokenType::LEFT_CURLY_BRACE_TOKEN,
      };
    
      extendedExpression = this->parseExpression(NULL, BASE_PRECEDENCE, terminators);
    }
    else {
      extendedExpression = new AST::NullExpression(className.getPosition());
    }

    this->skipMultilineSpaceTokens();

    // parse class body
    this->requireToken(Specification::TokenType::LEFT_CURLY_BRACE_TOKEN);
    Lexer::Token blockToken = this->consumeCurrentToken();

    std::vector<AST::ClassMemberDeclarationStatement*> declarations = {};

    // parse declarations
    while (!this->isEnd() && !this->matchToken(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN)) {
      this->skipMultilineSpaceTokens();

      AST::ClassMemberDeclarationStatement* declaration = this->parseClassMemberDeclarationStatement();
      declarations.push_back(declaration);
    }

    this->skipMultilineSpaceTokens();

    this->requireToken(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN);
    this->consumeCurrentToken();

    return new AST::ClassDeclarationStatement(classToken.getPosition(), className, extendedExpression, declarations);
  }
  AST::ClassMemberDeclarationStatement* Parser::parseClassMemberDeclarationStatement() {
    // predefine class member properties
    Specification::TokenType accessModifier;
    bool isStatic;
    bool isConstant;

    // special case for constructor
    if (this->matchToken(Specification::TokenType::CONSTRUCTOR_KEYWORD_TOKEN)) {
      accessModifier = Specification::TokenType::PUBLIC_KEYWORD_TOKEN;
      isStatic = true;
      isConstant = true;
    } else {
      // get access modifier
      if (this->matchTokens(Specification::ACCESS_MODIFIER_TOKENS)) {
        accessModifier = this->consumeCurrentToken().getType();
      } else {
        accessModifier = Specification::TokenType::PRIVATE_KEYWORD_TOKEN;
      }

      this->skipSingleLineSpaceTokens();

      // get static modifier
      isStatic = this->matchToken(Specification::TokenType::STATIC_KEYWORD_TOKEN);

      this->skipSingleLineSpaceTokens();
    
      // get constant keyword
      isConstant = this->matchToken(Specification::TokenType::CONSTANT_KEYWORD_TOKEN);
    }

    this->skipSingleLineSpaceTokens();

    // get field name
    std::vector<Specification::TokenType> nameTokenOptions = {
      Specification::TokenType::IDENTIFIER_TOKEN,
      Specification::TokenType::CONSTRUCTOR_KEYWORD_TOKEN,
    };
    this->requireTokens(nameTokenOptions);
    Lexer::Token nameToken = this->consumeCurrentToken();

    this->skipSingleLineSpaceTokens();

    // if a method is being parsed
    if (this->matchToken(Specification::TokenType::LEFT_PARENTHESES_TOKEN)) {
      // get parameters
      std::vector<AST::FunctionParameterExpression*> parameters = this->parseFunctionParameterExpressionList();
       
      this->skipMultilineSpaceTokens();

      AST::BlockStatement* body = this->parseBlockStatement();

      return new AST::ClassMethodDeclarationStatement(nameToken.getPosition(), accessModifier, isStatic, nameToken, parameters, body);
    }
  
    // otherwise it is a field
    if (this->matchToken(Specification::TokenType::NEWLINE_TOKEN)) {
      AST::Expression* initialization = new AST::NullExpression(nameToken.getPosition());
      return new AST::ClassFieldDeclarationStatement(nameToken.getPosition(), accessModifier, isStatic, isConstant, nameToken, initialization);
    }
    if (this->matchToken(Specification::TokenType::ASSIGN_TOKEN)) {
      this->consumeCurrentToken();

      this->skipSingleLineSpaceTokens();

      std::vector<Specification::TokenType> terminators = {
        Specification::TokenType::NEWLINE_TOKEN
      };

      AST::Expression* initialization = this->parseExpression(NULL, BASE_PRECEDENCE, terminators);
      return new AST::ClassFieldDeclarationStatement(nameToken.getPosition(), accessModifier, isStatic, isConstant, nameToken, initialization);
    }

    throw Exception(nameToken.getPosition(), "Invalid class member is used");
  }
  AST::NullStatement* Parser::parseComment() {
    this->requireToken(Specification::TokenType::COMMENT_TOKEN);
    Lexer::Token commentToken = this->consumeCurrentToken();

    while (!this->isEnd() && !this->matchToken(Specification::TokenType::NEWLINE_TOKEN)) {
      this->consumeCurrentToken();
    }

    if (this->matchToken(Specification::TokenType::NEWLINE_TOKEN)) {
      this->consumeCurrentToken();
    } 
    
    AST::NullStatement* nullStatement = new AST::NullStatement(commentToken.getPosition());
    return nullStatement;
  }

  bool Parser::matchVariableDeclarationStatement() {
    return this->matchToken(Specification::TokenType::VARIABLE_KEYWORD_TOKEN);
  }
  bool Parser::matchConstantDeclarationStatement() {
    return this->matchToken(Specification::TokenType::CONSTANT_KEYWORD_TOKEN);
  }
  bool Parser::matchConditionStatement() {
    return this->matchToken(Specification::TokenType::IF_KEYWORD_TOKEN);
  }
  bool Parser::matchForStatement() {
    return this->matchToken(Specification::TokenType::FOR_KEYWORD_TOKEN);
  }
  bool Parser::matchWhileStatement() {
    return this->matchToken(Specification::TokenType::WHILE_KEYWORD_TOKEN);
  }
  bool Parser::matchBreakStatement() {
    return this->matchToken(Specification::TokenType::BREAK_KEYWORD_TOKEN);
  }
  bool Parser::matchContinueStatement() {
    return this->matchToken(Specification::TokenType::CONTINUE_KEYWORD_TOKEN);
  }
  bool Parser::matchFunctionDeclarationStatement() {
    return this->matchToken(Specification::TokenType::FUNCTION_KEYWORD_TOKEN);
  }
  bool Parser::matchReturnStatement() {
    return this->matchToken(Specification::TokenType::RETURN_KEYWORD_TOKEN);
  }
  bool Parser::matchClassDeclarationStatement() {
    return this->matchToken(Specification::TokenType::CLASS_KEYWORD_TOKEN);
  }
  bool Parser::matchImportStatement() {
    return this->matchToken(Specification::TokenType::IMPORT_KEYWORD_TOKEN);
  }
  bool Parser::matchExportStatement() {
    return this->matchToken(Specification::TokenType::EXPORT_KEYWORD_TOKEN);
  }
  bool Parser::matchBlockStatement() {
    return this->matchToken(Specification::TokenType::LEFT_CURLY_BRACE_TOKEN);
  }
  bool Parser::matchComment() {
    return this->matchToken(Specification::TokenType::COMMENT_TOKEN);    
  }

  AST::Expression* Parser::parseExpression(AST::Expression* baseExpression, int basePrecedence, std::vector<Specification::TokenType> terminators) {
    // parse cases when baseExpression is given
    if (baseExpression != NULL) {
      this->skipSingleLineSpaceTokens();

      std::vector<Specification::TokenType> newTerminators = terminators;
      newTerminators.push_back(Specification::TokenType::NEWLINE_TOKEN);

      // if the line is finished - complete the expression
      if (this->isEnd() || this->matchTokens(newTerminators)) {
        return baseExpression;
      }
      // check if base expression is followed by another expression without operator
      else if (!this->matchTokens(OPERATION_PRECEDENCE)) {
        Base::Position position = this->getCurrentToken().getPosition();
        throw Exception(position, "Invalid expression composition");
      }
    }

    this->skipMultilineSpaceTokens();

    // initialize a list of passed tokens
    std::vector<Lexer::Token> passedTokens = {};

    // pass tokens until the end or until operator
    while (!this->isEnd() && !this->matchTokens(OPERATION_PRECEDENCE)) {
      // check expression termination
      if (this->matchTokens(terminators)) {
        return this->parseExpressionFromTokens(passedTokens);
      }

      // consume current token
      passedTokens.push_back(this->consumeCurrentToken());

      this->skipSingleLineSpaceTokens();
    }

    // check if the end is reached
    if (this->isEnd() || this->matchTokens(terminators)) {
      return this->parseExpressionFromTokens(passedTokens);
    }

    // otherwise the operator token is matched
    Lexer::Token operatorToken = this->consumeCurrentToken();
    int currentPrecedence = getOperatorPrecedence(operatorToken.getType());

    // handle unary operations
    if (isUnaryOperator(operatorToken.getType())) {
      // handle cases when base precedence is higher than current precedence
      // higher base precedence - complete base expression first
      if (
        (basePrecedence > currentPrecedence) || 
        (basePrecedence == currentPrecedence && isLeftAssociativeOperator(operatorToken.getType()))
      ) {
        // do not pass current operator
        this->decrementPosition();

        // if base expression is not present
        if (baseExpression == NULL) {
          return this->parseExpressionFromTokens(passedTokens);
        }

        // complete base expression
        return this->parseExpression(baseExpression, BASE_PRECEDENCE, terminators);
      }

      // otherwise handle cases when current precedence is higher
      // higher current precedence - work with current operation

      // check if some tokens or base expression is present
      // if so, suffix operation is found
      if (baseExpression != NULL || passedTokens.size() > 0) {
        // validate suffix operator
        if (!isSuffixOperator(operatorToken.getType()) && !isAffixUnaryOperator(operatorToken.getType())) {
          Base::Position position = operatorToken.getPosition();
          throw Exception(position, "Prefix operator is used in suffix position");
        }

        // compute operand expression
        AST::Expression* operand = this->getExpressionFromBaseExpresionOrPassedTokens(baseExpression, passedTokens);

        if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(operand)) {
          Base::Position position = this->getCurrentToken().getPosition();
          throw Exception(position, "Invalid operand");
        }

        // compose operation
        AST::SuffixUnaryOperationExpression* operation = new AST::SuffixUnaryOperationExpression(operatorToken.getPosition(), operatorToken, operand);
        
        // continue parsing based on this expression
        return this->parseExpression(operation, currentPrecedence, terminators);
      }
    
      // otherwise prefix expression is present
      if (!isPrefixOperator(operatorToken.getType()) && !isAffixUnaryOperator(operatorToken.getType())) {
        Base::Position position = operatorToken.getPosition();
        throw Exception(position, "Suffix operator is used in prefix position");
      }

      // no base expression and no tokens here 
      AST::Expression* operand = this->parseExpression(NULL, BASE_PRECEDENCE, terminators);

      if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(operand)) {
        Base::Position position = this->getCurrentToken().getPosition();
        throw Exception(position, "Invalid operand");
      }

      AST::SuffixUnaryOperationExpression* operation = new AST::SuffixUnaryOperationExpression(operatorToken.getPosition(), operatorToken, operand);

      // continue parsing based on this expression
      return this->parseExpression(operation, currentPrecedence, terminators);
    }
  
    // handle binary operations
    if (isBinaryOperator(operatorToken.getType())) {
      // handle cases when base precedence is higher than or equal to current precedence
      if (
        (basePrecedence > currentPrecedence) ||
        (basePrecedence == currentPrecedence && isLeftAssociativeOperator(operatorToken.getType())) 
      ) {
        // do not pass current operator
        this->decrementPosition();

        // if not base precedence passed
        if (baseExpression == NULL) {
          return this->parseExpressionFromTokens(passedTokens);
        }

        return this->parseExpression(baseExpression, BASE_PRECEDENCE, terminators);
      }

      // otherwise current precedence is higher
      // parse current operation first

      // get left branch
      AST::Expression* leftBranch = this->getExpressionFromBaseExpresionOrPassedTokens(baseExpression, passedTokens);

      if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(leftBranch)) {
        Base::Position position = this->getCurrentToken().getPosition();
        throw Exception(position, "Invalid operand");
      }

      // get right branch
      AST::Expression* rightBranch = this->parseExpression(NULL, BASE_PRECEDENCE, terminators);
    
      if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(leftBranch)) {
        Base::Position position = this->getCurrentToken().getPosition();
        throw Exception(position, "Invalid operand");
      }

      // compose expression
      AST::BinaryOperationExpression* operation = new AST::BinaryOperationExpression(operatorToken.getPosition(), operatorToken, leftBranch, rightBranch);
    
      return this->parseExpression(operation, currentPrecedence, terminators);
    }
  
    // handle grouping operations
    if (isGroupingOperator(operatorToken.getType())) {
      // remember position before grouping operator
      int positionBeforeGroupingTerminator = this->position - 1;
    
      // get grouping closing token
      Specification::TokenType closingToken = getGroupingClosingTokenType(operatorToken.getType());

      // get grouping terminators
      std::vector<Specification::TokenType> groupingItemTerminators = terminators;
      groupingItemTerminators.push_back(Specification::TokenType::COMMA_TOKEN);
      groupingItemTerminators.push_back(closingToken);

      std::vector<AST::Expression*> expressions = {};

      this->skipMultilineSpaceTokens();

      // parse expressions
      while (!this->isEnd() && !this->matchToken(closingToken)) {
        this->skipMultilineSpaceTokens();

        // parse expression
        AST::Expression* expression = this->parseExpression(NULL, BASE_PRECEDENCE, groupingItemTerminators);

        if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(expression)) {
          Base::Position position = this->getCurrentToken().getPosition();
          throw Exception(position, "Invalid expression");
        }

        expressions.push_back(expression);

        this->skipMultilineSpaceTokens();

        if (this->matchToken(closingToken)) break;

        this->requireToken(Specification::TokenType::COMMA_TOKEN);
        this->consumeCurrentToken();

        this->skipMultilineSpaceTokens();
      }

      // consume closing token
      this->requireToken(closingToken);
      this->consumeCurrentToken();

      // compose grouping expression
      AST::GroupingExpression* groupingExpression = new AST::GroupingExpression(operatorToken.getPosition(), operatorToken, expressions);

      // in case base precedence is higher, complete that first
      if (basePrecedence >= currentPrecedence) {
        this->position = positionBeforeGroupingTerminator;

        // if base expression is not present
        if (baseExpression == NULL) {
          return this->parseExpressionFromTokens(passedTokens);
        }

        return this->parseExpression(baseExpression, BASE_PRECEDENCE, terminators);
      }
    
      // if no base expression and no passed tokens, return grouping expression
      if (baseExpression == NULL && passedTokens.size() == 0) {
        return groupingExpression;
      }

      // otherwise, compute left branch
      AST::Expression* leftBranch = this->getExpressionFromBaseExpresionOrPassedTokens(baseExpression, passedTokens);

      if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(leftBranch)) {
        Base::Position position = this->getCurrentToken().getPosition();
        throw Exception(position, "Invalid expression");
      }

      // compose grouping application expression
      AST::GroupingApplicationExpression* groupingApplicationExpression = new AST::GroupingApplicationExpression(groupingExpression->getOperator().getPosition(), leftBranch, groupingExpression);

      return this->parseExpression(groupingApplicationExpression, BASE_PRECEDENCE, terminators);
    }
  
    // handle association operations
    if (isAssociationOperator(operatorToken.getType())) {
      // initialize entries
      std::vector<std::pair<AST::Expression*, AST::Expression*>> entries = {};

      this->skipMultilineSpaceTokens();

      // shared terminators
      std::vector<Specification::TokenType> keyExpressionTerminators = {};
      keyExpressionTerminators.push_back(Specification::TokenType::COLON_TOKEN);
      keyExpressionTerminators.push_back(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN);

      std::vector<Specification::TokenType> valueExpressionTerminators = {};
      valueExpressionTerminators.push_back(Specification::TokenType::COMMA_TOKEN);
      valueExpressionTerminators.push_back(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN);

      while (!this->isEnd() && !this->matchToken(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN)) {
        this->skipMultilineSpaceTokens();

        // initialize entry - expressions pair
        AST::Expression* keyExpression = NULL;
        AST::Expression* valueExpression = NULL;

        // parse key expression
        keyExpression = this->parseExpression(NULL, BASE_PRECEDENCE, keyExpressionTerminators);

        if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(keyExpression)) {
          Base::Position position = this->getCurrentToken().getPosition();
          throw Exception(position, "Invalid key expression");
        }

        this->skipMultilineSpaceTokens();

        // if the current entry is finished
        // then given expression is both key and value - shortcut
        if (this->matchToken(Specification::TokenType::COMMA_TOKEN) || this->matchToken(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN)) {
          valueExpression = keyExpression->clone();
        }
        // if colon token is reached then value expression is also given 
        else if (this->matchToken(Specification::TokenType::COLON_TOKEN)) {
          this->consumeCurrentToken();

          this->skipMultilineSpaceTokens();

          valueExpression = this->parseExpression(NULL, BASE_PRECEDENCE, valueExpressionTerminators);

          if (Shared::Classes::isInstanceOf<AST::Expression, AST::NullExpression>(keyExpression)) {
            Base::Position position = this->getCurrentToken().getPosition();
            throw Exception(position, "Invalid value expression");
          }
        }

        // push parsed expressions pair
        entries.push_back({
          keyExpression,
          valueExpression,
        });

        this->skipMultilineSpaceTokens();

        // stop if the association is ended
        if (this->matchToken(Specification::TokenType::RIGHT_CURLY_BRACE_TOKEN)) break;
      
        // otherwise expect comma
        this->requireToken(Specification::TokenType::COMMA_TOKEN);
        this->consumeCurrentToken();

        this->skipMultilineSpaceTokens();
      }

      // compose expressions
      AST::AssociationExpression* associationExpression = new AST::AssociationExpression(operatorToken.getPosition(), entries);

      return this->parseExpression(associationExpression, currentPrecedence, terminators);
    }
  
    // handle invalid operators
    Base::Position position = operatorToken.getPosition();
    throw Exception(position, "Invalid operator");
  }
  AST::Expression* Parser::getExpressionFromBaseExpresionOrPassedTokens(AST::Expression* baseExpression, std::vector<Lexer::Token> tokens) {
    // default case
    if (!tokens.size()) {
      return baseExpression;
    }

    return this->parseExpressionFromTokens(tokens);
  }
  AST::Expression* Parser::parseExpressionFromTokens(std::vector<Lexer::Token> tokens) {
    // handle null expression
    if (tokens.size() == 0) {
      if (this->isEnd()) return new AST::NullExpression(this->getPreviousToken().getPosition());
      return new AST::NullExpression(this->getCurrentToken().getPosition());
    }
    // handle literal tokens
    else if (tokens.size() == 1 && isLiteralToken(tokens[0].getType())) {
      return new AST::LiteralExpression(tokens[0].getPosition(), tokens[0]);
    } 
    // handle identifier token
    else if (tokens.size() == 1 && isIdentifierToken(tokens[0].getType())) {
      return new AST::IdentifierExpression(tokens[0].getPosition(), tokens[0]);
    }

    // otherwise these tokens do not form a valid expression
    Base::Position position = tokens[0].getPosition();
    throw Exception(position, "Invalid tokens combination");
  };
  bool Parser::isLiteralToken(Specification::TokenType type) {
    return Shared::Vectors::includes<Specification::TokenType>(Specification::LITERAL_TOKENS, type);
  }
  bool Parser::isIdentifierToken(Specification::TokenType type) {
    return type == Specification::TokenType::IDENTIFIER_TOKEN;
  }

  void Parser::requireNewlineForNextStatement() {
    this->skipSingleLineSpaceTokens();
    if (this->isEnd()) return;

    this->requireToken(Specification::TokenType::NEWLINE_TOKEN);
    this->consumeCurrentToken();
  }
  void Parser::skipSingleLineSpaceTokens() {
    this->skipToken(Specification::TokenType::SPACE_TOKEN);
  }
  void Parser::skipMultilineSpaceTokens() {
    std::vector<Specification::TokenType> skipList = {
      Specification::TokenType::SPACE_TOKEN, 
      Specification::TokenType::NEWLINE_TOKEN,
    };

    this->skipTokens(skipList);
  }

  void Parser::requireToken(Specification::TokenType tokenType) {
    Lexer::Token currentToken = this->getCurrentToken();

    if (!this->matchToken(tokenType)) {
      std::string message = "Invalid token. Required: \"";
      message += Specification::MAP_TOKEN_TYPE_TO_STRING.at(tokenType);
      message += "\"";

      throw Exception(currentToken.getPosition(), message);
    }
  }
  void Parser::requireTokens(std::vector<Specification::TokenType> tokenTypes) {
    Lexer::Token currentToken = this->getCurrentToken();

    if (!Shared::Vectors::includes<Specification::TokenType>(tokenTypes, currentToken.getType())) {
      throw Exception(currentToken.getPosition(), "Invalid token");
    }
  }

  bool Parser::matchToken(Specification::TokenType tokenType) {
    if (this->isEnd()) return false;

    Lexer::Token currentToken = this->getCurrentToken();
    return currentToken.isOfType(tokenType);
  }
  bool Parser::matchTokens(std::vector<Specification::TokenType> tokensTypes) {
    for (int i = 0; i < tokensTypes.size(); i++) {
      if (this->matchToken(tokensTypes[i])) {
        return true;
      }
    }

    return false;
  }

  void Parser::skipToken(Specification::TokenType tokenType) {
    while (this->matchToken(tokenType)) {
      this->incrementPosition();
    }
  }
  void Parser::skipTokens(std::vector<Specification::TokenType> tokenTypes) {
    while (this->matchTokens(tokenTypes)) {
      this->incrementPosition();
    }
  }

  Lexer::Token Parser::consumeCurrentToken() {
    this->incrementPosition();
    return this->getPreviousToken();
  };
  Lexer::Token Parser::getCurrentToken() {
    return this->tokens[this->position];
  };
  Lexer::Token Parser::getPreviousToken() {
    return this->tokens[this->position - 1];
  }

  void Parser::incrementPosition() {
    this->position += 1;
  }
  void Parser::decrementPosition() {
    this->position -= 1;
  }
  
  bool Parser::isEnd() {
    return this->position >= this->tokens.size();
  }
}
