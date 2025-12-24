#pragma once

#include "builtins/declarations.h"

#include "runtime/types.h"
#include "runtime/exceptions.h"

#include "shared/utils.h"

#include <iostream>

// module to perform console operations

namespace Builtins {
  namespace Console {
    // StringValue* _builtins_console_input();
    inline const std::string inputName = "_builtins_console_input";
    inline const FunctionArgumentsAmount inputArgumentsAmount(0);
    Runtime::StringValue* inputCallable(std::vector<Runtime::Value*>) {
      std::string raw = "";
      std::cin >> raw;

      Runtime::StringValue* value = new Runtime::StringValue(raw);
      return value;
    };
    inline const Builtins::FunctionBuiltinDeclaration inputDeclaration(inputName, inputCallable, inputArgumentsAmount);

    // void _builtins_console_output(StringValue*);
    inline const std::string outputName = "_builtins_console_output";
    inline const FunctionArgumentsAmount outputArgumentsAmount(1);
    Runtime::NullValue* outputCallable(std::vector<Runtime::Value*> arguments) {
      Runtime::Value* outputArgument = arguments[0];
      std::cout << Runtime::StringValue::getDataOf(outputArgument);
      return new Runtime::NullValue;
    };
    inline const Builtins::FunctionBuiltinDeclaration inputDeclaration(outputName, outputCallable, outputArgumentsAmount);
  
    // all declarations
    inline const Builtins::BuiltinModuleDeclarations declarations = {
      &inputDeclaration,
      &outputDeclaration,
    };
  }
}