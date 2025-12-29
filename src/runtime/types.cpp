#include "runtime/stack.h"
#include "runtime/types.h"
#include "shared/classes.h"

namespace Runtime {
  NullValue::NullValue() {}
  DataType NullValue::getType() {
    return DataType::Null;
  }

  BooleanValue::BooleanValue(bool data) {
    this->data = data;
  }
  BooleanValue::BooleanValue(const BooleanValue& other) {
    this->data = other.data;
  }
  DataType BooleanValue::getType() {
    return DataType::Boolean;
  }
  bool BooleanValue::getData() {
    return this->data;
  }
  void BooleanValue::setData(bool data) {
    this->data = data;
  }
  bool BooleanValue::getDataOf(Value* value) {
    return Shared::Classes::cast<Value, BooleanValue>(value)->getData();
  }

  NumberValue::NumberValue(double data) {
    this->data = data;
  }
  NumberValue::NumberValue(const NumberValue& other) {
    this->data = other.data;
  }
  DataType NumberValue::getType() {
    return DataType::Number;
  }
  double NumberValue::getData() {
    return this->data;
  }
  void NumberValue::setData(double data) {
    this->data = data;
  }
  double NumberValue::getDataOf(Value* value) {
    return Shared::Classes::cast<Value, NumberValue>(value)->getData();
  }

  StringValue::StringValue(std::string data) {
    this->data = data;
  }
  StringValue::StringValue(const StringValue& other) {
    this->data = other.data;
  }
  DataType StringValue::getType() {
    return DataType::String;
  }
  std::string StringValue::getData() {
    return this->data;
  }
  void StringValue::setData(std::string data) {
    this->data = data;
  }
  std::string StringValue::getDataOf(Value* value) {
    return Shared::Classes::cast<Value, StringValue>(value)->getData();
  }

  VectorValue::VectorValue(std::vector<Value*> items) {
    this->items = items;
  }
  DataType VectorValue::getType() {
    return DataType::Vector;
  }
  std::vector<Value*> VectorValue::getItems() {
    return this->items;
  }
  void VectorValue::setItem(int index, Value* value) {
    this->items[index] = value;
  }
  void VectorValue::push(Value* value) {
    this->items.push_back(value);
  }
  Value* VectorValue::pop() {
    Value* last = this->items[this->items.size() - 1];
    this->items.pop_back();
    return last;
  }

  Field::Field(FieldAccess access, FieldType type, FieldMutability mutability, ClassValue* classOwner, Value* objectOwner, Value* key, Value* value): key(key) {
    this->access = access;
    this->type = type;
    this->mutability = mutability;
    this->classOwner = classOwner;
    this->objectOwner = objectOwner;
    this->key = key;
    this->value = value;
  }
  FieldAccess Field::getAccess() {
    return this->access;
  }
  FieldType Field::getType() {
    return this->type;
  }
  FieldMutability Field::getMutability() {
    return this->mutability;
  }
  ClassValue* Field::getClassOwner() {
    return this->classOwner;
  }
  Value* Field::getObjectOwner() {
    return this->objectOwner;
  }
  Value* Field::getKey() {
    return this->key;
  }
  Value* Field::getValue() {
    return this->value;
  }
  void Field::setValue(Value* value) {
    this->value = value;
  }

  ObjectValue::ObjectValue(ClassValue* constructor, std::vector<Field> entries) {
    this->constructor = constructor;
    this->entries = entries;
  }
  DataType ObjectValue::getType() {
    return DataType::Object;
  }
  ClassValue* ObjectValue::getConstructor() {
    return this->constructor;
  }
  std::vector<Field> ObjectValue::getEntries() {
    return this->entries;
  }
  Value* ObjectValue::getEntryValue(Value* key) {
    for (int i = 0; i < this->entries.size(); i++) {
      if (compareValues(key, this->entries[i].getKey())) {
        return this->entries[i].getValue();
      }
    }

    return NULL;
  }
  bool ObjectValue::addField(Field field) {
    if (this->hasEntry(field.getKey())) return false;
    this->entries.push_back(field);
    return true;
  }
  bool ObjectValue::setEntry(Value* key, Value* value) {
    for (int i = 0; i < this->entries.size(); i++) {
      if (compareValues(key, this->entries[i].getKey())) {
        this->entries[i].setValue(value);
        return true;
      }
    }

    return false;
  }
  bool ObjectValue::hasEntry(Value* key) {
    for (int i = 0; i < this->entries.size(); i++) {
      if (compareValues(key, this->entries[i].getKey())) {
        return true;
      }
    }

    return false;
  }

