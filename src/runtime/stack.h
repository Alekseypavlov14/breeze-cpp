#pragma once 

#include <string>
#include <vector>

#include "runtime/types.h"

namespace Runtime {
  // container describes a variable
  // containers are stored in memory pool
  class Container {
    private:
      std::string name;
      Value* value;

    public:
      Container(std::string, Value*);

      std::string getName();
      Value* getValue();
      void setValue(Value*);
  };

  // describes execution scope
  // scopes are stored in stacks and are copied by value
  class Scope {
    private:
      std::vector<Container*> containers;

    public:
      Scope();

      // returns flag if the container is added successfully
      bool addContainer(Container*);

      bool isContainerAdded(std::string);

      // returns NULL if no container is found
      Container* getContainerByName(std::string);
 
      // returns flag if the container is removed successfully
      bool removeContainerByName(std::string);
  };

  // describes stack for executing module
  class Stack {
    private: 
      std::vector<Scope> scopes;

    public:
      Stack();
      Stack(const Stack&);

      void addScope(Scope);
      void removeScope();
      
      // returns flag if the container is added successfully
      bool addContainer(Container*);

      // returns NULL if no container is found
      Container* getContainerByName(std::string);

      // returns flag if the container is removed successfully
      bool removeContainerByName(std::string);
  };
}
