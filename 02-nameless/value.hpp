#pragma once
#include <functional>
#include <sstream>
#include <unordered_map>

#include "nameless_expression.hpp"
#include "sptr.hpp"
enum class ValueType {
  INT,
  BOOL,
  PROC,
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

class NamelessEnvironment;
namespace nameless {
class Expression;
}
struct Procedure {
  SPtr<NamelessEnvironment> env_;
  SPtr<nameless::Expression> body_;
};
// procedure Value
std::string to_string(const Procedure &v) { return "PROCEDURE"; }
DEFINE_VALUE(ProcedureValue, PROC, Procedure)