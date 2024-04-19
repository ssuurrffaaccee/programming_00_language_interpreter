#pragma once
#include <queue>

#include "check.hpp"
#include "continuation.hpp"
#include "environment.hpp"
#include "expression.hpp"
#include "scheduler.hpp"
#include "value.hpp"

SPtr<Value> apply_cont(const SPtr<Continuation> &cont,
                       const SPtr<Value> &value);

SPtr<Value> value_of_cont(const SPtr<Expression> &exp, SPtr<Environment> &env,
                          const SPtr<Continuation> &cont) {
  if (exp->type_ == ExpressionType::CONST_EXP) {
    auto const_exp = get<ConstExp>(exp);
    auto const_value = to_value(const_exp->num_);
    return apply_cont(cont, const_value);
  }
  if (exp->type_ == ExpressionType::VAR_EXP) {
    const VarExp *var_exp = get<VarExp>(exp);
    auto value = env->apply(var_exp->var_);
    return apply_cont(cont, value);
  }
  if (exp->type_ == ExpressionType::DIFF_EXP) {
    const DiffExp *diff_exp = get<DiffExp>(exp);
    auto diff1_cont = make_diff1_cont(diff_exp->right_exp_, env, cont);
    return value_of_cont(diff_exp->left_exp_, env, diff1_cont);
  }
  if (exp->type_ == ExpressionType::ADD_EXP) {
    const AddExp *add_exp = get<AddExp>(exp);
    auto add1_cont = make_add1_cont(add_exp->right_exp_, env, cont);
    return value_of_cont(add_exp->left_exp_, env, add1_cont);
  }
  if (exp->type_ == ExpressionType::IS_ZERO_EXP) {
    const IsZeroExp *is_zero_exp = get<IsZeroExp>(exp);
    auto zero1_cont = make_zero1_cont(cont);
    return value_of_cont(is_zero_exp->exp_, env, zero1_cont);
  }
  if (exp->type_ == ExpressionType::IF_EXP) {
    const IfExp *if_exp = get<IfExp>(exp);
    auto if_test_cont =
        make_if_test_cont(if_exp->true_exp_, if_exp->false_exp_, env, cont);
    return value_of_cont(if_exp->condition_exp_, env, if_test_cont);
  }
  if (exp->type_ == ExpressionType::LET_EXP) {
    const LetExp *let_exp = get<LetExp>(exp);
    auto let_cont = make_let_cont(let_exp->var_, let_exp->body_, env, cont);
    return value_of_cont(let_exp->exp_, env, let_cont);
  }
  if (exp->type_ == ExpressionType::PROC_EXP) {
    const ProcExp *proc_exp = get<ProcExp>(exp);
    Procedure procedure;
    procedure.parameter_ = proc_exp->parameter_;
    procedure.body_ = proc_exp->body_;
    procedure.env_ = env;
    auto proc_value = to_value(procedure);
    return apply_cont(cont, proc_value);
  }
  if (exp->type_ == ExpressionType::CALL_EXP) {
    const CallExp *call_exp = get<CallExp>(exp);
    auto operator_cont = make_operator_cont(call_exp->arg_, env, cont);
    return value_of_cont(call_exp->proc_generator_, env, operator_cont);
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
    return value_of_cont(let_rec_exp->apply_body_, new_env, cont);
  }
  if (exp->type_ == ExpressionType::PRINT_EXP) {
    const PrintExp *print_exp = get<PrintExp>(exp);
    auto print_cont = make_print_cont(cont);
    return value_of_cont(print_exp->exp_, env, print_cont);
  }
  if (exp->type_ == ExpressionType::BEGIN_EXP) {
    const BeginExp *begin_exp = get<BeginExp>(exp);
    CHECK(!(begin_exp->exps_.empty()))
    auto first_exp = begin_exp->exps_.front();
    if (begin_exp->exps_.size() == 1) {
      return value_of_cont(first_exp, env, cont);
    } else {
      auto begin_cont = make_begin_cont(begin_exp->exps_, 1, env, cont);
      return value_of_cont(first_exp, env, begin_cont);
    }
  }
  if (exp->type_ == ExpressionType::SPAWN_EXP) {
    const SpawnExp *spawn_exp = get<SpawnExp>(exp);
    auto spawn_cont = make_spawn_cont(cont);
    return value_of_cont(spawn_exp->exp_, env, spawn_cont);
  }
  if (exp->type_ == ExpressionType::YIELD_EXP) {
    ThreadScheduler::get().place_on_ready_queue(
        [=]() { return apply_cont(cont, to_value(99)); });
    auto next_thread = ThreadScheduler::get().get_next_thread();
    return next_thread.operator()();
  }
  CHECK_WITH_INFO(false, "got unkown expression type!!!!" +
                             std::to_string(int(exp->type_)))
  return nullptr;
}
SPtr<Value> call_procedure(const SPtr<Value> &proc_value,
                           const SPtr<Value> &value,
                           const SPtr<Continuation> &cont) {
  // value is argument value
  if (proc_value->type_ == ValueType::PROC) {
    auto proc = value_to<ProcedureValue>(proc_value);
    CHECK(proc.env_ != nullptr)
    auto new_env = proc.env_->extend(proc.parameter_, value);
    return value_of_cont(proc.body_, new_env, cont);
  }
  if (proc_value->type_ == ValueType::REC_PROC) {
    auto proc = value_to<RecursiveProcedureValue>(proc_value);
    CHECK(proc.env_ != nullptr)
    auto new_env_0 = proc.env_->extend(proc.proc_name_, proc_value);
    auto new_env_1 = new_env_0->extend(proc.parameter_, value);
    return value_of_cont(proc.proc_body_, new_env_1, cont);
  }
  CHECK_WITH_INFO(false, "unkown procedure type!!!");
}

