#pragma once
#include <functional>
#include <sstream>
#include <unordered_map>

#include "expression.hpp"
#include "sptr.hpp"
enum class ValueType {
  UNKOWN,
  INT,
  BOOL,
  PROC,
  REC_PROC,
  MUTEX
};
struct Value {
  ValueType type_;
  virtual ~Value(){}
};
using ToStringFuncMapType =
    std::unordered_map<ValueType,
                       std::function<std::string(const SPtr<Value> &)>>;
ToStringFuncMapType &get_to_string_func_global_map() {
  static std::unordered_map<ValueType,
                            std::function<std::string(const SPtr<Value> &)>>
      maps_;
  return maps_;
}
// Value
#define DEFINE_VALUE(CLASS_NAME, ENUM_NAME, TYPE_NAME)                      \
  struct CLASS_NAME : public Value {                                        \
    CLASS_NAME() { type_ = ValueType::ENUM_NAME; }                          \
    using InternalType = TYPE_NAME;                                         \
    TYPE_NAME v_;                                                           \
  };                                                                        \
  SPtr<Value> to_value(TYPE_NAME v) {                                       \
    CLASS_NAME *t_value = new CLASS_NAME{};                                 \
    t_value->v_ = v;                                                        \
    return SPtr<Value>{(Value *)t_value};                                   \
  }                                                                         \
  struct CLASS_NAME##ENUM_NAME##TYPE_NAME##to_string_registeration {        \
    CLASS_NAME##ENUM_NAME##TYPE_NAME##to_string_registeration() {           \
      auto &to_string_func_map = get_to_string_func_global_map();           \
      to_string_func_map[ValueType::ENUM_NAME] = [](const SPtr<Value> &v) { \
        const CLASS_NAME *t_value = (CLASS_NAME *)v.get();                  \
        return to_string(t_value->v_);                                      \
      };                                                                    \
    }                                                                       \
  };                                                                        \
  static CLASS_NAME##ENUM_NAME##TYPE_NAME##to_string_registeration          \
      CLASS_NAME##ENUM_NAME##TYPE_NAME##to_string_registeration_instance;

template <typename T>
auto value_to(const SPtr<Value> &value) -> typename T::InternalType {
  const T *t_value = (T *)value.get();
  return t_value->v_;
}

std::string to_string(const SPtr<Value> &value) {
  auto &to_string_func_map = get_to_string_func_global_map();
  auto iter = to_string_func_map.find(value->type_);
  CHECK_WITH_INFO(iter != to_string_func_map.end(), "not find to_string func")
  return iter->second(value);
}

// Int Value
std::string to_string(int v) { return std::string("INT:") + std::to_string(v); }
DEFINE_VALUE(Int, INT, int)

// Bool Value
std::string to_string(bool v) {
  if (v) {
    return std::string("BOOL:True");
  } else {
    return std::string("BOOL:False");
  }
}
DEFINE_VALUE(Bool, BOOL, bool)

class Environment;
struct Procedure {
  SPtr<Environment> env_;
  std::string parameter_;
  SPtr<Expression> body_;
};
// procedure Value
std::string to_string(const Procedure &v) { return "PROCEDURE"; }
DEFINE_VALUE(ProcedureValue, PROC, Procedure)

class Environment;
struct RecursiveProcedure {
  SPtr<Environment> env_;
  std::string proc_name_;
  std::string parameter_;
  SPtr<Expression> proc_body_;
};
// procedure Value
std::string to_string(const RecursiveProcedure &v) { return "REC_PROCEDURE"; }
DEFINE_VALUE(RecursiveProcedureValue, REC_PROC, RecursiveProcedure)


struct Mutex {
  enum class MutextStatus {
    UNKNOWN,
    LOCKED,
    UNLOCKED,
  };
  MutextStatus status_;
};
std::string to_string(const Mutex &v) {
  std::stringstream ss;
  ss << "MUTEX(";
  if (v.status_ == Mutex::MutextStatus::LOCKED) {
    ss << "LOCKED)";
    return ss.str();
  }
  if (v.status_ == Mutex::MutextStatus::UNLOCKED) {
    ss << "UNLOCKED)";
    return ss.str();
  }
  ss << "UNKONW)";
  return ss.str();
}
DEFINE_VALUE(MutexValue, MUTEX, Mutex)