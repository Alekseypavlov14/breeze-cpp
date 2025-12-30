#include "runtime/memory.h"
#include "runtime/exceptions.h"
#include "shared/classes.h"
#include "shared/sets.h"

#include <iostream>

namespace Runtime {
  Memory::Memory() {
    this->prepareStructuresForModules(0);
  }
  Memory::~Memory() {
    this->removeAllContainers();
    this->removeAllValues();
  }

  void Memory::prepareStructuresForModules(int modulesAmount) {
    this->removeAllContainers();
    this->removeAllValues();

    this->stacks = {};
    this->exports = {};

    this->currentStackIndex = 0;
    this->currentExportsIndex = 0;

    this->containers = {};
    this->values = {};

    this->containerReferenceCount = {};
    this->valuesReferenceCount = {};

    this->permanentContainers = {};
    this->permanentValues = {};

    this->temporaryContainers = {};
    this->temporaryValues = {};

    this->processingValues = {};

    for (int i = 0; i < modulesAmount; i++) {
      this->stacks.push_back(Stack());
      this->exports.push_back(ExportsRegistry());
    } 
  }

  Stack* Memory::getCurrentStack() {
    return this->currentStack;
  }
  ExportsRegistry* Memory::getCurrentExportsRegistry() {
    return this->currentExports;
  }

  void Memory::setCurrentStack(Stack* stack) {
    this->currentStack = stack;
  }
  void Memory::setCurrentExportsRegistry(ExportsRegistry* exports) {
    this->currentExports = exports;
  }

  void Memory::setCurrentStackByIndex(int index) {
    this->currentStack = &this->stacks[index];
    this->currentStackIndex = index;
  }
  void Memory::setCurrentExportsRegistryByIndex(int index) {
    this->currentExports = &this->exports[index];
    this->currentExportsIndex = index;
  }

  int Memory::getCurrentStackIndex() {
    return this->currentStackIndex;
  }
  int Memory::getCurrentExportsRegistryIndex() {
    return this->currentExportsIndex;
  }

  void Memory::retainContainer(Container* container) {
    this->excludeTemporaryContainer(container);
    this->containers.insert(container);
    this->containerReferenceCount[container]++;

    this->retainValue(container->getValue());
  }
  void Memory::releaseContainer(Container* container) {
    // decrement container counter
    int containerCount = --this->containerReferenceCount[container];

    // release used value
    this->releaseValue(container->getValue());

    // clean if nobody refers to this container
    if (containerCount <= 0 && !Shared::Sets::includes(this->permanentContainers, container)) {
      this->removeContainer(container);
    }
  }
  void Memory::removeContainer(Container* removingContainer) {
    std::set<Container*> newContainers = {};

    for (Container* container: this->containers) {
      if (container == removingContainer) {
        // delete reference count
        this->containerReferenceCount.erase(container);
        // release allocated memory
        delete container;

        continue;
      }

      newContainers.insert(container);
    }

    this->containers = newContainers;
  }

  void Memory::retainValue(Value* value) {
    // clear processing values
    this->processingValues = {};

    // search all retaining values
    this->recursivelySearchValues(value);

    // decrement pointer for all releasing values
    for (Value* value: this->processingValues) {
      this->excludeTemporaryValue(value);
      this->values.insert(value);
      this->valuesReferenceCount[value]++;
    }
  }
  void Memory::releaseValue(Value* value) {
    // clear processing values
    this->processingValues = {};

    // search all releasing values
    this->recursivelySearchValues(value);

    // decrement pointer for all releasing values
    for (Value* value: this->processingValues) {
      this->valuesReferenceCount[value]--;
    }
  
    // init new values list
    std::set<Value*> newValues = {};

    // save only values that are still in use
    for (Value* value: this->values) {
      if (this->valuesReferenceCount[value] > 0 && !Shared::Sets::includes(this->permanentValues, value)) {
        newValues.insert(value);
      } else {
        // remove unused value
        this->valuesReferenceCount.erase(value);
        delete value;
      }
    }

    // update values list
    this->values = newValues;
  }

