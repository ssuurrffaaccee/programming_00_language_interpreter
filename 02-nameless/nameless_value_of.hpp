#pragma once
#include "check.hpp"
#include "expression.hpp"
#include "nameless_environment.hpp"
#include "nameless_expression.hpp"
#include "value.hpp"
namespace nl = nameless;
template <typename T>
T* get(const SPtr<nl::Expression>& exp) {
  return (T*)exp.get();
}
SPtr<Value> value_of(const SPtr<nl::Expression>& exp,
                     SPtr<NamelessEnvironment>& env) {
  if (exp->type_ == nl::ExpressionType::CONST_EXP) {
    auto* const_exp = get<nl::ConstExp>(exp);
    return to_value(const_exp->num_);
  }
  if (exp->type_ == nl::ExpressionType::VAR_EXP) {
    auto* var_exp = get<nl::VarExp>(exp);
    return env->apply(var_exp->index_);
  }
  if (exp->type_ == nl::ExpressionType::DIFF_EXP) {
    auto* diff_exp = get<nl::DiffExp>(exp);
    SPtr<Value> left_value = value_of(diff_exp->left_exp_, env);
    SPtr<Value> right_value = value_of(diff_exp->right_exp_, env);
    CHECK(left_value->type_ == ValueType::INT)
    CHECK(right_value->type_ == ValueType::INT)
    return to_value(value_to<Int>(left_value) - value_to<Int>(right_value));
  }
  if (exp->type_ == nl::ExpressionType::ADD_EXP) {
    auto* add_exp = get<nl::AddExp>(exp);
    SPtr<Value> left_value = value_of(add_exp->left_exp_, env);
    SPtr<Value> right_value = value_of(add_exp->right_exp_, env);
    CHECK(left_value->type_ == ValueType::INT)
    CHECK(right_value->type_ == ValueType::INT)
    return to_value(value_to<Int>(left_value) + value_to<Int>(right_value));
  }
  if (exp->type_ == nl::ExpressionType::IS_ZERO_EXP) {
    auto* is_zero_exp = get<nl::IsZeroExp>(exp);
    SPtr<Value> value = value_of(is_zero_exp->exp_, env);
    if (0 == value_to<Int>(value)) {
      return to_value(true);
    } else {
      return to_value(false);
    }
  }
  if (exp->type_ == nl::ExpressionType::IF_EXP) {
    auto* if_exp = get<nl::IfExp>(exp);
    SPtr<Value> value = value_of(if_exp->condition_exp_, env);
    CHECK(value->type_ == ValueType::BOOL)
    if (value_to<Bool>(value)) {
      return value_of(if_exp->true_exp_, env);
    } else {
      return value_of(if_exp->false_exp_, env);
    }
  }
  if (exp->type_ == nl::ExpressionType::LET_EXP) {
    auto* let_exp = get<nl::LetExp>(exp);
    SPtr<Value> value = value_of(let_exp->exp_, env);
    auto new_env = env->extend(value);
    return value_of(let_exp->body_, new_env);
  }
  if (exp->type_ == nl::ExpressionType::PROC_EXP) {
    auto* proc_exp = get<nl::ProcExp>(exp);
    Procedure procedure;
    procedure.body_ = proc_exp->body_;
    procedure.env_ = env;
    return to_value(procedure);
  }
  if (exp->type_ == nl::ExpressionType::CALL_EXP) {
    auto* call_exp = get<nl::CallExp>(exp);
    auto proc_value = value_of(call_exp->proc_generator_, env);
    auto arg_value = value_of(call_exp->arg_, env);
    auto proc = value_to<ProcedureValue>(proc_value);
    CHECK(proc.env_ != nullptr)
    auto new_env = proc.env_->extend(arg_value);
    return value_of(proc.body_, new_env);
  }
  CHECK_WITH_INFO(false, "got unkown expression type!!!!")
  return nullptr;
}