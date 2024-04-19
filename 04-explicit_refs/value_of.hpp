#pragma once
#include "check.hpp"
#include "environment.hpp"
#include "expression.hpp"
#include "store.hpp"
#include "value.hpp"

template <typename T>
T *get(const SPtr<Expression> &exp) {
  return (T *)exp.get();
}

SPtr<Value> value_of(const SPtr<Expression> &exp, SPtr<Environment> &env,
                     SPtr<Store> &store) {
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
    SPtr<Value> left_value = value_of(diff_exp->left_exp_, env, store);
    SPtr<Value> right_value = value_of(diff_exp->right_exp_, env, store);
    CHECK(left_value->type_ == ValueType::INT)
    CHECK(right_value->type_ == ValueType::INT)
    return to_value(value_to<Int>(left_value) - value_to<Int>(right_value));
  }
  if (exp->type_ == ExpressionType::ADD_EXP) {
    const AddExp *add_exp = get<AddExp>(exp);
    SPtr<Value> left_value = value_of(add_exp->left_exp_, env, store);
    SPtr<Value> right_value = value_of(add_exp->right_exp_, env, store);
    CHECK(left_value->type_ == ValueType::INT)
    CHECK(right_value->type_ == ValueType::INT)
    return to_value(value_to<Int>(left_value) + value_to<Int>(right_value));
  }
  if (exp->type_ == ExpressionType::IS_ZERO_EXP) {
    const IsZeroExp *is_zero_exp = get<IsZeroExp>(exp);
    SPtr<Value> value = value_of(is_zero_exp->exp_, env, store);
    if (0 == value_to<Int>(value)) {
      return to_value(true);
    } else {
      return to_value(false);
    }
  }
  if (exp->type_ == ExpressionType::IF_EXP) {
    const IfExp *if_exp = get<IfExp>(exp);
    SPtr<Value> value = value_of(if_exp->condition_exp_, env, store);
    CHECK(value->type_ == ValueType::BOOL)
    if (value_to<Bool>(value)) {
      return value_of(if_exp->true_exp_, env, store);
    } else {
      return value_of(if_exp->false_exp_, env, store);
    }
  }
  if (exp->type_ == ExpressionType::LET_EXP) {
    const LetExp *let_exp = get<LetExp>(exp);
    SPtr<Value> value = value_of(let_exp->exp_, env, store);
    auto new_env = env->extend(let_exp->var_, value);
    return value_of(let_exp->body_, new_env, store);
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
    auto proc_value = value_of(call_exp->proc_generator_, env, store);
    auto arg_value = value_of(call_exp->arg_, env, store);
    if (proc_value->type_ == ValueType::PROC) {
      auto proc = value_to<ProcedureValue>(proc_value);
      CHECK(proc.env_ != nullptr)
      auto new_env = proc.env_->extend(proc.parameter_, arg_value);
      return value_of(proc.body_, new_env, store);
    }
    if (proc_value->type_ == ValueType::REC_PROC) {
      auto proc = value_to<RecursiveProcedureValue>(proc_value);
      CHECK(proc.env_ != nullptr)
      auto new_env_0 = proc.env_->extend(proc.proc_name_, proc_value);
      auto new_env_1 = new_env_0->extend(proc.parameter_, arg_value);
      return value_of(proc.proc_body_, new_env_1, store);
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
    return value_of(let_rec_exp->apply_body_, new_env, store);
  }
  if (exp->type_ == ExpressionType::BEGIN_EXP) {
    const BeginExp *begin_exp = get<BeginExp>(exp);
    SPtr<Value> res;
    for (auto &exp : begin_exp->exps_) {
      res = value_of(exp, env, store);
    }
    return res;
  }
  if (exp->type_ == ExpressionType::NEWREF_EXP) {
    const NewRefExp *new_ref_exp = get<NewRefExp>(exp);
    auto value = value_of(new_ref_exp->exp_, env, store);
    return to_value(store->new_ref(value));
  }
  if (exp->type_ == ExpressionType::DEREF_EXP) {
    const DerefExp *deref_exp = get<DerefExp>(exp);
    auto value = value_of(deref_exp->exp_, env, store);
    return store->deref(value_to<RefValue>(value));
  }
  if (exp->type_ == ExpressionType::SET_REF_EXP) {
    const SetRefExp *set_ref_exp = get<SetRefExp>(exp);
    auto ref_value = value_of(set_ref_exp->ref_exp_, env, store);
    auto value = value_of(set_ref_exp->value_exp_, env, store);
    store->set_ref(value_to<RefValue>(ref_value), value);
    // return fake value
    auto fake_ret_value = new Int{};
    fake_ret_value->v_ = 23;
    return SPtr<Value>{(Value *)fake_ret_value};
  }
  CHECK_WITH_INFO(false, "got unkown expression type!!!!")
  return nullptr;
}