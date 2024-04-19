#pragma once
#include <sstream>

#include "check.hpp"
#include "sptr.hpp"
enum class TypeType {
  UNKOWN_TYPE,
  INT_TYPE,
  BOOL_TYPE,
  PROC_TYPE,
  NAMED_TYPE,
};

struct Type {
  TypeType type_;
  virtual ~Type() {}
};

struct IntType : public Type {
  IntType() { type_ = TypeType::INT_TYPE; }
};
SPtr<Type> make_int_type() {
  IntType *type = new IntType{};
  return SPtr<Type>{(Type *)type};
}
struct BoolType : public Type {
  BoolType() { type_ = TypeType::BOOL_TYPE; }
};
SPtr<Type> make_bool_type() {
  BoolType *type = new BoolType{};
  return SPtr<Type>{(Type *)type};
}
struct ProcType : public Type {
  ProcType() { type_ = TypeType::PROC_TYPE; }
  SPtr<Type> parameter_type_;
  SPtr<Type> result_type_;
};
SPtr<Type> make_proc_type(const SPtr<Type> &parameter_type,
                          const SPtr<Type> &result_type) {
  ProcType *type = new ProcType{};
  type->parameter_type_ = parameter_type;
  type->result_type_ = result_type;
  return SPtr<Type>{(Type *)type};
}
struct NamedType : public Type {
  NamedType() { type_ = TypeType::NAMED_TYPE; }
  std::string name_;
};
SPtr<Type> make_name_type(const std::string &name) {
  NamedType *type = new NamedType{};
  type->name_ = name;
  return SPtr<Type>{(Type *)type};
}
bool is_type_equal(const SPtr<Type> &type_0, const SPtr<Type> &type_1) {
  if (type_0->type_ != type_1->type_) {
    return false;
  }
  if (type_0->type_ == TypeType::PROC_TYPE) {
    auto type_0_ptr = (ProcType *)type_0.get();
    auto type_1_ptr = (ProcType *)type_1.get();
    return is_type_equal(type_0_ptr->parameter_type_,
                         type_1_ptr->parameter_type_) &&
           is_type_equal(type_0_ptr->result_type_, type_1_ptr->result_type_);
  }
  return true;
}
std::string to_string(const SPtr<Type> &type) {
  std::stringstream ss;
  if (type->type_ == TypeType::INT_TYPE) {
    ss << "INT";
    return ss.str();
  }
  if (type->type_ == TypeType::BOOL_TYPE) {
    ss << "BOOL";
    return ss.str();
  }
  if (type->type_ == TypeType::NAMED_TYPE) {
    auto named_type = (NamedType *)type.get();
    ss << "TYPE(\"" << named_type->name_ << "\")";
    return ss.str();
  }
  if (type->type_ == TypeType::PROC_TYPE) {
    auto proc_type = (ProcType *)type.get();
    ss << "PROC:(" << to_string(proc_type->parameter_type_) << ")->("
       << to_string(proc_type->result_type_) << ")";
    return ss.str();
  }
  CHECK_WITH_INFO(false,
                  "got unkown type!!!!" + std::to_string(int(type->type_)));
  return "";
};