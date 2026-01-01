#pragma once

#include <vector>
#include <string>
#include <map>

// this module contains information about language tokens and keywords
// this data is used in most of interpreter modules
namespace Specification {
  // contains all possible token types
  // includes both tokens and keyword types
  enum class TokenType {
    IDENTIFIER_TOKEN,
    COMMENT_TOKEN,
    NEWLINE_TOKEN,
    SPACE_TOKEN,

    LAMBDA_TOKEN,

    GREATER_THAN_OR_EQUAL_TOKEN,
    GREATER_THAN_TOKEN,
    LESS_THAN_OR_EQUAL_TOKEN,
    LESS_THAN_TOKEN,
    EQUAL_TOKEN,
    NOT_EQUAL_TOKEN,
    AND_TOKEN,
    OR_TOKEN,
    NOT_TOKEN,

    INCREMENT_TOKEN,
    DECREMENT_TOKEN,

    EXPONENTIAL_TOKEN,
    EXPONENTIAL_ASSIGN_TOKEN,
    ASSIGN_TOKEN,
    PLUS_ASSIGN_TOKEN,
    PLUS_TOKEN,
    MINUS_ASSIGN_TOKEN,
    MINUS_TOKEN,
    MULTIPLICATION_ASSIGN_TOKEN,
    MULTIPLICATION_TOKEN,
    DIVISION_ASSIGN_TOKEN,
    DIVISION_TOKEN,
    REMAINDER_ASSIGN_TOKEN,
    REMAINDER_TOKEN,

    BIT_AND_ASSIGN_TOKEN,
    BIT_AND_TOKEN,
    BIT_OR_ASSIGN_TOKEN,
    BIT_OR_TOKEN,
    BIT_XOR_ASSIGN_TOKEN,
    BIT_XOR_TOKEN,
    BIT_NOT_TOKEN,
    LEFT_SHIFT_ASSIGN_TOKEN,
    LEFT_SHIFT_TOKEN,
    RIGHT_SHIFT_ASSIGN_TOKEN,
    RIGHT_SHIFT_TOKEN,

    LEFT_PARENTHESES_TOKEN,
    RIGHT_PARENTHESES_TOKEN,
    LEFT_CURLY_BRACE_TOKEN,
    RIGHT_CURLY_BRACE_TOKEN,
    LEFT_SQUARE_BRACKET_TOKEN,
    RIGHT_SQUARE_BRACKET_TOKEN,

    COMMA_TOKEN,
    COLON_TOKEN,
    SEMICOLON_TOKEN,
    DOT_TOKEN,

    NUMBER_TOKEN,
    STRING_TOKEN,

    // used for keywords
    VARIABLE_KEYWORD_TOKEN,
    CONSTANT_KEYWORD_TOKEN,

    IF_KEYWORD_TOKEN,
    ELSE_KEYWORD_TOKEN,

    FOR_KEYWORD_TOKEN,
    WHILE_KEYWORD_TOKEN,
    CONTINUE_KEYWORD_TOKEN,
    BREAK_KEYWORD_TOKEN,
    
    FUNCTION_KEYWORD_TOKEN,
    RETURN_KEYWORD_TOKEN,
    
    IMPORT_KEYWORD_TOKEN,
    FROM_KEYWORD_TOKEN,
    EXPORT_KEYWORD_TOKEN,
    
    CLASS_KEYWORD_TOKEN,
    NEW_KEYWORD_TOKEN,

    PRIVATE_KEYWORD_TOKEN,
    PROTECTED_KEYWORD_TOKEN,
    PUBLIC_KEYWORD_TOKEN,
    
    STATIC_KEYWORD_TOKEN,
    EXTENDS_KEYWORD_TOKEN,
    CONSTRUCTOR_KEYWORD_TOKEN,

    TRUE_KEYWORD_TOKEN,
    FALSE_KEYWORD_TOKEN,
    NULL_KEYWORD_TOKEN,
  };

