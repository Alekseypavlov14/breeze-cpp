#include "shared/files.h"

namespace Shared {
  std::string Files::readFileByAbsolutePath(std::string absolutePath) {
    std::ifstream file(absolutePath);
    if (!file.is_open()) return "";

    std::string content, line;
    while (std::getline(file, line)) {
      content += line + "\n";
    }
    
    return content;
  }
}
