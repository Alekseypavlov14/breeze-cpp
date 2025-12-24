#pragma once

#include "builtins/declarations.h"

#include "runtime/types.h"
#include "runtime/exceptions.h"
#include "specification/specification.h"
#include "shared/utils.h"

// module to perform types operations

namespace Builtins {
  namespace Types {
    inline const std::map<Runtime::DataType, std::string> TYPES = {
      { Runtime::DataType::Null, "null" },
      { Runtime::DataType::Boolean, "boolean" },
      { Runtime::DataType::Number, "number" },
      { Runtime::DataType::String, "string" },
      { Runtime::DataType::Vector, "vector" },
      { Runtime::DataType::Object, "object" },
      { Runtime::DataType::Class, "class" },
      { Runtime::DataType::Function, "function" },
    };

    // StringValue* _builtin_types_type(Value*)
    inline const std::string typeName = "_builtin_types_type";
    inline const FunctionArgumentsAmount typeArguments(1);
    Runtime::StringValue* typeCallable(std::vector<Runtime::Value*> arguments) {
      Runtime::Value* typeArgument = arguments[0];
      std::string type = TYPES.at(typeArgument->getType());
      return new Runtime::StringValue(type);
    }
    inline const FunctionBuiltinDeclaration typeDeclaration(typeName, typeCallable, typeArguments);

    // BooleanValue* _builtin_types_boolean(Value*)
    inline const std::string booleanName = "_builtin_types_boolean";
    inline const FunctionArgumentsAmount booleanArguments(1);
    Runtime::BooleanValue* booleanCallable(std::vector<Runtime::Value*> arguments) {
      Runtime::Value* booleanArgument = arguments[0];
      bool result = Runtime::getBoolean(booleanArgument);
      return new Runtime::BooleanValue(result);
    }
    inline const FunctionBuiltinDeclaration booleanDeclaration(booleanName, booleanCallable, booleanArguments);

    // NumberValue* _builtin_types_number(Value*)
    inline const std::string numberName = "_builtin_types_number";
    inline const FunctionArgumentsAmount numberArguments(1);
    Runtime::NumberValue* numberCallable(std::vector<Runtime::Value*> arguments) {
      Runtime::Value* numberArgument = arguments[0];
      
      if (Shared::isInstanceOf<Runtime::Value, Runtime::NullValue>(numberArgument)) {
        return new Runtime::NumberValue(0);
      } 
      if (Shared::isInstanceOf<Runtime::Value, Runtime::BooleanValue>(numberArgument)) {
        bool realValue = Runtime::BooleanValue::getDataOf(numberArgument);
        if (realValue) return new Runtime::NumberValue(1);
        else return new Runtime::NumberValue(0);
      }
      if (Shared::isInstanceOf<Runtime::Value, Runtime::NumberValue>(numberArgument)) {
        return Shared::cast<Runtime::Value, Runtime::NumberValue>(numberArgument);
      }
      if (Shared::isInstanceOf<Runtime::Value, Runtime::StringValue>(numberArgument)) {
        std::string realValue = Runtime::StringValue::getDataOf(numberArgument);
        return new Runtime::NumberValue(std::stod(realValue));
      }

      throw Runtime::Exception("Invalid type is given");
    }
    inline const FunctionBuiltinDeclaration numberDeclaration(numberName, numberCallable, numberArguments);

    // StringValue* _builtin_types_string(PrimitiveValue*)
    inline const std::string stringName = "_builtin_types_string";
    inline const FunctionArgumentsAmount stringArguments(1);
    Runtime::StringValue* stringCallable(std::vector<Runtime::Value*> arguments) {
      Runtime::Value* stringValue = arguments[0];

      if (Shared::isInstanceOf<Runtime::Value, Runtime::NullValue>(stringValue)) {
        return new Runtime::StringValue(Specification::MAP_TOKEN_TYPE_TO_STRING.at(Specification::TokenType::NULL_KEYWORD_TOKEN));
      } 
      if (Shared::isInstanceOf<Runtime::Value, Runtime::BooleanValue>(stringValue)) {
        bool realValue = Runtime::BooleanValue::getDataOf(stringValue);
        if (realValue) return new Runtime::StringValue(Specification::MAP_TOKEN_TYPE_TO_STRING.at(Specification::TokenType::TRUE_KEYWORD_TOKEN));
        else return new Runtime::StringValue(Specification::MAP_TOKEN_TYPE_TO_STRING.at(Specification::TokenType::FALSE_KEYWORD_TOKEN));
      }
      if (Shared::isInstanceOf<Runtime::Value, Runtime::NumberValue>(stringValue)) {
        double realValue = Runtime::NumberValue::getDataOf(stringValue);
        return new Runtime::StringValue(std::to_string(realValue));
      }
      if (Shared::isInstanceOf<Runtime::Value, Runtime::StringValue>(stringValue)) {
        return Shared::cast<Runtime::Value, Runtime::StringValue>(stringValue);
      }

      throw Runtime::Exception("Invalid type is given");
    }
    inline const FunctionBuiltinDeclaration stringDeclaration(stringName, stringCallable, stringArguments);

    // all declarations
    inline const BuiltinModuleDeclarations declarations = {
      &typeDeclaration,
      &booleanDeclaration,
      &numberDeclaration,
      &stringDeclaration,
    };
  }
}