  // Map TokenType to string representation
  inline const std::map<TokenType, std::string> MAP_TOKEN_TYPE_TO_STRING = {
    { TokenType::IDENTIFIER_TOKEN, "identifier" },
    { TokenType::COMMENT_TOKEN, "comment" },
    { TokenType::NEWLINE_TOKEN, "newline" },
    { TokenType::SPACE_TOKEN, "space" },

    { TokenType::LAMBDA_TOKEN, "=>" },

    { TokenType::GREATER_THAN_OR_EQUAL_TOKEN, ">=" },
    { TokenType::GREATER_THAN_TOKEN, ">" },
    { TokenType::LESS_THAN_OR_EQUAL_TOKEN, "<=" },
    { TokenType::LESS_THAN_TOKEN, "<" },
    { TokenType::EQUAL_TOKEN, "==" },
    { TokenType::NOT_EQUAL_TOKEN, "!=" },
    { TokenType::AND_TOKEN, "&&" },
    { TokenType::OR_TOKEN, "||" },
    { TokenType::NOT_TOKEN, "!" },

    { TokenType::INCREMENT_TOKEN, "++" },
    { TokenType::DECREMENT_TOKEN, "--" },

    { TokenType::EXPONENTIAL_TOKEN, "**" },
    { TokenType::EXPONENTIAL_ASSIGN_TOKEN, "**=" },
    { TokenType::ASSIGN_TOKEN, "=" },
    { TokenType::PLUS_ASSIGN_TOKEN, "+=" },
    { TokenType::PLUS_TOKEN, "+" },
    { TokenType::MINUS_ASSIGN_TOKEN, "-=" },
    { TokenType::MINUS_TOKEN, "-" },
    { TokenType::MULTIPLICATION_ASSIGN_TOKEN, "*=" },
    { TokenType::MULTIPLICATION_TOKEN, "*" },
    { TokenType::DIVISION_ASSIGN_TOKEN, "/=" },
    { TokenType::DIVISION_TOKEN, "/" },
    { TokenType::REMAINDER_ASSIGN_TOKEN, "%=" },
    { TokenType::REMAINDER_TOKEN, "%" },

    { TokenType::BIT_AND_ASSIGN_TOKEN, "&=" },
    { TokenType::BIT_AND_TOKEN, "&" },
    { TokenType::BIT_OR_ASSIGN_TOKEN, "|=" },
    { TokenType::BIT_OR_TOKEN, "|" },
    { TokenType::BIT_XOR_ASSIGN_TOKEN, "^=" },
    { TokenType::BIT_XOR_TOKEN, "^" },
    { TokenType::BIT_NOT_TOKEN, "~" },
    { TokenType::LEFT_SHIFT_ASSIGN_TOKEN, "<<=" },
    { TokenType::LEFT_SHIFT_TOKEN, "<<" },
    { TokenType::RIGHT_SHIFT_ASSIGN_TOKEN, ">>=" },
    { TokenType::RIGHT_SHIFT_TOKEN, ">>" },

    { TokenType::LEFT_PARENTHESES_TOKEN, "(" },
    { TokenType::RIGHT_PARENTHESES_TOKEN, ")" },
    { TokenType::LEFT_CURLY_BRACE_TOKEN, "{" },
    { TokenType::RIGHT_CURLY_BRACE_TOKEN, "}" },
    { TokenType::LEFT_SQUARE_BRACKET_TOKEN, "[" },
    { TokenType::RIGHT_SQUARE_BRACKET_TOKEN, "]" },

    { TokenType::COMMA_TOKEN, "," },
    { TokenType::COLON_TOKEN, ":" },
    { TokenType::SEMICOLON_TOKEN, ";" },
    { TokenType::DOT_TOKEN, "." },

    { TokenType::NUMBER_TOKEN, "number" },
    { TokenType::STRING_TOKEN, "string" },

    // keywords
    { TokenType::VARIABLE_KEYWORD_TOKEN, "var" },
    { TokenType::CONSTANT_KEYWORD_TOKEN, "const" },

    { TokenType::IF_KEYWORD_TOKEN, "if" },
    { TokenType::ELSE_KEYWORD_TOKEN, "else" },

    { TokenType::FOR_KEYWORD_TOKEN, "for" },
    { TokenType::WHILE_KEYWORD_TOKEN, "while" },
    { TokenType::CONTINUE_KEYWORD_TOKEN, "continue" },
    { TokenType::BREAK_KEYWORD_TOKEN, "break" },

    { TokenType::FUNCTION_KEYWORD_TOKEN, "function" },
    { TokenType::RETURN_KEYWORD_TOKEN, "return" },

    { TokenType::IMPORT_KEYWORD_TOKEN, "import" },
    { TokenType::FROM_KEYWORD_TOKEN, "from" },
    { TokenType::EXPORT_KEYWORD_TOKEN, "export" },

    { TokenType::CLASS_KEYWORD_TOKEN, "class" },
    { TokenType::NEW_KEYWORD_TOKEN, "new" },

    { TokenType::PRIVATE_KEYWORD_TOKEN, "private" },
    { TokenType::PROTECTED_KEYWORD_TOKEN, "protected" },
    { TokenType::PUBLIC_KEYWORD_TOKEN, "public" },

    { TokenType::STATIC_KEYWORD_TOKEN, "static" },
    { TokenType::EXTENDS_KEYWORD_TOKEN, "extends" },
    { TokenType::CONSTRUCTOR_KEYWORD_TOKEN, "constructor" },

    { TokenType::TRUE_KEYWORD_TOKEN, "true" },
    { TokenType::FALSE_KEYWORD_TOKEN, "false" },
    { TokenType::NULL_KEYWORD_TOKEN, "null" }
  };

