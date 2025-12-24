#include "runtime/memory.h"
#include "runtime/exceptions.h"
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

    this->currentStackIndex = 0;
    this->currentExportsIndex = 0;

    this->containers = {};
    this->values = {};

    this->containerReferenceCount = {};
    this->valuesReferenceCount = {};

    this->temporaryContainers = {};
    this->temporaryValues = {};

    this->processingValues = {};

    for (int i = 0; i < modulesAmount; i++) {
      this->stacks.push_back(Stack());
      this->exports.push_back(ExportsRegistry());
    } 
  }
  void Memory::loadBuiltinContainers(std::vector<Container*> containers) {
    for (int i = 0; i < this->stacks.size(); i++) {
      this->setCurrentStackByIndex(i);

      for (int j = 0; j < containers.size(); j++) {
        this->addContainerToStack(containers[i]);
      }
    }
  }

  void Memory::setCurrentStackByIndex(int index) {
    this->currentStackIndex = index;
  }
  void Memory::setCurrentExportsRegistryByIndex(int index) {
    this->currentExportsIndex = index;
  }

  int Memory::getCurrentStackIndex() {
    return this->currentStackIndex;
  }
  int Memory::getCurrentExportsRegistryByIndex() {
    return this->currentExportsIndex;
  }

  void Memory::addScopeToStack() {
    if (this->currentStackIndex >= this->stacks.size()) {
      throw Runtime::Exception("No stack available by this index");
    } 

    this->stacks[this->currentStackIndex].addScope(Scope());
  }
  void Memory::removeScopeFromStack() {
    if (this->currentStackIndex >= this->stacks.size()) {
      throw Runtime::Exception("No stack available by this index");
    } 

    this->stacks[this->currentStackIndex].removeScope();
  }
  bool Memory::addContainerToStack(Container* container) {
    if (this->currentStackIndex >= this->stacks.size()) {
      throw Runtime::Exception("No stack available by this index");
    } 

    return this->stacks[this->currentStackIndex].addContainer(container);
  }
  Container* Memory::getContainerFromStack(std::string name) {
    if (this->currentStackIndex >= this->stacks.size()) {
      throw Runtime::Exception("No stack available by this index");
    } 

    return this->stacks[this->currentStackIndex].getContainerByName(name);
  }
  bool Memory::removeContainerFromStack(std::string name) {
    if (this->currentStackIndex >= this->stacks.size()) {
      throw Runtime::Exception("No stack available by this index");
    } 

    return this->stacks[this->currentStackIndex].removeContainerByName(name);
  }
  Stack Memory::cloneCurrentStack() {
    return this->stacks[this->currentStackIndex];
  }
  int Memory::getCurrentStackSize() {
    if (this->currentStackIndex >= this->exports.size()) {
      throw Runtime::Exception("No exports available by this index");
    }

    return this->stacks[this->currentStackIndex].getSize();
  }

  bool Memory::addContainerToExports(Container* container) {
    if (this->currentExportsIndex >= this->exports.size()) {
      throw Runtime::Exception("No exports available by this index");
    }

    return this->exports[this->currentExportsIndex].addContainer(container);
  }
  Container* Memory::getContainerFromExports(std::string name) {
    if (this->currentExportsIndex >= this->exports.size()) {
      throw Runtime::Exception("No exports available by this index");
    }

    return this->exports[this->currentExportsIndex].getContainerByName(name);
  }

  void Memory::retainContainer(Container* container) {
    this->containerReferenceCount[container]++;
    this->retainValue(container->getValue());
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

  void Memory::retainValue(Value* value) {
    // clear processing values
    this->processingValues = {};

    // search all retaining values
    this->recursivelySearchValues(value);

    // decrement pointer for all releasing values
    for (int i = 0; i < this->processingValues.size(); i++) {
      this->valuesReferenceCount[this->processingValues[i]]++;
    }
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

  void Memory::addTemporaryContainer(Container* container) {
    this->temporaryContainers.push_back(container);
  }
  void Memory::excludeTemporaryContainer(Container* container) {
    Shared::removeAll(this->containers, container);
  }
  void Memory::clearTemporaryContainers() {
    for (int i = 0; i < this->temporaryContainers.size(); i++) {
      delete this->temporaryContainers[i];
    }
    this->temporaryContainers = {};
  }

  void Memory::addTemporaryValue(Value* value) {
    this->temporaryValues.push_back(value);
  }
  void Memory::excludeTemporaryValue(Value* value) {
    Shared::removeAll(this->temporaryValues, value);
  }
  void Memory::clearTemporaryValues() {
    for (int i = 0; i < this->temporaryValues.size(); i++) {
      delete this->temporaryValues[i];
    }
    this->temporaryValues = {};
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
