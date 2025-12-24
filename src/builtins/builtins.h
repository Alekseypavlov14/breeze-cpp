#include "builtins/declarations.h"

#include "builtins/modules/console.h"
#include "builtins/modules/types.h"

namespace Builtins {
  inline const std::vector<BuiltinModuleDeclarations> declarations = {
    Console::declarations,
    Types::declarations,
  };
}