  // used for lexical analysis
  // composes token type and corresponding regex template
  struct TokenSpecification {
    const TokenType type;
    const std::string regex;
  };

  // definition of token specifications
  // keywords are obtained from identifiers
  inline const TokenSpecification IDENTIFIER_TOKEN_SPECIFICATION = { TokenType::IDENTIFIER_TOKEN, "[a-zA-Z_][a-zA-Z0-9_]*" };
  inline const TokenSpecification COMMENT_TOKEN_SPECIFICATION = { TokenType::COMMENT_TOKEN, R"(//[^\n\r]*)" };
  inline const TokenSpecification NEWLINE_TOKEN_SPECIFICATION = { TokenType::NEWLINE_TOKEN, R"(\n)" };
  inline const TokenSpecification SPACE_TOKEN_SPECIFICATION = { TokenType::SPACE_TOKEN, R"(\s)" };

  inline const TokenSpecification LAMBDA_TOKEN_SPECIFICATION = { TokenType::LAMBDA_TOKEN, R"(=>)" };

  inline const TokenSpecification GREATER_THAN_OR_EQUAL_TOKEN_SPECIFICATION = { TokenType::GREATER_THAN_OR_EQUAL_TOKEN, R"(>=)" };
  inline const TokenSpecification GREATER_THAN_TOKEN_SPECIFICATION = { TokenType::GREATER_THAN_TOKEN, R"(>)" };
  inline const TokenSpecification LESS_THAN_OR_EQUAL_TOKEN_SPECIFICATION = { TokenType::LESS_THAN_OR_EQUAL_TOKEN, R"(<=)" };
  inline const TokenSpecification LESS_THAN_TOKEN_SPECIFICATION = { TokenType::LESS_THAN_TOKEN, R"(<)" };
  inline const TokenSpecification EQUAL_TOKEN_SPECIFICATION = { TokenType::EQUAL_TOKEN, R"(==)" };
  inline const TokenSpecification NOT_EQUAL_TOKEN_SPECIFICATION = { TokenType::NOT_EQUAL_TOKEN, R"(!=)" };
  inline const TokenSpecification AND_TOKEN_SPECIFICATION = { TokenType::AND_TOKEN, R"(&&)" };
  inline const TokenSpecification OR_TOKEN_SPECIFICATION = { TokenType::OR_TOKEN, R"(\|\|)" };
  inline const TokenSpecification NOT_TOKEN_SPECIFICATION = { TokenType::NOT_TOKEN, R"(!)" };

