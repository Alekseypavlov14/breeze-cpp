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

  std::string Strings::unescape(std::string source) {
    std::string result;

    for (int i = 0; i < source.size(); ++i) {
      if (source[i] == '\\' && (i + 1) < source.size()) {
        switch (source[i + 1]) {
          case 'n':  result.push_back('\n'); ++i; break;
          case 't':  result.push_back('\t'); ++i; break;
          case 'r':  result.push_back('\r'); ++i; break;
          case '\\': result.push_back('\\'); ++i; break;
          case '\'': result.push_back('\''); ++i; break;
          case '\"': result.push_back('\"'); ++i; break;
          case '0':  result.push_back('\0'); ++i; break;
          default:   result.push_back(source[i]); break;
        }
      } else {
        result.push_back(source[i]);
      }
    }

    return result;
  }
}
