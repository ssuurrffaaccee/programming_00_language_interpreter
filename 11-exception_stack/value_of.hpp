#pragma once
#include "check.hpp"
#include "continuation.hpp"
#include "environment.hpp"
#include "expression.hpp"
#include "value.hpp"
#include <stack>

using ContStack = std::stack<SPtr<Continuation>>;
SPtr<Value> value_of_cont(ContStack &cont_stack) {
  SPtr<Value> value;
  while (!cont_stack.empty()) {
    auto cont = cont_stack.top();
    cont_stack.pop();
    if (cont->type_ == ContinuationType::EXP_CONT) {
      ExpCont *exp_cont = get<ExpCont>(cont);
      auto exp = exp_cont->exp_;
      auto env = exp_cont->env_;
      if (exp->type_ == ExpressionType::CONST_EXP) {
        auto const_exp = get<ConstExp>(exp);
        value = to_value(const_exp->num_);
        continue;
      }
      if (exp->type_ == ExpressionType::STR_EXP) {
        auto str_exp = get<StrExp>(exp);
        value = to_value(str_exp->str_);
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
        cont_stack.push(diff1_cont);
        auto exp_cont = make_exp_cont(diff_exp->left_exp_, env);
        cont_stack.push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::ADD_EXP) {
        const AddExp *add_exp = get<AddExp>(exp);
        auto add1_cont = make_add1_cont(add_exp->right_exp_, env);
        cont_stack.push(add1_cont);
        auto exp_cont = make_exp_cont(add_exp->left_exp_, env);
        cont_stack.push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::IS_ZERO_EXP) {
        const IsZeroExp *is_zero_exp = get<IsZeroExp>(exp);
        auto zero1_cont = make_zero1_cont();
        cont_stack.push(zero1_cont);
        auto exp_cont = make_exp_cont(is_zero_exp->exp_, env);
        cont_stack.push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::IF_EXP) {
        const IfExp *if_exp = get<IfExp>(exp);
        auto if_test_cont =
            make_if_test_cont(if_exp->true_exp_, if_exp->false_exp_, env);
        cont_stack.push(if_test_cont);
        auto exp_cont = make_exp_cont(if_exp->condition_exp_, env);
        cont_stack.push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::LET_EXP) {
        const LetExp *let_exp = get<LetExp>(exp);
        auto let_cont = make_let_cont(let_exp->var_, let_exp->body_, env);
        cont_stack.push(let_cont);
        auto exp_cont = make_exp_cont(let_exp->exp_, env);
        cont_stack.push(exp_cont);
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
        cont_stack.push(operator_cont);
        auto exp_cont = make_exp_cont(call_exp->proc_generator_, env);
        cont_stack.push(exp_cont);
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
        cont_stack.push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::PRINT_EXP) {
        const PrintExp *print_exp = get<PrintExp>(exp);
        auto print_cont = make_print_cont();
        cont_stack.push(print_cont);
        auto exp_cont = make_exp_cont(print_exp->exp_, env);
        cont_stack.push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::TRY_EXP) {
        const TryExp *try_exp = get<TryExp>(exp);
        auto try_cont =
            make_try_cont(try_exp->exception_var_, try_exp->handler_, env);
        cont_stack.push(try_cont);
        auto exp_cont = make_exp_cont(try_exp->exp_, env);
        cont_stack.push(exp_cont);
        continue;
      }
      if (exp->type_ == ExpressionType::RAISE_EXP) {
        const RaiseExp *raise_exp = get<RaiseExp>(exp);
        auto raise_cont = make_raise1_cont(cont);
        cont_stack.push(raise_cont);
        auto exp_cont = make_exp_cont(raise_exp->exp_, env);
        cont_stack.push(exp_cont);
        continue;
      }
      CHECK_WITH_INFO(false, "got unkown expression type!!!!" +
                                 std::to_string(int(exp->type_)))
    }
    // if (cont->type_ == ContinuationType::END_CONT) {
    //   continue;
    // }
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
      cont_stack.push(exp_cont);
      continue;
    }
    if (cont->type_ == ContinuationType::IF_TEST_CONT) {
      IfTestCont *if_test_cont = get<IfTestCont>(cont);
      CHECK(value->type_ == ValueType::BOOL)
      if (value_to<Bool>(value)) {
        auto exp_cont =
            make_exp_cont(if_test_cont->true_exp_, if_test_cont->the_time_env_);
        cont_stack.push(exp_cont);
        continue;
      } else {
        auto exp_cont = make_exp_cont(if_test_cont->false_exp_,
                                      if_test_cont->the_time_env_);
        cont_stack.push(exp_cont);
        continue;
      }
    }
    if (cont->type_ == ContinuationType::DIFF1_CONT) {
      Diff1Cont *diff1_cont = get<Diff1Cont>(cont);
      // value is thre result of exp1
      // diff is not complete,so we store the value
      auto diff2_cont = make_diff2_cont(value);
      cont_stack.push(diff2_cont);
      auto exp_cont =
          make_exp_cont(diff1_cont->exp2_, diff1_cont->the_time_env_);
      cont_stack.push(exp_cont);
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
      cont_stack.push(add2_cont);
      auto exp_cont = make_exp_cont(add1_cont->exp2_, add1_cont->the_time_env_);
      cont_stack.push(exp_cont);
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
      cont_stack.push(operand_cont);
      auto exp_cont =
          make_exp_cont(operator_cont->arg_exp_, operator_cont->the_time_env_);
      cont_stack.push(exp_cont);
      continue;
    }
    if (cont->type_ == ContinuationType::OPRAND_CONT) {
      OperandCont *operand_cont = get<OperandCont>(cont);
      auto proc_value = operand_cont->proc_value_;
      // value is argument value
      if (proc_value->type_ == ValueType::PROC) {
        auto proc = value_to<ProcedureValue>(proc_value);
        CHECK(proc.env_ != nullptr)
        auto new_env = proc.env_->extend(proc.parameter_, value);
        auto exp_cont = make_exp_cont(proc.body_, new_env);
        cont_stack.push(exp_cont);
        continue;
      }
      if (proc_value->type_ == ValueType::REC_PROC) {
        auto proc = value_to<RecursiveProcedureValue>(proc_value);
        CHECK(proc.env_ != nullptr)
        auto new_env_0 = proc.env_->extend(proc.proc_name_, proc_value);
        auto new_env_1 = new_env_0->extend(proc.parameter_, value);
        auto exp_cont = make_exp_cont(proc.proc_body_, new_env_1);
        cont_stack.push(exp_cont);
        continue;
      }
    }
    if (cont->type_ == ContinuationType::PRINT_CONT) {
      PrintCont *print_cont = get<PrintCont>(cont);
      std::cout << "print: " << to_string(value) << '\n';
      continue;
    }
    if (cont->type_ == ContinuationType::TRY_CONT) {
      TryCont *try_cont = get<TryCont>(cont);
      continue;
    }
    if (cont->type_ == ContinuationType::RAISE1_CONT) {
      Raise1Cont *raise_cont = get<Raise1Cont>(cont);
      while (!cont_stack.empty()) {
        auto cont = cont_stack.top();
        cont_stack.pop();
        if (cont->type_ == ContinuationType::TRY_CONT) {
          TryCont *try_cont = get<TryCont>(cont);
          auto new_env = try_cont->the_time_env_->extend(try_cont->var_, value);
          auto exp_cont = make_exp_cont(try_cont->handler_, new_env);
          cont_stack.push(exp_cont);
          break;
        }
      }
      if (cont_stack.empty()) {
        CHECK_WITH_INFO(false, "got unhandled exception:" + to_string(value));
      }
      continue;
    }
    CHECK_WITH_INFO(false, "got unkown continuation type!!!!")
  }
  CHECK(value != nullptr)
  return value;
}