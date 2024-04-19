#pragma once
#include <iostream>

#include "check.hpp"
#include "environment.hpp"
#include "expression.hpp"
#include "store.hpp"
#include "value.hpp"

template <typename T>
T *get(const SPtr<Expression> &exp) {
  return (T *)exp.get();
}

Ref fake_value() {
  // return fake value
  return Ref{27};
}

Ref value_of(const SPtr<Expression> &exp, SPtr<Environment> &env,
             SPtr<Store> &store) {
  if (exp->type_ == ExpressionType::CONST_EXP) {
    auto const_exp = get<ConstExp>(exp);
    auto const_value = to_value(const_exp->num_);
    return store->new_ref(const_value);
  }
  if (exp->type_ == ExpressionType::VAR_EXP) {
    const VarExp *var_exp = get<VarExp>(exp);
    auto ref = env->apply(var_exp->var_);
    auto value = store->deref(ref);
    if(value->type_ ==ValueType::LAZY){
        auto lazy = value_to<LazyValue>(value);
        auto actual_value_ref = value_of(lazy.arg_exp_,lazy.env_,store);
        store->set_ref(ref,store->deref(actual_value_ref));
    }
    return ref;
  }
  if (exp->type_ == ExpressionType::DIFF_EXP) {
    const DiffExp *diff_exp = get<DiffExp>(exp);
    Ref left_value_ref = value_of(diff_exp->left_exp_, env, store);
    Ref right_value_ref = value_of(diff_exp->right_exp_, env, store);
    auto left_value = store->deref(left_value_ref);
    auto right_value = store->deref(right_value_ref);
    CHECK(left_value->type_ == ValueType::INT)
    CHECK(right_value->type_ == ValueType::INT)
    auto diff_value =
        to_value(value_to<Int>(left_value) - value_to<Int>(right_value));
    return store->new_ref(diff_value);
  }
  if (exp->type_ == ExpressionType::ADD_EXP) {
    const AddExp *add_exp = get<AddExp>(exp);
    Ref left_value_ref = value_of(add_exp->left_exp_, env, store);
    Ref right_value_ref = value_of(add_exp->right_exp_, env, store);
    auto left_value = store->deref(left_value_ref);
    auto right_value = store->deref(right_value_ref);
    CHECK(left_value->type_ == ValueType::INT)
    CHECK(right_value->type_ == ValueType::INT)
    auto add_value =
        to_value(value_to<Int>(left_value) + value_to<Int>(right_value));
    return store->new_ref(add_value);
  }
  if (exp->type_ == ExpressionType::IS_ZERO_EXP) {
    const IsZeroExp *is_zero_exp = get<IsZeroExp>(exp);
    Ref value_ref = value_of(is_zero_exp->exp_, env, store);
    auto value = store->deref(value_ref);
    CHECK(value->type_ == ValueType::INT)
    if (0 == value_to<Int>(value)) {
      return store->new_ref(to_value(true));
    } else {
      return store->new_ref(to_value(false));
    }
  }
  if (exp->type_ == ExpressionType::IF_EXP) {
    const IfExp *if_exp = get<IfExp>(exp);
    Ref value_ref = value_of(if_exp->condition_exp_, env, store);
    auto value = store->deref(value_ref);
    CHECK(value->type_ == ValueType::BOOL)
    if (value_to<Bool>(value)) {
      return value_of(if_exp->true_exp_, env, store);
    } else {
      return value_of(if_exp->false_exp_, env, store);
    }
  }
  if (exp->type_ == ExpressionType::LET_EXP) {
    const LetExp *let_exp = get<LetExp>(exp);
    auto value_ref = value_of(let_exp->exp_, env, store);
    auto value = store->deref(value_ref);
    auto ref = store->new_ref(value);
    auto new_env = env->extend(let_exp->var_, ref);
    return value_of(let_exp->body_, new_env, store);
  }
  if (exp->type_ == ExpressionType::PROC_EXP) {
    const ProcExp *proc_exp = get<ProcExp>(exp);
    Procedure procedure;
    procedure.parameter_ = proc_exp->parameter_;
    procedure.body_ = proc_exp->body_;
    procedure.env_ = env;
    auto proc_value = to_value(procedure);
    return store->new_ref(proc_value);
  }
  if (exp->type_ == ExpressionType::CALL_EXP) {
    const CallExp *call_exp = get<CallExp>(exp);
    auto proc_value_ref = value_of(call_exp->proc_generator_, env, store);
    Lazy lazy;
    lazy.arg_exp_ = call_exp->arg_;
    lazy.env_ = env;
    auto lazy_value = to_value(lazy);
    auto arg_value_ref = store->new_ref(lazy_value);
    auto proc_value = store->deref(proc_value_ref);
    if (proc_value->type_ == ValueType::PROC) {
      auto proc = value_to<ProcedureValue>(proc_value);
      CHECK(proc.env_ != nullptr)
      auto new_env = proc.env_->extend(proc.parameter_, arg_value_ref);
      return value_of(proc.body_, new_env, store);
    }
    if (proc_value->type_ == ValueType::REC_PROC) {
      auto proc = value_to<RecursiveProcedureValue>(proc_value);
      CHECK(proc.env_ != nullptr)
      auto new_env_0 = proc.env_->extend(proc.proc_name_, proc_value_ref);
      auto new_env_1 = new_env_0->extend(proc.parameter_, arg_value_ref);
      return value_of(proc.proc_body_, new_env_1, store);
    }
    CHECK_WITH_INFO(false, "got unkown proc type");
  }
  if (exp->type_ == ExpressionType::LETREC_EXP) {
    const LetRecExp *let_rec_exp = get<LetRecExp>(exp);
    RecursiveProcedure procedure;
    procedure.proc_name_ = let_rec_exp->proc_name_;
    procedure.parameter_ = let_rec_exp->parameter_;
    procedure.proc_body_ = let_rec_exp->proc_body_;
    procedure.env_ = env;
    auto procedure_value = to_value(procedure);
    auto procedure_value_ref = store->new_ref(procedure_value);
    auto new_env = env->extend(let_rec_exp->proc_name_, procedure_value_ref);
    return value_of(let_rec_exp->apply_body_, new_env, store);
  }
  if (exp->type_ == ExpressionType::BEGIN_EXP) {
    const BeginExp *begin_exp = get<BeginExp>(exp);
    Ref res_ref;
    for (auto &exp : begin_exp->exps_) {
      res_ref = value_of(exp, env, store);
    }
    return res_ref;
  }
  if (exp->type_ == ExpressionType::ASSIGN_EXP) {
    const AssignExp *assign_exp = get<AssignExp>(exp);
    auto ref_value = env->apply(assign_exp->name_);
    auto value_ref = value_of(assign_exp->value_exp_, env, store);
    store->set_ref(ref_value, store->deref(value_ref));
    return fake_value();
  }
  if (exp->type_ == ExpressionType::PRINT_EXP) {
    const PrintExp *print_exp = get<PrintExp>(exp);
    auto value_ref = value_of(print_exp->exp_, env, store);
    {
      auto value = store->deref(value_ref);
      std::cout << "print " << to_string(value) << "\n";
    }
    return value_ref;
  }
  if (exp->type_ == ExpressionType::MAKE_PAIR_EXP) {
    const MakePairExp *make_pair_exp = get<MakePairExp>(exp);
    Ref left_value_ref = value_of(make_pair_exp->left_exp_, env, store);
    Ref right_value_ref = value_of(make_pair_exp->right_exp_, env, store);
    Pair pair;
    pair.left_value_ref_ = left_value_ref;
    pair.right_value_ref_ = right_value_ref;
    auto pair_value = to_value(pair);
    return store->new_ref(pair_value);
  }
  if (exp->type_ == ExpressionType::LEFT_EXP) {
    const LeftExp *left_exp = get<LeftExp>(exp);
    Ref pair_value_ref = value_of(left_exp->pair_exp_, env, store);
    auto pair_value = store->deref(pair_value_ref);
    return value_to<PairValue>(pair_value).left_value_ref_;
  }
  if (exp->type_ == ExpressionType::RIGHT_EXP) {
    const RightExp *right_exp = get<RightExp>(exp);
    Ref pair_value_ref = value_of(right_exp->pair_exp_, env, store);
    auto pair_value = store->deref(pair_value_ref);
    return value_to<PairValue>(pair_value).right_value_ref_;
  }
  if (exp->type_ == ExpressionType::SET_LEFT_EXP) {
    const SetLeftExp *set_left_exp = get<SetLeftExp>(exp);
    Ref pair_value_ref = value_of(set_left_exp->pair_exp_, env, store);
    Ref value_ref = value_of(set_left_exp->value_exp_, env, store);
    auto pair_value = store->deref(pair_value_ref);
    CHECK(pair_value->type_ == ValueType::PAIR)
    auto pair_value_ptr = (PairValue *)pair_value.get();
    pair_value_ptr->v_.left_value_ref_ = value_ref;
    return fake_value();
  }
  if (exp->type_ == ExpressionType::SET_RIGHT_EXP) {
    const SetRightExp *set_right_exp = get<SetRightExp>(exp);
    Ref pair_value_ref = value_of(set_right_exp->pair_exp_, env, store);
    Ref value_ref = value_of(set_right_exp->value_exp_, env, store);
    auto pair_value = store->deref(pair_value_ref);
    CHECK(pair_value->type_ == ValueType::PAIR)
    auto pair_value_ptr = (PairValue *)pair_value.get();
    pair_value_ptr->v_.right_value_ref_ = value_ref;
    return fake_value();
  }
  CHECK_WITH_INFO(false, "got unkown expression type!!!!")
  return Ref{-1};
}