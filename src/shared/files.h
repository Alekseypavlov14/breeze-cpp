#pragma once

#include <fstream>
#include <string>

namespace Shared {
  class Files {
    public:
      static std::string readFileByAbsolutePath(std::string);
  };
}
