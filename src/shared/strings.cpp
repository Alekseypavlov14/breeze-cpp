#include "shared/strings.h"

namespace Shared {
  std::string Strings::slice(std::string content, int start, int end) {
    std::string part = "";
  
    for (int i = start; i < end; i++) {
      part += content[i];
    }

    return part;
  }

  bool Strings::startsWith(std::string content, std::string checker) {
    for (int i = 0; i < checker.size(); i++) {
      if (i >= content.size()) return false;
      if (content[i] != checker[i]) return false;
    }

    return true;
  }
}
