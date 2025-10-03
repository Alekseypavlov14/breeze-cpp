#include <fstream>

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
  // check if string starts with given sequence
  bool startsWith(std::string string, std::string start) {
    for (int i = 0; i < start.size(); i++) {
      if (i >= string.size()) return false;
      if (string[i] != start[i]) return false;
    }

    return true;
  }

  // file utils
  std::string readFileByAbsolutePath(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";

    std::string content, line;
    while (std::getline(file, line)) {
      content += line + "\n";
    }
    
    return content;
  }
}
