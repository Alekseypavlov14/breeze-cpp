#include "shared/utils.h"

namespace Shared {
  // slices string
  std::string slice(std::string str, int start, int end) {
    std::string part = "";

    for (int i = start; i < end; i++) {
      part += str[i];
    }

    return part;
  }
}
