#pragma once
#include <sstream>

#include "check.hpp"
#include "sptr.hpp"
enum class TypeType {
  UNKOWN_TYPE,
  INT_TYPE,
  BOOL_TYPE,
  PROC_TYPE,
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