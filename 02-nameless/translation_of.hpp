#pragma once
#include "check.hpp"
#include "expression.hpp"
#include "nameless_expression.hpp"
#include "static_environment.hpp"
#include "value.hpp"
template <typename T>
T *get(const SPtr<Expression> &exp) {
  return (T *)exp.get();
}

SPtr<nameless::Expression> translation_of(const SPtr<Expression> &exp,
                                          SPtr<StaticEnvironment> &env) {
  if (exp->type_ == ExpressionType::CONST_EXP) {
    auto const_exp = get<ConstExp>(exp);
    return nameless::const_(const_exp->num_);
  }
  if (exp->type_ == ExpressionType::VAR_EXP) {
    const VarExp *var_exp = get<VarExp>(exp);
    return nameless::var(env->apply(var_exp->var_));
  }
  if (exp->type_ == ExpressionType::DIFF_EXP) {
    const DiffExp *diff_exp = (DiffExp *)exp.get();
    SPtr<nameless::Expression> left_exp =
        translation_of(diff_exp->left_exp_, env);
    SPtr<nameless::Expression> right_exp =
        translation_of(diff_exp->right_exp_, env);
    return nameless::diff(left_exp, right_exp);
  }
  if (exp->type_ == ExpressionType::ADD_EXP) {
    const AddExp *add_exp = (AddExp *)exp.get();
    SPtr<nameless::Expression> left_exp =
        translation_of(add_exp->left_exp_, env);
    SPtr<nameless::Expression> right_exp =
        translation_of(add_exp->right_exp_, env);
    return nameless::add(left_exp, right_exp);
  }
  if (exp->type_ == ExpressionType::IS_ZERO_EXP) {
    const IsZeroExp *is_zero_exp = get<IsZeroExp>(exp);
    SPtr<nameless::Expression> exp = translation_of(is_zero_exp->exp_, env);
    return nameless::is_zero(exp);
  }
  if (exp->type_ == ExpressionType::IF_EXP) {
    const IfExp *if_exp = get<IfExp>(exp);
    SPtr<nameless::Expression> condition_exp =
        translation_of(if_exp->condition_exp_, env);
    SPtr<nameless::Expression> true_exp =
        translation_of(if_exp->true_exp_, env);
    SPtr<nameless::Expression> false_exp =
        translation_of(if_exp->false_exp_, env);
    return nameless::if_(condition_exp, true_exp, false_exp);
  }
  if (exp->type_ == ExpressionType::LET_EXP) {
    const LetExp *let_exp = get<LetExp>(exp);
    SPtr<nameless::Expression> var_exp = translation_of(let_exp->exp_, env);
    auto new_env = env->extend(let_exp->var_);
    SPtr<nameless::Expression> body_exp =
        translation_of(let_exp->body_, new_env);
    return nameless::let(var_exp, body_exp);
  }
  if (exp->type_ == ExpressionType::PROC_EXP) {
    const ProcExp *proc_exp = get<ProcExp>(exp);
    auto new_env = env->extend(proc_exp->parameter_);
    auto body_exp = translation_of(proc_exp->body_, new_env);
    return nameless::proc(body_exp);
  }
  if (exp->type_ == ExpressionType::CALL_EXP) {
    const CallExp *call_exp = get<CallExp>(exp);
    auto proc_exp = translation_of(call_exp->proc_generator_, env);
    auto arg_exp = translation_of(call_exp->arg_, env);
    return nameless::call(proc_exp, arg_exp);
  }
  CHECK_WITH_INFO(false, "got unkown expression type!!!!")
  return nullptr;
}