  inline const TokenSpecification INCREMENT_TOKEN_SPECIFICATION = { TokenType::INCREMENT_TOKEN, R"(\+\+)" };
  inline const TokenSpecification DECREMENT_TOKEN_SPECIFICATION = { TokenType::DECREMENT_TOKEN, R"(--)" };

  inline const TokenSpecification EXPONENTIAL_TOKEN_SPECIFICATION = { TokenType::EXPONENTIAL_TOKEN, R"(\*\*)" };
  inline const TokenSpecification EXPONENTIAL_ASSIGN_TOKEN_SPECIFICATION = { TokenType::EXPONENTIAL_ASSIGN_TOKEN, R"(\*\*=)" };
  inline const TokenSpecification ASSIGN_TOKEN_SPECIFICATION = { TokenType::ASSIGN_TOKEN, R"(=)" };
  inline const TokenSpecification PLUS_ASSIGN_TOKEN_SPECIFICATION = { TokenType::PLUS_ASSIGN_TOKEN, R"(\+=)" };
  inline const TokenSpecification PLUS_TOKEN_SPECIFICATION = { TokenType::PLUS_TOKEN, R"(\+)" };
  inline const TokenSpecification MINUS_ASSIGN_TOKEN_SPECIFICATION = { TokenType::MINUS_ASSIGN_TOKEN, R"(-=)" };
  inline const TokenSpecification MINUS_TOKEN_SPECIFICATION = { TokenType::MINUS_TOKEN, R"(-)" };
  inline const TokenSpecification MULTIPLICATION_ASSIGN_TOKEN_SPECIFICATION = { TokenType::MULTIPLICATION_ASSIGN_TOKEN, R"(\*=)" };
  inline const TokenSpecification MULTIPLICATION_TOKEN_SPECIFICATION = { TokenType::MULTIPLICATION_TOKEN, R"(\*)" };
  inline const TokenSpecification DIVISION_ASSIGN_TOKEN_SPECIFICATION = { TokenType::DIVISION_ASSIGN_TOKEN, R"(/=)" };
  inline const TokenSpecification DIVISION_TOKEN_SPECIFICATION = { TokenType::DIVISION_TOKEN, R"(/)" };
  inline const TokenSpecification REMAINDER_ASSIGN_TOKEN_SPECIFICATION = { TokenType::REMAINDER_ASSIGN_TOKEN, R"(%=)" };
  inline const TokenSpecification REMAINDER_TOKEN_SPECIFICATION = { TokenType::REMAINDER_TOKEN, R"(%)" };

  inline const TokenSpecification BIT_AND_ASSIGN_TOKEN_SPECIFICATION = { TokenType::BIT_AND_ASSIGN_TOKEN, R"(&=)" };
  inline const TokenSpecification BIT_AND_TOKEN_SPECIFICATION = { TokenType::BIT_AND_TOKEN, R"(&)" };
  inline const TokenSpecification BIT_OR_ASSIGN_TOKEN_SPECIFICATION = { TokenType::BIT_OR_ASSIGN_TOKEN, R"(\|=)" };
  inline const TokenSpecification BIT_OR_TOKEN_SPECIFICATION = { TokenType::BIT_OR_TOKEN, R"(\|)" };
  inline const TokenSpecification BIT_XOR_ASSIGN_TOKEN_SPECIFICATION = { TokenType::BIT_XOR_ASSIGN_TOKEN, R"(\^=)" };
  inline const TokenSpecification BIT_XOR_TOKEN_SPECIFICATION = { TokenType::BIT_XOR_TOKEN, R"(\^)" };
  inline const TokenSpecification BIT_NOT_TOKEN_SPECIFICATION = { TokenType::BIT_NOT_TOKEN, R"(~)" };
  inline const TokenSpecification LEFT_SHIFT_ASSIGN_TOKEN_SPECIFICATION = { TokenType::LEFT_SHIFT_ASSIGN_TOKEN, R"(<<=)" };
  inline const TokenSpecification LEFT_SHIFT_TOKEN_SPECIFICATION = { TokenType::LEFT_SHIFT_TOKEN, R"(<<)" };
  inline const TokenSpecification RIGHT_SHIFT_ASSIGN_TOKEN_SPECIFICATION = { TokenType::RIGHT_SHIFT_ASSIGN_TOKEN, R"(>>=)" };
  inline const TokenSpecification RIGHT_SHIFT_TOKEN_SPECIFICATION = { TokenType::RIGHT_SHIFT_TOKEN, R"(>>)" };