SPtr<Value> apply_cont(const SPtr<Continuation> &cont,
                       const SPtr<Value> &value) {
  if (cont->type_ == ContinuationType::END_CONT) {
    ThreadScheduler::get().the_final_answer_ = value;
    auto next_thread = ThreadScheduler::get().get_next_thread();
    return next_thread.operator()();
  }
  if (cont->type_ == ContinuationType::END_SUBTHREAD_CONT) {
    auto next_thread = ThreadScheduler::get().get_next_thread();
    return next_thread.operator()();
  }
  if (cont->type_ == ContinuationType::ZERO1_CONT) {
    Zero1Cont *zero1_cont = get<Zero1Cont>(cont);
    CHECK(value->type_ == ValueType::INT)
    if (value_to<Int>(value) == 0) {
      return apply_cont(zero1_cont->cont_, to_value(true));
    } else {
      return apply_cont(zero1_cont->cont_, to_value(false));
    }
  }
  if (cont->type_ == ContinuationType::LET_CONT) {
    LetCont *let_cont = get<LetCont>(cont);
    auto new_env = let_cont->the_time_env_->extend(let_cont->var_, value);
    return value_of_cont(let_cont->body_, new_env, let_cont->cont_);
  }
  if (cont->type_ == ContinuationType::IF_TEST_CONT) {
    IfTestCont *if_test_cont = get<IfTestCont>(cont);
    CHECK(value->type_ == ValueType::BOOL)
    if (value_to<Bool>(value)) {
      return value_of_cont(if_test_cont->true_exp_, if_test_cont->the_time_env_,
                           if_test_cont->cont_);
    } else {
      return value_of_cont(if_test_cont->false_exp_,
                           if_test_cont->the_time_env_, if_test_cont->cont_);
    }
  }
  if (cont->type_ == ContinuationType::DIFF1_CONT) {
    Diff1Cont *diff1_cont = get<Diff1Cont>(cont);
    // value is thre result of exp1
    // diff is not complete,so we store the value
    auto diff2_cont = make_diff2_cont(value, diff1_cont->cont_);
    return value_of_cont(diff1_cont->exp2_, diff1_cont->the_time_env_,
                         diff2_cont);
  }
  if (cont->type_ == ContinuationType::DIFF2_CONT) {
    Diff2Cont *diff2_cont = get<Diff2Cont>(cont);
    CHECK(diff2_cont->exp1_value_->type_ == ValueType::INT)
    CHECK(value->type_ == ValueType::INT)
    auto diff_value =
        to_value(value_to<Int>(diff2_cont->exp1_value_) - value_to<Int>(value));
    return apply_cont(diff2_cont->cont_, diff_value);
  }
  if (cont->type_ == ContinuationType::ADD1_CONT) {
    Add1Cont *add1_cont = get<Add1Cont>(cont);
    // value is thre result of exp1
    auto add2_cont = make_add2_cont(value, add1_cont->cont_);
    return value_of_cont(add1_cont->exp2_, add1_cont->the_time_env_, add2_cont);
  }
  if (cont->type_ == ContinuationType::ADD2_CONT) {
    Add2Cont *add2_cont = get<Add2Cont>(cont);
    CHECK(add2_cont->exp1_value_->type_ == ValueType::INT)
    CHECK(value->type_ == ValueType::INT)
    auto sum_value =
        to_value(value_to<Int>(add2_cont->exp1_value_) + value_to<Int>(value));

    return apply_cont(add2_cont->cont_, sum_value);
  }
  if (cont->type_ == ContinuationType::OPRATOR_CONT) {
    OperatorCont *operator_cont = get<OperatorCont>(cont);
    // value is proc_value
    auto operand_cont = make_oprand_cont(value, operator_cont->cont_);
    return value_of_cont(operator_cont->arg_exp_, operator_cont->the_time_env_,
                         operand_cont);
  }
  if (cont->type_ == ContinuationType::OPRAND_CONT) {
    OperandCont *operand_cont = get<OperandCont>(cont);
    auto proc_value = operand_cont->proc_value_;
    return call_procedure(proc_value, value, operand_cont->cont_);
  }
  if (cont->type_ == ContinuationType::PRINT_CONT) {
    PrintCont *print_cont = get<PrintCont>(cont);
    std::cout << "print: " << to_string(value) << '\n';
    return apply_cont(print_cont->cont_, value);
  }
  if (cont->type_ == ContinuationType::BEGIN_CONT) {
    BeginCont *begin_cont = get<BeginCont>(cont);
    if (begin_cont->next_exp_idx_ == begin_cont->exps_.size() - 1) {
      return value_of_cont(begin_cont->exps_[begin_cont->next_exp_idx_],
                           begin_cont->the_time_env_, begin_cont->cont_);
    } else {
      auto cur_exp_idx = begin_cont->next_exp_idx_;
      begin_cont->next_exp_idx_++;
      return value_of_cont(begin_cont->exps_[cur_exp_idx],
                           begin_cont->the_time_env_, cont);
    }
  }
  if (cont->type_ == ContinuationType::SPAWN_CONT) {
    SpawnCont *spawn_cont = get<SpawnCont>(cont);
    CHECK(value->type_ == ValueType::PROC ||
          value->type_ == ValueType::REC_PROC)
    ThreadScheduler::get().place_on_ready_queue([proc_value = value]() {
      return call_procedure(proc_value, to_value(28),
                            make_end_subthread_cont());
    });
    return apply_cont(spawn_cont->cont_, to_value(73));
  }
  CHECK_WITH_INFO(false, "got unkown continuation type!!!!")
  return nullptr;
}