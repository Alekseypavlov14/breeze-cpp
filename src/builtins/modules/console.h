#pragma once

#include "builtins/declarations.h"

#include "runtime/types.h"
#include "runtime/exceptions.h"

#include "shared/classes.h"

#include <iostream>

// module to perform console operations

namespace Builtins {
  namespace Console {
    // StringValue* _builtins_console_input();
    inline const std::string inputName = "_builtins_console_input";
    inline const Runtime::FunctionArgumentsAmount inputArgumentsAmount(0);
    inline Runtime::StringValue* inputCallable(std::vector<Runtime::Value*>) {
      std::string raw = "";
      std::cin >> raw;

      Runtime::StringValue* value = new Runtime::StringValue(raw);
      return value;
    };
    inline Builtins::FunctionBuiltinDeclaration inputDeclaration(inputName, inputCallable, inputArgumentsAmount);

    // void _builtins_console_output(StringValue*);
    inline const std::string outputName = "_builtins_console_output";
    inline const Runtime::FunctionArgumentsAmount outputArgumentsAmount(1);
    inline Runtime::NullValue* outputCallable(std::vector<Runtime::Value*> arguments) {
      Runtime::Value* outputArgument = arguments[0];
      std::cout << Runtime::StringValue::getDataOf(outputArgument);
      return new Runtime::NullValue;
    };
    inline Builtins::FunctionBuiltinDeclaration outputDeclaration(outputName, outputCallable, outputArgumentsAmount);
  
    // all declarations
    inline const Builtins::BuiltinModuleDeclarations declarations = {
      &inputDeclaration,
      &outputDeclaration,
    };
  }
}