  inline const TokenSpecification LEFT_PARENTHESES_TOKEN_SPECIFICATION = { TokenType::LEFT_PARENTHESES_TOKEN, R"(\()" };
  inline const TokenSpecification RIGHT_PARENTHESES_TOKEN_SPECIFICATION = { TokenType::RIGHT_PARENTHESES_TOKEN, R"(\))" };
  inline const TokenSpecification LEFT_CURLY_BRACE_TOKEN_SPECIFICATION = { TokenType::LEFT_CURLY_BRACE_TOKEN, R"(\{)" };
  inline const TokenSpecification RIGHT_CURLY_BRACE_TOKEN_SPECIFICATION = { TokenType::RIGHT_CURLY_BRACE_TOKEN, R"(\})" };
  inline const TokenSpecification LEFT_SQUARE_BRACKET_TOKEN_SPECIFICATION = { TokenType::LEFT_SQUARE_BRACKET_TOKEN, R"(\[)" };
  inline const TokenSpecification RIGHT_SQUARE_BRACKET_TOKEN_SPECIFICATION = { TokenType::RIGHT_SQUARE_BRACKET_TOKEN, R"(\])" };

  inline const TokenSpecification COMMA_TOKEN_SPECIFICATION = { TokenType::COMMA_TOKEN, R"(,)" };
  inline const TokenSpecification COLON_TOKEN_SPECIFICATION = { TokenType::COLON_TOKEN, R"(:)" };
  inline const TokenSpecification SEMICOLON_TOKEN_SPECIFICATION = { TokenType::SEMICOLON_TOKEN, R"(;)" };
  inline const TokenSpecification DOT_TOKEN_SPECIFICATION = { TokenType::DOT_TOKEN, R"(\.)" };

  inline const TokenSpecification NUMBER_TOKEN_SPECIFICATION = { TokenType::NUMBER_TOKEN, R"(\d[\d\.]*)" };
  inline const TokenSpecification STRING_TOKEN_SPECIFICATION = { TokenType::STRING_TOKEN, R"(\'((?:[^\'\\]|\\.)*?)\'|\"((?:[^\"\\]|\\.)*?)\")" };

