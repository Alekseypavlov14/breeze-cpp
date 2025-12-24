#pragma once

#include <vector>

#include "runtime/stack.h"
#include "runtime/types.h"

namespace Runtime {
  // is responsible for memory usage and deallocation
  class Memory {
    private:
      // keeps track of module stacks during execution
      // stacks and scopes are pointing to containers
      std::vector<Stack> stacks;
      // keeps track of modules exports
      // exports contains containers from stacks and scopes
      std::vector<ExportsRegistry> exports;
      
      // pointers to current operating entities
      int currentStackIndex;
      int currentExportsIndex;
      
      // keeps track of all used variables (containers)
      // containers only point to values, flat structure
      // flat structure -> retain/release is enough
      std::vector<Container*> containers;
      // keeps track of all used values (containers point to value)
      // values can point to other values, cyclic structure
      // cyclic structure -> has to be released recursively and checked for accessibility
      std::vector<Value*> values;
      
      // count allocated memory references
      // used to release memory when nobody uses it
      std::unordered_map<Container*, int> containerReferenceCount;
      std::unordered_map<Value*, int> valuesReferenceCount;
      
      // holds list of pointers to temporary containers
      // temporary containers are used for expressions
      std::vector<Container*> temporaryContainers;
      // holds list of pointers to temporary values 
      // temporary values are used for expression computations
      std::vector<Value*> temporaryValues;
      
      // utility that optimizes value operations
      // searches value and its children and adds them to this.processingValues
      void recursivelySearchValues(Value*);
      // used during operations to prevent loops
      // particularly used for recursive values releasing
      std::vector<Value*> processingValues;
      
      // deletes all containers and counts
      void removeAllContainers();
      // deletes all values and counts
      void removeAllValues();

    public:
      Memory();
      ~Memory();
      
      // creates stacks, exports registries for each module
      // is called before module graph executed
      void prepareStructuresForModules(int modulesAmount);
      void loadBuiltinContainers(std::vector<Container*>);
      
      void setCurrentStackByIndex(int);
      void setCurrentExportsRegistryByIndex(int);

      int getCurrentStackIndex();
      int getCurrentExportsRegistryByIndex();

      // stack usage
      void addScopeToStack();
      void removeScopeFromStack();
      // add container to current stack
      bool addContainerToStack(Container* container);
      // get container by name
      Container* getContainerFromStack(std::string);
      // remove container from current stack
      bool removeContainerFromStack(std::string);
      // clone current stack to create closure
      Stack cloneCurrentStack();
      // current stack size
      int getCurrentStackSize();
      
      // exports usage
      bool addContainerToExports(Container* container);
      Container* getContainerFromExports(std::string);
      
      // control containers usage
      // increment pointer reference count
      void retainContainer(Container* container);
      // decrement pointer reference count and deletes container if nobody uses it
      void releaseContainer(Container* container);
      // deletes container by pointer from containers list
      void removeContainer(Container*);

      // control values usage
      // increment pointer reference count
      void retainValue(Value* value);
      // decrement pointer reference count and remove unused values recursively
      void releaseValue(Value* value);
      
      void addTemporaryContainer(Container*);
      void excludeTemporaryContainer(Container*);
      void clearTemporaryContainers();

      void addTemporaryValue(Value*);
      void excludeTemporaryValue(Value*);
      void clearTemporaryValues();
      
      // clears values that are not accessible through any container
      // removes cycled references 
      void removeUnreachableValues();
  };
}
