#pragma once 

#include <vector>

namespace Shared {
  class Vectors {
    public:
      template<class T> 
      static bool includes(std::vector<T> list, T element) {
        for (int i = 0; i < list.size(); i++) {
          if (element == list[i]) return true;
        }

        return false;
      }

      template<class T>
      static std::vector<T> removeAll(std::vector<T> list, T element) {
        if (!includes(list, element)) return list;
        
        std::vector<T> newList = {};

        for (int i = 0; i < list.size(); i++) {
          if (list[i] != element) {
            newList.push_back(list[i]);
          }
        }

        return newList;
      }
  };
}