  // vector of token specifications
  // does not contain keywords - they are obtained from identifiers
  inline const std::vector<TokenSpecification> TOKEN_SPECIFICATIONS = {
    IDENTIFIER_TOKEN_SPECIFICATION,
    COMMENT_TOKEN_SPECIFICATION,
    NEWLINE_TOKEN_SPECIFICATION,
    SPACE_TOKEN_SPECIFICATION,

    LAMBDA_TOKEN_SPECIFICATION,

    GREATER_THAN_OR_EQUAL_TOKEN_SPECIFICATION,
    GREATER_THAN_TOKEN_SPECIFICATION,
    LESS_THAN_OR_EQUAL_TOKEN_SPECIFICATION,
    LESS_THAN_TOKEN_SPECIFICATION,
    EQUAL_TOKEN_SPECIFICATION,
    NOT_EQUAL_TOKEN_SPECIFICATION,
    AND_TOKEN_SPECIFICATION,
    OR_TOKEN_SPECIFICATION,
    NOT_TOKEN_SPECIFICATION,

    INCREMENT_TOKEN_SPECIFICATION,
    DECREMENT_TOKEN_SPECIFICATION,

    EXPONENTIAL_TOKEN_SPECIFICATION,
    EXPONENTIAL_ASSIGN_TOKEN_SPECIFICATION,
    ASSIGN_TOKEN_SPECIFICATION,
    PLUS_ASSIGN_TOKEN_SPECIFICATION,
    PLUS_TOKEN_SPECIFICATION,
    MINUS_ASSIGN_TOKEN_SPECIFICATION,
    MINUS_TOKEN_SPECIFICATION,
    MULTIPLICATION_ASSIGN_TOKEN_SPECIFICATION,
    MULTIPLICATION_TOKEN_SPECIFICATION,
    DIVISION_ASSIGN_TOKEN_SPECIFICATION,
    DIVISION_TOKEN_SPECIFICATION,
    REMAINDER_ASSIGN_TOKEN_SPECIFICATION,
    REMAINDER_TOKEN_SPECIFICATION,

    BIT_AND_ASSIGN_TOKEN_SPECIFICATION,
    BIT_AND_TOKEN_SPECIFICATION,
    BIT_OR_ASSIGN_TOKEN_SPECIFICATION,
    BIT_OR_TOKEN_SPECIFICATION,
    BIT_XOR_ASSIGN_TOKEN_SPECIFICATION,
    BIT_XOR_TOKEN_SPECIFICATION,
    BIT_NOT_TOKEN_SPECIFICATION,
    LEFT_SHIFT_ASSIGN_TOKEN_SPECIFICATION,
    LEFT_SHIFT_TOKEN_SPECIFICATION,
    RIGHT_SHIFT_ASSIGN_TOKEN_SPECIFICATION,
    RIGHT_SHIFT_TOKEN_SPECIFICATION,

    LEFT_PARENTHESES_TOKEN_SPECIFICATION,
    RIGHT_PARENTHESES_TOKEN_SPECIFICATION,
    LEFT_CURLY_BRACE_TOKEN_SPECIFICATION,
    RIGHT_CURLY_BRACE_TOKEN_SPECIFICATION,
    LEFT_SQUARE_BRACKET_TOKEN_SPECIFICATION,
    RIGHT_SQUARE_BRACKET_TOKEN_SPECIFICATION,

    COMMA_TOKEN_SPECIFICATION,
    COLON_TOKEN_SPECIFICATION,
    SEMICOLON_TOKEN_SPECIFICATION,
    DOT_TOKEN_SPECIFICATION,

    NUMBER_TOKEN_SPECIFICATION,
    STRING_TOKEN_SPECIFICATION
  };

  // definition of language keywords
  inline const std::string VAR_KEYWORD = "var";
  inline const std::string CONST_KEYWORD = "const";

  inline const std::string IF_KEYWORD = "if";
  inline const std::string ELSE_KEYWORD = "else";

  inline const std::string FOR_KEYWORD = "for";
  inline const std::string WHILE_KEYWORD = "while";
  inline const std::string CONTINUE_KEYWORD = "continue";
  inline const std::string BREAK_KEYWORD = "break";

  inline const std::string FUNCTION_KEYWORD = "function";
  inline const std::string RETURN_KEYWORD = "return";

  inline const std::string IMPORT_KEYWORD = "import";
  inline const std::string FROM_KEYWORD = "from";
  inline const std::string EXPORT_KEYWORD = "export";

  inline const std::string CLASS_KEYWORD = "class";
  inline const std::string NEW_KEYWORD = "new";

  inline const std::string PRIVATE_KEYWORD = "private";
  inline const std::string PROTECTED_KEYWORD = "protected";
  inline const std::string PUBLIC_KEYWORD = "public";

  inline const std::string STATIC_KEYWORD = "static";
  inline const std::string EXTENDS_KEYWORD = "extends";
  inline const std::string CONSTRUCTOR_KEYWORD = "constructor";

  inline const std::string TRUE_KEYWORD = "true";
  inline const std::string FALSE_KEYWORD = "false";
  inline const std::string NULL_KEYWORD = "null";

