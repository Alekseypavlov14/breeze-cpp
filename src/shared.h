#pragma once

#include <type_traits>
#include <stdexcept>
#include <vector>
#include <string>

// this module contains reusable utils
// no interpreter logic here
namespace Shared {
  // utility that checks instantiation
  template<class Candidate, class Target>
  bool isInstanceOf(Candidate* instance) {
    // map candidate to target => the candidate is derived from target 
    return dynamic_cast<Target*>(instance) != nullptr;
  }

  // checks if vector contains element
  template<class T> 
  bool includes(std::vector<T> list, T element) {
    for (int i = 0; i < list.size(); i++) {
      if (element == list[i]) return true;
    }

    return false;
  }

  std::string slice(std::string, int, int);
}
