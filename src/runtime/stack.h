#pragma once 

#include <string>
#include <vector>

namespace Runtime {
  // forward declaration (from types.h)
  class Value;

  // container describes a variable
  // containers are stored in memory pool
  class Container {
    private:
      bool isConstant;
      std::string name;
      Value* value;

    public:
      Container(std::string, Value*, bool = false);

      bool getIsConstant();
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

      // returns all containers in current scope
      std::vector<Container*> getContainers();

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

      // get all stack containers
      std::vector<Container*> getContainers();

      // returns containers from upper scope
      std::vector<Container*> getContainersFromCurrentScope();

      // returns NULL if no container is found
      Container* getContainerByName(std::string);

      // returns flag if the container is removed successfully
      bool removeContainerByName(std::string);

      int getSize();
  };

  // exports registry
  class ExportsRegistry {
    private:
      std::vector<Container*> containers;
    
    public:
      ExportsRegistry();

      bool addContainer(Container*);
      std::vector<Container*> getContainers();
      Container* getContainerByName(std::string);
  };
}
