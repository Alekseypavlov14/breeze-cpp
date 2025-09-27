#include "runtime/stack.h"

namespace Runtime {
  Container::Container(std::string name, Value* value) {
    this->name = name;
    this->value = value;
  }
  std::string Container::getName() {
    return this->name;
  }
  Value* Container::getValue() {
    return this->value;
  }
  void Container::setValue(Value* value) {
    this->value = value;
  }

  Scope::Scope() {
    this->containers = {};
  }
  bool Scope::addContainer(Container* container) {
    if (this->isContainerAdded(container->getName())) return false;

    this->containers.push_back(container);
    return true;
  }
  bool Scope::isContainerAdded(std::string name) {
    for (int i = 0; i < this->containers.size(); i++) {
      if (this->containers[i]->getName() == name) {
        return true;
      }
    }

    return false;
  }
  Container* Scope::getContainerByName(std::string name) {
    for (int i = 0; i < this->containers.size(); i++) {
      if (this->containers[i]->getName() == name) {
        return this->containers[i];
      }
    }

    return NULL;
  }
  bool Scope::removeContainerByName(std::string name) {
    std::vector<Container*> updatedContainers = {};
    bool isDeletingContainerFound = false;

    for (int i = 0; i < this->containers.size(); i++) {
      if (this->containers[i]->getName() == name) {
        isDeletingContainerFound = true;
      } else {
        updatedContainers.push_back(this->containers[i]);
      }
    }

    this->containers = updatedContainers;
    return isDeletingContainerFound;
  }

  Stack::Stack() {
    this->scopes = {};
  }
  Stack::Stack(const Stack& stack) {
    this->scopes = stack.scopes;
  }
  void Stack::addScope(Scope scope) {
    this->scopes.push_back(scope);
  }
  void Stack::removeScope() {
    this->scopes.pop_back();
  }
  bool Stack::addContainer(Container* container) {
    if (this->scopes.size() == 0) return false;
    return this->scopes[this->scopes.size() - 1].addContainer(container);
  }
  Container* Stack::getContainerByName(std::string name) {
    for (int i = this->scopes.size() - 1; i >= 0; i--) {
      Container* container = this->scopes[i].getContainerByName(name);
      if (container != NULL) return container;
    }

    return NULL;
  }
  bool Stack::removeContainerByName(std::string name) {
    return this->scopes[this->scopes.size() - 1].removeContainerByName(name);
  }
}