  ClassValue::ClassValue(std::vector<ClassValue*> parents, std::vector<Field> fields, FunctionValue* constructor, FunctionValue* destructor): constructor(constructor), destructor(destructor) {
    this->parents = parents;
    this->fields = fields;
    this->constructor = constructor;
    this->destructor = destructor;
  }
  DataType ClassValue::getType() {
    return DataType::Class;
  }
  std::vector<ClassValue*> ClassValue::getParents() {
    return this->parents;
  }  
  std::vector<Field> ClassValue::getFields() {
    return this->fields;
  }

  FunctionArgumentsAmount::FunctionArgumentsAmount(int argumentsAmount, int optionalArguments) {
    this->argumentsAmount = argumentsAmount;
    this->optionalArguments = optionalArguments;
  }
  int FunctionArgumentsAmount::getTotalArgumentsAmount() {
    return this->argumentsAmount;
  }
  int FunctionArgumentsAmount::getRequiredArgumentsAmount() {
    return this->argumentsAmount - this->optionalArguments;
  }
  int FunctionArgumentsAmount::getOptionalArgumentsAmount() {
    return this->optionalArguments;
  }

  FunctionValue::FunctionValue(Stack stack, Value* context, Callable callable, FunctionArgumentsAmount arguments) {
    this->closure = stack;
    this->context = context;
    this->callable = callable;
    this->argumentsAmount = arguments;
  }
  DataType FunctionValue::getType() {
    return DataType::Function;
  }
  Stack FunctionValue::getClosure() {
    return this->closure;
  }
  Value* FunctionValue::getContext() {
    return this->context;
  }
  Value* FunctionValue::execute(std::vector<Value*> values) {
    return this->callable(values);
  }
  FunctionArgumentsAmount FunctionValue::getArgumentsAmount() {
    return this->argumentsAmount;
  }
 
  bool compareValues(Value* value1, Value* value2) {
    // check if it is the same value
    if (value1 == value2) return true;
    
    // otherwise if one of values is not defined 
    if (!value1 || !value2) return false;

    // check if both are primitives
    if (Shared::Classes::isInstanceOf<Value, PrimitiveValue>(value1) && Shared::Classes::isInstanceOf<Value, PrimitiveValue>(value2)) {
      DataType type1 = value1->getType();
      DataType type2 = value2->getType();

      // check if the same type
      if (type1 != type2) return false;
      
      // two nulls are the same
      if (type1 == DataType::Null) return true;
      if (type1 == DataType::Boolean) {
        bool data1 = BooleanValue::getDataOf(value1);
        bool data2 = BooleanValue::getDataOf(value2);
        return data1 == data2;
      }
      if (type1 == DataType::Number) {
        double data1 = NumberValue::getDataOf(value1);
        double data2 = NumberValue::getDataOf(value2);
        return data1 == data2;
      }
      if (type1 == DataType::String) {
        std::string data1 = StringValue::getDataOf(value1);
        std::string data2 = StringValue::getDataOf(value2);
        return data1 == data2;
      }
    }

    // otherwise consider different
    return false;
  }

  bool getBoolean(Value* value) {
    if (Shared::Classes::isInstanceOf<Value, NullValue>(value)) return false;
    if (Shared::Classes::isInstanceOf<Value, BooleanValue>(value)) return BooleanValue::getDataOf(value);
    if (Shared::Classes::isInstanceOf<Value, NumberValue>(value)) return NumberValue::getDataOf(value) != NUMBER_DEFAULT_VALUE;
    if (Shared::Classes::isInstanceOf<Value, StringValue>(value)) return StringValue::getDataOf(value) != STRING_DEFAULT_VALUE;
    if (Shared::Classes::isInstanceOf<Value, CompoundValue>(value)) return true;
    return false;
  }

  bool isInstanceOf(Value* superItem, Value* validatingItem) {
    if (compareValues(superItem, validatingItem)) return true;

    if (Shared::Classes::isInstanceOf<Value, ClassValue>(validatingItem)) {
      ClassValue* casted = Shared::Classes::cast<Value, ClassValue>(validatingItem);

      for (int i = 0; i < casted->getParents().size(); i++) {
        if (isInstanceOf(superItem, casted->getParents()[i])) {
          return true;
        }
      }
    }

    if (Shared::Classes::isInstanceOf<Value, ObjectValue>(validatingItem)) {
      ObjectValue* casted = Shared::Classes::cast<Value, ObjectValue>(validatingItem);
      return isInstanceOf(superItem, casted->getConstructor());
    }

    return false;
  }
}
