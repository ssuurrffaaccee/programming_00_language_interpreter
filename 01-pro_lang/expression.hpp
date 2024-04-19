#pragma once
#include "sptr.hpp"
#include <string>
// Expression
enum class ExpressionType {
  UNKOWN_EXP,
  CONST_EXP,
  VAR_EXP,
  DIFF_EXP,
  ADD_EXP,
  IS_ZERO_EXP,
  IF_EXP,
  LET_EXP,
  CALL_EXP,
  PROC_EXP,
};
struct Expression {
  ExpressionType type_;
  virtual ~Expression(){}
};

struct ConstExp : public Expression {
  ConstExp() { type_ = ExpressionType::CONST_EXP; }
  int num_;
};
SPtr<Expression> const_(int v) {
  ConstExp *exp = new ConstExp{};
  exp->num_ = v;
  return SPtr<Expression>{(Expression *)exp};
}

struct VarExp : public Expression {
  VarExp() { type_ = ExpressionType::VAR_EXP; }
  std::string var_;
};
SPtr<Expression> var(const std::string &name) {
  VarExp *exp = new VarExp{};
  exp->var_ = name;
  return SPtr<Expression>{(Expression *)exp};
}

struct DiffExp : public Expression {
  DiffExp() { type_ = ExpressionType::DIFF_EXP; }
  SPtr<Expression> left_exp_;
  SPtr<Expression> right_exp_;
};
SPtr<Expression> diff(const SPtr<Expression> &l, const SPtr<Expression> &r) {
  DiffExp *exp = new DiffExp{};
  exp->left_exp_ = l;
  exp->right_exp_ = r;
  return SPtr<Expression>{(Expression *)exp};
}
struct AddExp : public Expression {
  AddExp() { type_ = ExpressionType::ADD_EXP; }
  SPtr<Expression> left_exp_;
  SPtr<Expression> right_exp_;
};
SPtr<Expression> add(const SPtr<Expression> &l, const SPtr<Expression> &r) {
  AddExp *exp = new AddExp{};
  exp->left_exp_ = l;
  exp->right_exp_ = r;
  return SPtr<Expression>{(Expression *)exp};
}

struct IsZeroExp : public Expression {
  IsZeroExp() { type_ = ExpressionType::IS_ZERO_EXP; }
  SPtr<Expression> exp_;
};
SPtr<Expression> is_zero(const SPtr<Expression> &e) {
  IsZeroExp *exp = new IsZeroExp{};
  exp->exp_ = e;
  return SPtr<Expression>{(Expression *)exp};
}

struct IfExp : public Expression {
  IfExp() { type_ = ExpressionType::IF_EXP; }
  SPtr<Expression> condition_exp_;
  SPtr<Expression> true_exp_;
  SPtr<Expression> false_exp_;
};
SPtr<Expression> if_(const SPtr<Expression> &condition_exp,
                     const SPtr<Expression> &true_exp,
                     const SPtr<Expression> &false_exp) {
  IfExp *exp = new IfExp{};
  exp->condition_exp_ = condition_exp;
  exp->true_exp_ = true_exp;
  exp->false_exp_ = false_exp;
  return SPtr<Expression>{(Expression *)exp};
}
struct LetExp : public Expression {
  LetExp() { type_ = ExpressionType::LET_EXP; }
  std::string var_;
  SPtr<Expression> exp_;
  SPtr<Expression> body_;
};
SPtr<Expression> let(const std::string &var, const SPtr<Expression> &exp,
                     const SPtr<Expression> &body) {
  LetExp *let_exp = new LetExp{};
  let_exp->var_ = var;
  let_exp->exp_ = exp;
  let_exp->body_ = body;
  return SPtr<Expression>{(Expression *)let_exp};
}

struct ProcExp : public Expression {
  ProcExp() { type_ = ExpressionType::PROC_EXP; }
  std::string parameter_;
  SPtr<Expression> body_;
};
SPtr<Expression> proc(const std::string &parameter,
                      const SPtr<Expression> &body) {
  ProcExp *exp = new ProcExp{};
  exp->parameter_ = parameter;
  exp->body_ = body;
  return SPtr<Expression>{(Expression *)exp};
}

struct CallExp : public Expression {
  CallExp() { type_ = ExpressionType::CALL_EXP; }
  SPtr<Expression> proc_generator_;
  SPtr<Expression> arg_;
};
SPtr<Expression> call(const SPtr<Expression> &proc_generator,
                      const SPtr<Expression> &arg) {
  CallExp *exp = new CallExp{};
  exp->proc_generator_ = proc_generator;
  exp->arg_ = arg;
  return SPtr<Expression>{(Expression *)exp};
}