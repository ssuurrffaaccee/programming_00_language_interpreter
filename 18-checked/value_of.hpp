#pragma once
#include "check.hpp"
#include "environment.hpp"
#include "expression.hpp"
#include "value.hpp"
SPtr<Value> value_of(const SPtr<Expression> &exp, SPtr<Environment> &env) {
  if (exp->type_ == ExpressionType::CONST_EXP) {
    auto const_exp = get<ConstExp>(exp);
    return to_value(const_exp->num_);
  }
  if (exp->type_ == ExpressionType::VAR_EXP) {
    const VarExp *var_exp = get<VarExp>(exp);
    return env->apply(var_exp->var_);
  }
  if (exp->type_ == ExpressionType::DIFF_EXP) {
    const DiffExp *diff_exp = get<DiffExp>(exp);
    SPtr<Value> left_value = value_of(diff_exp->left_exp_, env);
    SPtr<Value> right_value = value_of(diff_exp->right_exp_, env);
    CHECK(left_value->type_ == ValueType::INT)
    CHECK(right_value->type_ == ValueType::INT)
    return to_value(value_to<Int>(left_value) - value_to<Int>(right_value));
  }
  if (exp->type_ == ExpressionType::ADD_EXP) {
    const AddExp *add_exp = get<AddExp>(exp);
    SPtr<Value> left_value = value_of(add_exp->left_exp_, env);
    SPtr<Value> right_value = value_of(add_exp->right_exp_, env);
    CHECK(left_value->type_ == ValueType::INT)
    CHECK(right_value->type_ == ValueType::INT)
    return to_value(value_to<Int>(left_value) + value_to<Int>(right_value));
  }
  if (exp->type_ == ExpressionType::IS_ZERO_EXP) {
    const IsZeroExp *is_zero_exp = get<IsZeroExp>(exp);
    SPtr<Value> value = value_of(is_zero_exp->exp_, env);
    if (0 == value_to<Int>(value)) {
      return to_value(true);
    } else {
      return to_value(false);
    }
  }
  if (exp->type_ == ExpressionType::IF_EXP) {
    const IfExp *if_exp = get<IfExp>(exp);
    SPtr<Value> value = value_of(if_exp->condition_exp_, env);
    CHECK(value->type_ == ValueType::BOOL)
    if (value_to<Bool>(value)) {
      return value_of(if_exp->true_exp_, env);
    } else {
      return value_of(if_exp->false_exp_, env);
    }
  }
  if (exp->type_ == ExpressionType::LET_EXP) {
    const LetExp *let_exp = get<LetExp>(exp);
    SPtr<Value> value = value_of(let_exp->exp_, env);
    auto new_env = env->extend(let_exp->var_, value);
    return value_of(let_exp->body_, new_env);
  }
  if (exp->type_ == ExpressionType::PROC_EXP) {
    const ProcExp *proc_exp = get<ProcExp>(exp);
    Procedure procedure;
    procedure.parameter_ = proc_exp->parameter_;
    procedure.body_ = proc_exp->body_;
    procedure.env_ = env;
    return to_value(procedure);
  }
  if (exp->type_ == ExpressionType::CALL_EXP) {
    const CallExp *call_exp = get<CallExp>(exp);
    auto proc_value = value_of(call_exp->proc_generator_, env);
    auto arg_value = value_of(call_exp->arg_, env);
    if (proc_value->type_ == ValueType::PROC) {
      auto proc = value_to<ProcedureValue>(proc_value);
      CHECK(proc.env_ != nullptr)
      auto new_env = proc.env_->extend(proc.parameter_, arg_value);
      return value_of(proc.body_, new_env);
    }
    if (proc_value->type_ == ValueType::REC_PROC) {
      auto proc = value_to<RecursiveProcedureValue>(proc_value);
      CHECK(proc.env_ != nullptr)
      auto new_env_0 = proc.env_->extend(proc.proc_name_, proc_value);
      auto new_env_1 = new_env_0->extend(proc.parameter_, arg_value);
      return value_of(proc.proc_body_, new_env_1);
    }
    CHECK_WITH_INFO(false,"got unkown proc type");
  }
  if (exp->type_ == ExpressionType::LETREC_EXP) {
    const LetRecExp *let_rec_exp = get<LetRecExp>(exp);
    RecursiveProcedure procedure;
    procedure.proc_name_ = let_rec_exp->proc_name_;
    procedure.parameter_ = let_rec_exp->parameter_;
    procedure.proc_body_ = let_rec_exp->proc_body_;
    procedure.env_ = env;
    auto procedure_value = to_value(procedure);
    auto new_env = env->extend(let_rec_exp->proc_name_, procedure_value);
    return value_of(let_rec_exp->apply_body_, new_env);
  }
  CHECK_WITH_INFO(false, "got unkown expression type!!!!")
  return nullptr;
}