#pragma once
#include "check.hpp"
#include "continuation.hpp"
#include "environment.hpp"
#include "expression.hpp"
#include "scheduler.hpp"
#include "value.hpp"

void call_procedure(const SPtr<Value> &proc_value, const SPtr<Value> &value,
                    const SPtr<ContStack> &cur_stack) {
  // value is argument value
  if (proc_value->type_ == ValueType::PROC) {
    auto proc = value_to<ProcedureValue>(proc_value);
    CHECK(proc.env_ != nullptr)
    auto new_env = proc.env_->extend(proc.parameter_, value);
    auto exp_cont = make_exp_cont(proc.body_, new_env);
    cur_stack->push(exp_cont);
    return;
  }
  if (proc_value->type_ == ValueType::REC_PROC) {
    auto proc = value_to<RecursiveProcedureValue>(proc_value);
    CHECK(proc.env_ != nullptr)
    auto new_env_0 = proc.env_->extend(proc.proc_name_, proc_value);
    auto new_env_1 = new_env_0->extend(proc.parameter_, value);
    auto exp_cont = make_exp_cont(proc.proc_body_, new_env_1);
    cur_stack->push(exp_cont);
    return;
  }
  CHECK_WITH_INFO(false, "unkown procedure type!!!");
}

SPtr<Value> value_of_cont(SPtr<ContStack> &main_thread_cont_stack) {
  SPtr<Value> value;
  SPtr<ContStack> cur_stack = main_thread_cont_stack;
  while (!cur_stack->empty()) {
    auto cont = cur_stack->top();
    cur_stack->pop();
    if (cont->type_ == ContinuationType::EXP_CONT) {
      ExpCont *exp_cont = get<ExpCont>(cont);
      auto exp = exp_cont->exp_;
      auto env = exp_cont->env_;
      if (exp->type_ == ExpressionType::CONST_EXP) {
        auto const_exp = get<ConstExp>(exp);
        value = to_value(const_exp->num_);
        continue;
      }
      if (exp->type_ == ExpressionType::VAR_EXP) {
        const VarExp *var_exp = get<VarExp>(exp);
        value = env->apply(var_exp->var_);
        continue;
      }
      if (exp->type_ == ExpressionType::DIFF_EXP) {
        const DiffExp *diff_exp = get<DiffExp>(exp);
        auto diff1_cont = make_diff1_cont(diff_exp->right_exp_, env);
        cur_stack->push(diff1_cont);
        auto exp_cont = make_exp_cont(diff_exp->left_exp_, env);
        cur_stack->push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::ADD_EXP) {
        const AddExp *add_exp = get<AddExp>(exp);
        auto add1_cont = make_add1_cont(add_exp->right_exp_, env);
        cur_stack->push(add1_cont);
        auto exp_cont = make_exp_cont(add_exp->left_exp_, env);
        cur_stack->push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::IS_ZERO_EXP) {
        const IsZeroExp *is_zero_exp = get<IsZeroExp>(exp);
        auto zero1_cont = make_zero1_cont();
        cur_stack->push(zero1_cont);
        auto exp_cont = make_exp_cont(is_zero_exp->exp_, env);
        cur_stack->push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::IF_EXP) {
        const IfExp *if_exp = get<IfExp>(exp);
        auto if_test_cont =
            make_if_test_cont(if_exp->true_exp_, if_exp->false_exp_, env);
        cur_stack->push(if_test_cont);
        auto exp_cont = make_exp_cont(if_exp->condition_exp_, env);
        cur_stack->push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::LET_EXP) {
        const LetExp *let_exp = get<LetExp>(exp);
        auto let_cont = make_let_cont(let_exp->var_, let_exp->body_, env);
        cur_stack->push(let_cont);
        auto exp_cont = make_exp_cont(let_exp->exp_, env);
        cur_stack->push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::PROC_EXP) {
        const ProcExp *proc_exp = get<ProcExp>(exp);
        Procedure procedure;
        procedure.parameter_ = proc_exp->parameter_;
        procedure.body_ = proc_exp->body_;
        procedure.env_ = env;
        value = to_value(procedure);
        continue;
      }
      if (exp->type_ == ExpressionType::CALL_EXP) {
        const CallExp *call_exp = get<CallExp>(exp);
        auto operator_cont = make_operator_cont(call_exp->arg_, env);
        cur_stack->push(operator_cont);
        auto exp_cont = make_exp_cont(call_exp->proc_generator_, env);
        cur_stack->push(exp_cont);
        continue;
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
        auto exp_cont = make_exp_cont(let_rec_exp->apply_body_, new_env);
        cur_stack->push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::PRINT_EXP) {
        const PrintExp *print_exp = get<PrintExp>(exp);
        auto print_cont = make_print_cont();
        cur_stack->push(print_cont);
        auto exp_cont = make_exp_cont(print_exp->exp_, env);
        cur_stack->push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::BEGIN_EXP) {
        const BeginExp *begin_exp = get<BeginExp>(exp);
        CHECK(!(begin_exp->exps_.empty()))
        auto first_exp = begin_exp->exps_.front();
        if (begin_exp->exps_.size() == 1) {
          auto exp_cont = make_exp_cont(first_exp, env);
          cur_stack->push(exp_cont);
          continue;
        } else {
          auto begin_cont = make_begin_cont(begin_exp->exps_, 1, env);
          cur_stack->push(begin_cont);
          auto exp_cont = make_exp_cont(first_exp, env);
          cur_stack->push(exp_cont);
          continue;
        }
      }
      if (exp->type_ == ExpressionType::SET_EXP) {
        const SetExp *set_exp = get<SetExp>(exp);
        auto set_cont = make_set_cont(set_exp->var_name_, env);
        cur_stack->push(set_cont);
        auto exp_cont = make_exp_cont(set_exp->exp_, env);
        cur_stack->push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::MUTEX_EXP) {
        const MutexExp *mutex_exp = get<MutexExp>(exp);
        Mutex mutex;
        mutex.status_ = Mutex::MutextStatus::UNLOCKED;
        value = to_value(mutex);
        continue;
      }
      if (exp->type_ == ExpressionType::SPAWN_EXP) {
        const SpawnExp *spawn_exp = get<SpawnExp>(exp);
        auto spawn_cont = make_spawn_cont();
        cur_stack->push(spawn_cont);
        auto exp_cont = make_exp_cont(spawn_exp->exp_, env);
        cur_stack->push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::YIELD_EXP) {
        ThreadScheduler::get().place_on_ready_queue(cur_stack);
        // CHECK(!ThreadScheduler::get().emtpy())
        cur_stack = ThreadScheduler::get().get_next_thread_stack();
        continue;
      }
      if (exp->type_ == ExpressionType::WAIT_EXP) {
        const WaitExp *wait_exp = get<WaitExp>(exp);
        auto wait_cont = make_wait_cont(wait_exp->mutex_name_, env);
        cur_stack->push(wait_cont);
        value = to_value(55);
        continue;
      }
      if (exp->type_ == ExpressionType::SIGNAL_EXP) {
        const SignalExp *signal_exp = get<SignalExp>(exp);
        auto value = env->apply(signal_exp->mutex_name_);
        CHECK(value->type_ == ValueType::MUTEX)
        Mutex mutex = value_to<MutexValue>(value);
        if (mutex.status_ == Mutex::MutextStatus::LOCKED) {
          mutex.status_ = Mutex::MutextStatus::UNLOCKED;
          ThreadScheduler::get().signal(signal_exp->mutex_name_);
          env->set(signal_exp->mutex_name_, to_value(mutex));
          value = to_value(53);
          continue;
        }
        if (mutex.status_ == Mutex::MutextStatus::UNLOCKED) {
          CHECK_WITH_INFO(false, "error: signal unlocked mutex!!!!!")
        }
        CHECK_WITH_INFO(false, "got unkown mutex status!!!")
      }
      CHECK_WITH_INFO(false, "got unkown expression type!!!!")
    }
    if (cont->type_ == ContinuationType::END_CONT) {
      ThreadScheduler::get().the_final_answer_ = value;
      if (!ThreadScheduler::get().empty()) {
        cur_stack = ThreadScheduler::get().get_next_thread_stack();
        continue;
      }
      break;
    }
    if (cont->type_ == ContinuationType::END_SUBTHREAD_CONT) {
      if (!ThreadScheduler::get().empty()) {
        cur_stack = ThreadScheduler::get().get_next_thread_stack();
        continue;
      }
      break;
    }
    if (cont->type_ == ContinuationType::ZERO1_CONT) {
      Zero1Cont *zero1_cont = get<Zero1Cont>(cont);
      CHECK(value->type_ == ValueType::INT)
      if (value_to<Int>(value) == 0) {
        value = to_value(true);
      } else {
        value = to_value(false);
      }
      continue;
    }
    if (cont->type_ == ContinuationType::LET_CONT) {
      LetCont *let_cont = get<LetCont>(cont);
      auto new_env = let_cont->the_time_env_->extend(let_cont->var_, value);
      auto exp_cont = make_exp_cont(let_cont->body_, new_env);
      cur_stack->push(exp_cont);
      continue;
    }
    if (cont->type_ == ContinuationType::IF_TEST_CONT) {
      IfTestCont *if_test_cont = get<IfTestCont>(cont);
      CHECK(value->type_ == ValueType::BOOL)
      if (value_to<Bool>(value)) {
        auto exp_cont =
            make_exp_cont(if_test_cont->true_exp_, if_test_cont->the_time_env_);
        cur_stack->push(exp_cont);
        continue;
      } else {
        auto exp_cont = make_exp_cont(if_test_cont->false_exp_,
                                      if_test_cont->the_time_env_);
        cur_stack->push(exp_cont);
        continue;
      }
    }
    if (cont->type_ == ContinuationType::DIFF1_CONT) {
      Diff1Cont *diff1_cont = get<Diff1Cont>(cont);
      // value is thre result of exp1
      // diff is not complete,so we store the value
      auto diff2_cont = make_diff2_cont(value);
      cur_stack->push(diff2_cont);
      auto exp_cont =
          make_exp_cont(diff1_cont->exp2_, diff1_cont->the_time_env_);
      cur_stack->push(exp_cont);
      continue;
    }
    if (cont->type_ == ContinuationType::DIFF2_CONT) {
      Diff2Cont *diff2_cont = get<Diff2Cont>(cont);
      CHECK(diff2_cont->exp1_value_->type_ == ValueType::INT)
      CHECK(value->type_ == ValueType::INT)
      value = to_value(value_to<Int>(diff2_cont->exp1_value_) -
                       value_to<Int>(value));
      continue;
    }
    if (cont->type_ == ContinuationType::ADD1_CONT) {
      Add1Cont *add1_cont = get<Add1Cont>(cont);
      // value is thre result of exp1
      auto add2_cont = make_add2_cont(value);
      cur_stack->push(add2_cont);
      auto exp_cont = make_exp_cont(add1_cont->exp2_, add1_cont->the_time_env_);
      cur_stack->push(exp_cont);
      continue;
    }
    if (cont->type_ == ContinuationType::ADD2_CONT) {
      Add2Cont *add2_cont = get<Add2Cont>(cont);
      CHECK(add2_cont->exp1_value_->type_ == ValueType::INT)
      CHECK(value->type_ == ValueType::INT)
      value = to_value(value_to<Int>(add2_cont->exp1_value_) +
                       value_to<Int>(value));
      continue;
    }
    if (cont->type_ == ContinuationType::OPRATOR_CONT) {
      OperatorCont *operator_cont = get<OperatorCont>(cont);
      // value is proc_value
      auto operand_cont = make_oprand_cont(value);
      cur_stack->push(operand_cont);
      auto exp_cont =
          make_exp_cont(operator_cont->arg_exp_, operator_cont->the_time_env_);
      cur_stack->push(exp_cont);
      continue;
    }
    if (cont->type_ == ContinuationType::OPRAND_CONT) {
      OperandCont *operand_cont = get<OperandCont>(cont);
      auto proc_value = operand_cont->proc_value_;
      call_procedure(proc_value, value, cur_stack);
      continue;
    }
    if (cont->type_ == ContinuationType::PRINT_CONT) {
      PrintCont *print_cont = get<PrintCont>(cont);
      std::cout << "print: " << to_string(value) << '\n';
      continue;
    }
    if (cont->type_ == ContinuationType::BEGIN_CONT) {
      BeginCont *begin_cont = get<BeginCont>(cont);
      if (begin_cont->next_exp_idx_ == begin_cont->exps_.size() - 1) {
        auto exp_cont =
            make_exp_cont(begin_cont->exps_[begin_cont->next_exp_idx_],
                          begin_cont->the_time_env_);
        cur_stack->push(exp_cont);
        continue;
      } else {
        int cur_exp_idx = begin_cont->next_exp_idx_;
        begin_cont->next_exp_idx_++;
        cur_stack->push(cont);
        auto exp_cont = make_exp_cont(begin_cont->exps_[cur_exp_idx],
                                      begin_cont->the_time_env_);
        cur_stack->push(exp_cont);
        continue;
      }
    }
    if (cont->type_ == ContinuationType::SPAWN_CONT) {
      SpawnCont *spawn_cont = get<SpawnCont>(cont);
      CHECK(value->type_ == ValueType::PROC ||
            value->type_ == ValueType::REC_PROC)
      auto new_thread_cont_stack = std::make_shared<ContStack>();
      new_thread_cont_stack->push(make_end_subthread_cont());
      call_procedure(value, to_value(28), new_thread_cont_stack);
      ThreadScheduler::get().place_on_ready_queue(new_thread_cont_stack);
      value = to_value(73);
      continue;
    }
    if (cont->type_ == ContinuationType::SET_CONT) {
      SetCont *set_cont = get<SetCont>(cont);
      set_cont->the_time_env_->set(set_cont->var_name_, value);
      value = to_value(44);
      continue;
    }
    if (cont->type_ == ContinuationType::WAIT_CONT) {
      WaitCont *wait_cont = get<WaitCont>(cont);
      auto value = wait_cont->the_time_env_->apply(wait_cont->mutex_name_);
      CHECK(value->type_ == ValueType::MUTEX)
      Mutex mutex = value_to<MutexValue>(value);
      if (mutex.status_ == Mutex::MutextStatus::LOCKED) {
        ThreadScheduler::get().wait(wait_cont->mutex_name_, cur_stack);
        cur_stack = ThreadScheduler::get().get_next_thread_stack();
        continue;
      }
      if (mutex.status_ == Mutex::MutextStatus::UNLOCKED) {
        mutex.status_ = Mutex::MutextStatus::LOCKED;
        wait_cont->the_time_env_->set(wait_cont->mutex_name_, to_value(mutex));
        value = to_value(52);
        continue;
      }
      CHECK_WITH_INFO(false, "got unkown mutex status!!!")
    }
    CHECK_WITH_INFO(false, "got unkown continuation type!!!!" +
                               std::to_string(int(cont->type_)))
  }
  CHECK(ThreadScheduler::get().the_final_answer_ != nullptr)
  return ThreadScheduler::get().the_final_answer_;
}