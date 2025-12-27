#pragma once

#include "runtime/stack.h"

#include <string>
#include <vector>
#include <functional>

// defines how language stores data in runtime
// adds typing and classes
// memory is freed by the stack and scope
namespace Runtime {
  // define all possible data types
  enum class DataType {
    Null,
    Boolean,
    Number,
    String,

    Vector,
    Object,
    
    Function,
    Class,
  };

  // default values
  inline const bool BOOLEAN_DEFAULT_VALUE = false;
  inline const double NUMBER_DEFAULT_VALUE = 0;
  inline const std::string STRING_DEFAULT_VALUE = "";

  // root for value hierarchy
  class Value {
    public:
      // makes hierarchy polymorphic
      virtual ~Value() = default;

      virtual DataType getType() = 0;
    };

  // define primitive types
  // wraps primitive data and is constant value
  // primitive data types are copied-by-value (cloned)
  class PrimitiveValue: public Value {};
  
  class NullValue: public PrimitiveValue {
    public:
      NullValue();

      DataType getType();
  };
  class BooleanValue: public PrimitiveValue {
    private:
      bool data;

    public:
      BooleanValue(bool);
      BooleanValue(const BooleanValue&);

      DataType getType();

      bool getData();
      void setData(bool);

      static bool getDataOf(Value*);
  };
  class NumberValue: public PrimitiveValue {
    private:
      double data;

    public:
      NumberValue(double);
      NumberValue(const NumberValue&);

      DataType getType();

      double getData();
      void setData(double);

      static double getDataOf(Value*);
  };
  class StringValue: public PrimitiveValue {
    private:
      std::string data;

    public:
      StringValue(std::string data);
      StringValue(const StringValue&);

      DataType getType();

      std::string getData();
      void setData(std::string);

      static std::string getDataOf(Value*);
  };

  // define compound types
  // interpreter code has to validate operations
  // compound data types are copied by reference
  class CompoundValue: public Value {};

  class VectorValue;
  class ObjectValue;
  class ClassValue;
  class FunctionValue;

  // defines sequence of values
  class VectorValue: public CompoundValue {
    private:
      std::vector<Value*> items;

    public:
      VectorValue(std::vector<Value*> items);

      DataType getType();

      // fundamental methods
      std::vector<Value*> getItems();
      void setItem(int, Value*);

      void push(Value*);
      Value* pop();
  };

  // define OOP classes
  // used for classes and objects
  enum class FieldAccess { 
    PUBLIC, 
    PROTECTED, 
    PRIVATE 
  };
  enum class FieldType {
    INSTANCE,
    STATIC,
  };
  enum class FieldMutability {
    VARIABLE,
    CONSTANT,
  };

  class Field {
    private:
      FieldAccess access;
      FieldType type;
      FieldMutability mutability;

      Value* key;
      Value* value;

    public: 
      Field(FieldAccess access, FieldType type, FieldMutability mutability, Value* key, Value* value);
      
      FieldAccess getAccess();
      FieldType getType();
      FieldMutability getMutability();

      Value* getKey();

      Value* getValue();
      void setValue(Value*);
  };

  // defines associations or maps
  // objects have public instance fields
  class ObjectValue: public CompoundValue {
    private:
      // contains NULL for plain objects
      ClassValue* constructor;
      std::vector<Field> entries;

    public:
      ObjectValue(ClassValue* constructor, std::vector<Field> entries);

      DataType getType();

      ClassValue* getConstructor();

      std::vector<Field> getEntries();
      Value* getEntryValue(Value*);

      // return if the field is added
      bool addField(Field);

      // return if the entry is set
      bool setEntry(Value*, Value*);
      
      bool hasEntry(Value*);
  };

  class ClassValue: public CompoundValue {
    private:
      // for inheritance
      // multiple inheritance is allowed
      std::vector<ClassValue*> parents;
      // list of data members
      std::vector<Field> fields;

      // constructor and destructor 
      FunctionValue* constructor;
      FunctionValue* destructor;

    public:
      ClassValue(
        std::vector<ClassValue*> parents, 
        std::vector<Field> fields,
        FunctionValue* constructor,
        FunctionValue* destructor
      );

      DataType getType();

      std::vector<ClassValue*> getParents();      
      std::vector<Field> getFields();
  };

  // define function value
  // define function callable type
  using Callable = std::function<Value*(std::vector<Value*>)>;

  // function arguments controller
  class FunctionArgumentsAmount {
    private:
      int argumentsAmount;
      int optionalArguments;

    public:
      FunctionArgumentsAmount(int argumentsAmount = 0, int optionalArguments = 0);

      int getTotalArgumentsAmount();
      int getRequiredArgumentsAmount();
      int getOptionalArgumentsAmount();
  };

  // function value
  class FunctionValue: public CompoundValue {
    private:
      // copy of the stack at the moment of declaration
      Stack closure;
      Callable callable;
      FunctionArgumentsAmount argumentsAmount;

    public:
      FunctionValue(Stack, Callable, FunctionArgumentsAmount);

      DataType getType();

      Stack getClosure();
      Value* execute(std::vector<Value*>); 
      FunctionArgumentsAmount getArgumentsAmount();
  };

  // fundamental utilities

  // compares two values 
  // primitive values are compared by value and by reference
  // compound values are compared only by reference and considered non-equal if references are different
  bool compareValues(Value*, Value*);

  // evaluate value a bool
  bool getBoolean(Value*);
} 
