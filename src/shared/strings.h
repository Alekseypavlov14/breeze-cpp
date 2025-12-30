#pragma once

#include <string>

namespace Shared {
  class Strings {
    public:
      // slices string including start index symbol and excluding end symbol
      static std::string slice(std::string, int, int);

      // checks if a string starts with a certain substring
      static bool startsWith(std::string, std::string);

      // unescape special characters
      static std::string unescape(std::string);
  };
}
