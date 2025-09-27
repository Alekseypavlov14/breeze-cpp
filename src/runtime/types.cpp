#include "runtime/types.h"
#include "shared/utils.h"

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

  NumberValue::NumberValue(double data) {
    this->data = data;
  }
  NumberValue::NumberValue(const NumberValue& other) {
    this->data = data;
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

  Field::Field(FieldAccess access, FieldType type, FieldMutability mutability, Value* key, Value* value): key(key) {
    this->access = access;
    this->type = type;
    this->mutability = mutability;
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

  ClassValue::ClassValue(
    std::vector<ClassValue*> parents, 
    std::vector<Field> fields,
    FunctionValue constructor,
    FunctionValue destructor
  ): constructor(constructor), destructor(destructor) {
    this->parents = parents;
    this->fields = fields;
    this->constructor = constructor;
    this->destructor;
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

  FunctionValue::FunctionValue(std::function<Value*(std::vector<Value*>)> callable) {
    this->callable = callable;
  }
  DataType FunctionValue::getType() {
    return DataType::Function;
  }
  Value* FunctionValue::execute(std::vector<Value*> values) {
    return this->execute(values);
  }
 
  bool compareValues(Value* value1, Value* value2) {
    // check if it is the same value
    if (value1 == value2) return true;

    // check if both are primitives
    if (Shared::isInstanceOf<Value, PrimitiveValue>(value1) && Shared::isInstanceOf<Value, PrimitiveValue>(value2)) {
      DataType type1 = value1->getType();
      DataType type2 = value2->getType();

      // check if the same type
      if (type1 != type2) return false;
      
      // two nulls are the same
      if (type1 == DataType::Null) return true;
      if (type1 == DataType::Boolean) {
        bool data1 = Shared::cast<Value, BooleanValue>(value1)->getData();
        bool data2 = Shared::cast<Value, BooleanValue>(value2)->getData();
        return data1 == data2;
      }
      if (type1 == DataType::Number) {
        double data1 = Shared::cast<Value, NumberValue>(value1)->getData();
        double data2 = Shared::cast<Value, NumberValue>(value2)->getData();
        return data1 == data2;
      }
      if (type1 == DataType::String) {
        std::string data1 = Shared::cast<Value, StringValue>(value1)->getData();
        std::string data2 = Shared::cast<Value, StringValue>(value2)->getData();
        return data1 == data2;
      }
    }

    // otherwise consider different
    return false;
  }
}
