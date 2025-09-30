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

    // clean if nobody refers to this container
    if (!containerCount) {
      this->removeContainer(container);
    }
  }

  void Memory::retainValue(Value* value) {
    this->valuesReferenceCount[value]++;
  }
  void Memory::releaseValue(Value* value) {
    // clear processing values
    this->processingValues = {};

    // search all releasing values
    this->recursivelySearchValues(value);

    // decrement pointer for all releasing values
    for (int i = 0; i < this->processingValues.size(); i++) {
      this->valuesReferenceCount[this->processingValues[i]]--;
    }
  
    // init new values list
    std::vector<Value*> newValues = {};

    // save only values that are still in use
    for (int i = 0; i < this->values.size(); i++) {
      if (this->valuesReferenceCount[this->values[i]] > 0) {
        newValues.push_back(this->values[i]);
      } else {
        // remove unused value
        this->valuesReferenceCount.erase(this->values[i]);
        delete this->values[i];
      }
    }

    // update values list
    this->values = newValues;
  }

  void Memory::removeUnreachableValues() {
    // remove processing values
    this->processingValues = {};

    // search all accessible values
    for (int i = 0; i < this->containers.size(); i++) {
      this->recursivelySearchValues(this->containers[i]->getValue());
    }

    // if no memory lost, stop procedure
    if (this->processingValues.size() == this->values.size()) return;

    std::vector<Value*> newValues = {};

    // delete all values that are not accessible
    for (int i = 0; i < this->values.size(); i++) {
      if (Shared::includes(this->processingValues, this->values[i])) {
        newValues.push_back(this->values[i]);
      } else {
        // remove value
        this->valuesReferenceCount.erase(this->values[i]);
        delete this->values[i];
      }
    }

    this->values = newValues;
  }

  void Memory::recursivelySearchValues(Value* value) {
    // skip already analyzed value
    if (Shared::includes(this->processingValues, value)) return;

    // analyze current value
    this->processingValues.push_back(value);
    
    // analyze current value children
    if (Shared::isInstanceOf<Value, PrimitiveValue>(value)) return;
    if (Shared::isInstanceOf<Value, VectorValue>(value)) {
      std::vector<Value*> items = Shared::cast<Value, VectorValue>(value)->getItems();
  
      for (int i = 0; i < items.size(); i++) {
        this->recursivelySearchValues(items[i]);
      }
    }
    if (Shared::isInstanceOf<Value, ObjectValue>(value)) {
      std::vector<Field> fields = Shared::cast<Value, ObjectValue>(value)->getEntries();
      for (int i = 0; i < fields.size(); i++) {
        this->recursivelySearchValues(fields[i].getKey());
        this->recursivelySearchValues(fields[i].getValue());
      }
    }
    if (Shared::isInstanceOf<Value, ClassValue>(value)) {
      std::vector<ClassValue*> parents = Shared::cast<Value, ClassValue>(value)->getParents();
      for (int i = 0; i < parents.size(); i++) {
        this->recursivelySearchValues(parents[i]);
      }

      std::vector<Field> fields = Shared::cast<Value, ClassValue>(value)->getFields();
      for (int i = 0; i < fields.size(); i++) {
        this->recursivelySearchValues(fields[i].getKey());
        this->recursivelySearchValues(fields[i].getValue());
      }
    }
    if (Shared::isInstanceOf<Value, FunctionValue>(value)) return;
  }

  void Memory::removeContainer(Container* container) {
    std::vector<Container*> newContainers = {};
    
    for (int i = 0; i < this->containers.size(); i++) {
      if (this->containers[i] == container) {
        // delete reference count
        this->containerReferenceCount.erase(container);
        // release allocated memory
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
        // delete reference count
        this->valuesReferenceCount.erase(value);
        // release allocated memory
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
