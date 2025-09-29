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

      // keeps track of all used variables (containers)
      // containers only point to values, flat structure
      // flat structure -> retain/release is enough
      std::vector<Container*> containers;
      // keeps track of all used values (containers point to value)
      // values can point to other values, cyclic structure
      // cyclic structure -> has to be released recursively and checked for accessibility
      std::vector<Value*> values;

      // pointers to current operating entity
      Stack* currentStack;
      ExportsRegistry* currentExportsRegistry;

      // count allocated memory references
      // used to release memory when nobody uses it
      std::unordered_map<Container*, int> containerReferenceCount;
      std::unordered_map<Value*, int> valuesReferenceCount;

      // used during operations to prevent loops
      // particularly used for recursive values releasing
      std::vector<Value*> processingValues;

      // utility that optimizes value operations
      // searches value and its children and adds them to this.processingValues
      void recursivelySearchValues(Value*);

    public:
      Memory();
      ~Memory();

      // creates stacks, exports registries for each module
      // is called before module graph executed
      void prepareStructuresForModules(int modulesAmount);

      Stack* getCurrentStack();
      void setCurrentStackByIndex(int);

      ExportsRegistry* getCurrentExportsRegistry();
      void setCurrentExportsRegistryByIndex(int);

      // control containers usage
      // increment and decrement pointer reference count and remove unused containers
      void retainContainer(Container* container);
      void releaseContainer(Container* container);

      // control values usage
      // increment and decrement pointer reference count and remove unused values
      // release is recursive
      void retainValue(Value* value);
      void releaseValue(Value* value);

      // clears values that are not accessible through any container
      // removes cycled references 
      void removeUnreachableValues();

      // clear specific container or value
      // delete pointer and its reference count
      void removeContainer(Container*);
      void removeValue(Value*);
      
      // clears all allocated objects
      void removeAllContainers();
      void removeAllValues();
  };
}
