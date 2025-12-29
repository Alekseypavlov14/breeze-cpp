#pragma once 

#include <set>

namespace Shared {
  class Sets {
    public:
      template<class T>
      static bool includes(std::set<T> set, T element) {
        return set.find(element) != set.end();
      }
  };
}