  void Memory::addPermanentContainer(Container* container) {
    this->permanentContainers.insert(container);
    this->addPermanentValue(container->getValue());
  }
  void Memory::addPermanentValue(Value* value) {
    this->permanentValues.insert(value);
  }

  void Memory::addTemporaryContainer(Container* container) {
    this->temporaryContainers.insert(container);
  }
  void Memory::excludeTemporaryContainer(Container* container) {
    this->temporaryContainers.erase(container);
  }
  void Memory::clearTemporaryContainers() {
    for (Container* container: this->temporaryContainers) {
      delete container;
    }
    this->temporaryContainers = {};
  }

  void Memory::addTemporaryValue(Value* value) {
    this->temporaryValues.insert(value);
  }
  void Memory::excludeTemporaryValue(Value* value) {
    this->temporaryValues.erase(value);
  }
  void Memory::clearTemporaryValues() {
    for (Value* value: this->temporaryValues) {
      delete value;
    }
    this->temporaryValues = {};
  }

  void Memory::removeUnreachableValues() {
    // remove processing values
    this->processingValues = {};

    // search all accessible values
    for (Container* container: this->containers) {
      this->recursivelySearchValues(container->getValue());
    }

    // if no memory lost, stop procedure
    if (this->processingValues.size() == this->values.size()) return;

    std::set<Value*> newValues = {};

    // delete all values that are not accessible
    for (Value* value: this->values) {
      if (Shared::Sets::includes(this->processingValues, value)) {
        newValues.insert(value);
      } else {
        // remove value
        this->valuesReferenceCount.erase(value);
        delete value;
      }
    }

    this->values = newValues;
  }
  void Memory::recursivelySearchValues(Value* value) {
    // skip already analyzed value
    if (Shared::Sets::includes(this->processingValues, value)) return;

    // analyze current value
    this->processingValues.insert(value);
    
    // analyze current value children
    if (Shared::Classes::isInstanceOf<Value, PrimitiveValue>(value)) return;
    if (Shared::Classes::isInstanceOf<Value, VectorValue>(value)) {
      std::vector<Value*> items = Shared::Classes::cast<Value, VectorValue>(value)->getItems();
  
      for (int i = 0; i < items.size(); i++) {
        this->recursivelySearchValues(items[i]);
      }
    }
    if (Shared::Classes::isInstanceOf<Value, ObjectValue>(value)) {
      std::vector<Field> fields = Shared::Classes::cast<Value, ObjectValue>(value)->getEntries();
      for (int i = 0; i < fields.size(); i++) {
        this->recursivelySearchValues(fields[i].getKey());
        this->recursivelySearchValues(fields[i].getValue());
      }
    }
    if (Shared::Classes::isInstanceOf<Value, ClassValue>(value)) {
      std::vector<ClassValue*> parents = Shared::Classes::cast<Value, ClassValue>(value)->getParents();
      for (int i = 0; i < parents.size(); i++) {
        this->recursivelySearchValues(parents[i]);
      }

      std::vector<Field> fields = Shared::Classes::cast<Value, ClassValue>(value)->getFields();
      for (int i = 0; i < fields.size(); i++) {
        this->recursivelySearchValues(fields[i].getKey());
        this->recursivelySearchValues(fields[i].getValue());
      }
    }
    if (Shared::Classes::isInstanceOf<Value, FunctionValue>(value)) {
      FunctionValue* functionValue = Shared::Classes::cast<Value, FunctionValue>(value);
      this->recursivelySearchValues(functionValue);

      std::vector<Container*> containers = functionValue->getClosure()->getContainers();
      for (int i = 0; i < containers.size(); i++) {
        this->recursivelySearchValues(containers[i]->getValue());        
      } 
    };
  }

  void Memory::removeAllContainers() {
    for (Container* container: this->containers) {
      delete container;
    }

    this->containers = {};
    this->containerReferenceCount = {};
  }
  void Memory::removeAllValues() {
    for (Value* value: this->values) {
      delete value;
    }

    this->values = {};
    this->valuesReferenceCount = {};
  }
}
