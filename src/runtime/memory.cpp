#include "runtime/memory.h"
#include "shared/utils.h"

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

    this->currentStack = NULL;
    this->currentExportsRegistry = NULL;

    this->containerReferenceCount = {};
    this->valuesReferenceCount = {};

    for (int i = 0; i < modulesAmount; i++) {
      this->stacks.push_back(Stack());
      this->exports.push_back(ExportsRegistry());
    } 
  }

  Stack* Memory::getCurrentStack() {
    return this->currentStack;
  }
  void Memory::setCurrentStackByIndex(int index) {
    this->currentStack = &this->stacks[index];
  }

  ExportsRegistry* Memory::getCurrentExportsRegistry() {
    return this->currentExportsRegistry;
  }
  void Memory::setCurrentExportsRegistryByIndex(int index) {
    this->currentExportsRegistry = &this->exports[index];
  }

  void Memory::retainContainer(Container* container) {
    this->containerReferenceCount[container]++;
  }
  void Memory::releaseContainer(Container* container) {
    // decrement container counter
    int containerCount = --this->containerReferenceCount[container];

    // release used value
    this->releaseValue(container->getValue());

    // clean if nobody refers this container
    if (!containerCount) {
      this->removeContainer(container);
      this->containerReferenceCount.erase(container);
    }
  }

  void Memory::retainValue(Value* value) {
    this->valuesReferenceCount[value]++;
  }
  void Memory::releaseValue(Value* value) {
    // clear releasing values
    this->releasingValues = {};

    // search all releasing values
    this->recursivelySearchReleasingValues(value);
  
    // init new values list
    std::vector<Value*> newValues = {};

    // save only values that are still in use
    for (int i = 0; i < this->values.size(); i++) {
      if (this->valuesReferenceCount[this->values[i]] > 0) {
        newValues.push_back(this->values[i]);
      }
    }
  }

  void Memory::recursivelySearchReleasingValues(Value* value) {
    // skip already analyzed value
    if (Shared::includes(this->releasingValues, value)) return;

    // analyze current value
    this->releasingValues.push_back(value);
    
    // decrement counter
    this->valuesReferenceCount[value]--;

    // analyze current value children
    if (Shared::isInstanceOf<Value, PrimitiveValue>(value)) return;
    if (Shared::isInstanceOf<Value, VectorValue>(value)) {
      std::vector<Value*> items = Shared::cast<Value, VectorValue>(value)->getItems();
  
      for (int i = 0; i < items.size(); i++) {
        this->recursivelySearchReleasingValues(items[i]);
      }
    }
    if (Shared::isInstanceOf<Value, ObjectValue>(value)) {
      std::vector<Field> fields = Shared::cast<Value, ObjectValue>(value)->getEntries();
      for (int i = 0; i < fields.size(); i++) {
        this->recursivelySearchReleasingValues(fields[i].getKey());
        this->recursivelySearchReleasingValues(fields[i].getValue());
      }
    }
    if (Shared::isInstanceOf<Value, ClassValue>(value)) {
      std::vector<ClassValue*> parents = Shared::cast<Value, ClassValue>(value)->getParents();
      for (int i = 0; i < parents.size(); i++) {
        this->recursivelySearchReleasingValues(parents[i]);
      }

      std::vector<Field> fields = Shared::cast<Value, ClassValue>(value)->getFields();
      for (int i = 0; i < fields.size(); i++) {
        this->recursivelySearchReleasingValues(fields[i].getKey());
        this->recursivelySearchReleasingValues(fields[i].getValue());
      }
    }
    if (Shared::isInstanceOf<Value, FunctionValue>(value)) return;
  }

  void Memory::removeContainer(Container* container) {
    std::vector<Container*> newContainers = {};
    
    for (int i = 0; i < this->containers.size(); i++) {
      if (this->containers[i] == container) {
        this->containerReferenceCount.erase(container);
        delete container;

        continue;
      }

      newContainers.push_back(this->containers[i]);
    }

    this->containers = newContainers;
  }
  void Memory::removeValue(Value* value) {
    std::vector<Value*> newValues = {};

    for (int i = 0; i < this->values.size(); i++) {
      if (this->values[i] == value) {
        this->valuesReferenceCount.erase(value);
        delete value;

        continue;
      }

      newValues.push_back(this->values[i]);
    }

    this->values = newValues;
  }

  void Memory::removeAllContainers() {
    for (int i = 0; i < this->containers.size(); i++) {
      delete this->containers[i];
    }

    this->containers = {};
    this->containerReferenceCount = {};
  }
  void Memory::removeAllValues() {
    for (int i = 0; i < this->values.size(); i++) {
      delete this->values[i];
    }

    this->values = {};
    this->valuesReferenceCount = {};
  }
}