  // vector of keywords
  inline const std::vector<std::string> KEYWORDS = {
    VAR_KEYWORD,
    CONST_KEYWORD,
    IF_KEYWORD,
    ELSE_KEYWORD,
    FOR_KEYWORD,
    WHILE_KEYWORD,
    CONTINUE_KEYWORD,
    BREAK_KEYWORD,
    FUNCTION_KEYWORD,
    RETURN_KEYWORD,
    IMPORT_KEYWORD,
    FROM_KEYWORD,
    EXPORT_KEYWORD,
    CLASS_KEYWORD,
    NEW_KEYWORD,
    PRIVATE_KEYWORD,
    PROTECTED_KEYWORD,
    PUBLIC_KEYWORD,
    STATIC_KEYWORD,
    EXTENDS_KEYWORD,
    CONSTRUCTOR_KEYWORD,
    TRUE_KEYWORD,
    FALSE_KEYWORD,
    NULL_KEYWORD
  };

  // map keyword to TokenType
  inline const std::map<std::string, TokenType> MAP_KEYWORD_TO_TOKEN_TYPE = {
    { VAR_KEYWORD, TokenType::VARIABLE_KEYWORD_TOKEN },
    { CONST_KEYWORD, TokenType::CONSTANT_KEYWORD_TOKEN },
    { IF_KEYWORD, TokenType::IF_KEYWORD_TOKEN },
    { ELSE_KEYWORD, TokenType::ELSE_KEYWORD_TOKEN },
    { FOR_KEYWORD, TokenType::FOR_KEYWORD_TOKEN },
    { WHILE_KEYWORD, TokenType::WHILE_KEYWORD_TOKEN },
    { CONTINUE_KEYWORD, TokenType::CONTINUE_KEYWORD_TOKEN },
    { BREAK_KEYWORD, TokenType::BREAK_KEYWORD_TOKEN },
    { FUNCTION_KEYWORD, TokenType::FUNCTION_KEYWORD_TOKEN },
    { RETURN_KEYWORD, TokenType::RETURN_KEYWORD_TOKEN },
    { IMPORT_KEYWORD, TokenType::IMPORT_KEYWORD_TOKEN },
    { FROM_KEYWORD, TokenType::FROM_KEYWORD_TOKEN },
    { EXPORT_KEYWORD, TokenType::EXPORT_KEYWORD_TOKEN },
    { CLASS_KEYWORD, TokenType::CLASS_KEYWORD_TOKEN },
    { NEW_KEYWORD, TokenType::NEW_KEYWORD_TOKEN },
    { PRIVATE_KEYWORD, TokenType::PRIVATE_KEYWORD_TOKEN },
    { PROTECTED_KEYWORD, TokenType::PROTECTED_KEYWORD_TOKEN },
    { PUBLIC_KEYWORD, TokenType::PUBLIC_KEYWORD_TOKEN },
    { STATIC_KEYWORD, TokenType::STATIC_KEYWORD_TOKEN },
    { EXTENDS_KEYWORD, TokenType::EXTENDS_KEYWORD_TOKEN },
    { CONSTRUCTOR_KEYWORD, TokenType::CONSTRUCTOR_KEYWORD_TOKEN },
    { TRUE_KEYWORD, TokenType::TRUE_KEYWORD_TOKEN },
    { FALSE_KEYWORD, TokenType::FALSE_KEYWORD_TOKEN },
    { NULL_KEYWORD, TokenType::NULL_KEYWORD_TOKEN }
  };

  // literal tokens
  inline const std::vector<TokenType> LITERAL_TOKENS = {
    TokenType::NUMBER_TOKEN,
    TokenType::STRING_TOKEN,
    TokenType::TRUE_KEYWORD_TOKEN,
    TokenType::FALSE_KEYWORD_TOKEN,
    TokenType::NULL_KEYWORD_TOKEN,
  };

  // access modifier tokens
  inline const std::vector<TokenType> ACCESS_MODIFIER_TOKENS = {
    TokenType::PRIVATE_KEYWORD_TOKEN,
    TokenType::PROTECTED_KEYWORD_TOKEN,
    TokenType::PUBLIC_KEYWORD_TOKEN,
  };
}
