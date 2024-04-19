#pragma once
#include <sstream>
#include <string>

#include "check.hpp"
#include "sptr.hpp"
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
  LETREC_EXP,
  PRINT_EXP,
  TRY_EXP,
  RAISE_EXP,
  STR_EXP,
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

struct LetRecExp : public Expression {
  LetRecExp() { type_ = ExpressionType::LETREC_EXP; }
  std::string proc_name_;
  std::string parameter_;
  SPtr<Expression> proc_body_;
  SPtr<Expression> apply_body_;
};

SPtr<Expression> let_rec(const std::string &proc_name,
                         const std::string &parameter,
                         const SPtr<Expression> &proc_body,
                         const SPtr<Expression> &apply_body) {
  LetRecExp *exp = new LetRecExp{};
  exp->proc_name_ = proc_name;
  exp->parameter_ = parameter;
  exp->proc_body_ = proc_body;
  exp->apply_body_ = apply_body;
  return SPtr<Expression>{(Expression *)exp};
}
template <typename T> T *get(const SPtr<Expression> &exp) {
  return (T *)exp.get();
}
struct PrintExp : public Expression {
  PrintExp() { type_ = ExpressionType::PRINT_EXP; }
  SPtr<Expression> exp_;
};
SPtr<Expression> print(const SPtr<Expression> &e) {
  PrintExp *exp = new PrintExp{};
  exp->exp_ = e;
  return SPtr<Expression>{(Expression *)exp};
}
struct TryExp : public Expression {
  TryExp() { type_ = ExpressionType::TRY_EXP; }
  SPtr<Expression> exp_;
  std::string exception_var_;
  SPtr<Expression> handler_;
};
SPtr<Expression> try_(const SPtr<Expression> &e,
                      const std::string &exception_var,
                      const SPtr<Expression> &handler_) {
  TryExp *exp = new TryExp{};
  exp->exp_ = e;
  exp->exception_var_ = exception_var;
  exp->handler_ = handler_;
  return SPtr<Expression>{(Expression *)exp};
}
struct RaiseExp : public Expression {
  RaiseExp() { type_ = ExpressionType::RAISE_EXP; }
  SPtr<Expression> exp_;
};
SPtr<Expression> raise(const SPtr<Expression> &e) {
  RaiseExp *exp = new RaiseExp{};
  exp->exp_ = e;
  return SPtr<Expression>{(Expression *)exp};
}

struct StrExp : public Expression {
  StrExp() { type_ = ExpressionType::STR_EXP; }
  std::string str_;
};
SPtr<Expression> str(const std::string &s) {
  StrExp *exp = new StrExp{};
  exp->str_ = s;
  return SPtr<Expression>{(Expression *)exp};
}

std::string to_string(Expression *exp) {
  CHECK(exp != nullptr)
  std::stringstream ss;
  if (exp->type_ == ExpressionType::CONST_EXP) {
    auto const_exp = (ConstExp *)exp;
    ss << "const_(" << const_exp->num_ << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::VAR_EXP) {
    const VarExp *var_exp = (VarExp *)exp;
    ss << "var(\"" << var_exp->var_ << "\")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::DIFF_EXP) {
    const DiffExp *diff_exp = (DiffExp *)exp;
    ss << "diff(" << to_string(diff_exp->left_exp_.get()) << ","
       << to_string(diff_exp->right_exp_.get()) << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::ADD_EXP) {
    const AddExp *add_exp = (AddExp *)exp;
    ss << "add(" << to_string(add_exp->left_exp_.get()) << ","
       << to_string(add_exp->right_exp_.get()) << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::IS_ZERO_EXP) {
    const IsZeroExp *is_zero_exp = (IsZeroExp *)exp;
    ss << "is_zero(" << to_string(is_zero_exp->exp_.get()) << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::IF_EXP) {
    const IfExp *if_exp = (IfExp *)exp;
    ss << "if(" << to_string(if_exp->condition_exp_.get()) << ","
       << to_string(if_exp->true_exp_.get()) << ","
       << to_string(if_exp->false_exp_.get()) << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::LET_EXP) {
    const LetExp *let_exp = (LetExp *)exp;
    ss << "let(\"" << let_exp->var_ << "\"," << to_string(let_exp->exp_.get())
       << "," << to_string(let_exp->body_.get()) << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::PROC_EXP) {
    const ProcExp *proc_exp = (ProcExp *)exp;
    ss << "proc(\"" << proc_exp->parameter_ << "\","
       << to_string(proc_exp->body_.get()) << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::CALL_EXP) {
    const CallExp *call_exp = (CallExp *)exp;
    ss << "call(" << to_string(call_exp->proc_generator_.get()) << ","
       << to_string(call_exp->arg_.get()) << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::LETREC_EXP) {
    const LetRecExp *let_rec_exp = (LetRecExp *)(exp);
    ss << "let_rec(\"" << let_rec_exp->proc_name_ << "\",\""
       << let_rec_exp->parameter_ << "\","
       << to_string(let_rec_exp->proc_body_.get()) << ","
       << to_string(let_rec_exp->apply_body_.get()) << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::PRINT_EXP) {
    const PrintExp *print_exp = (PrintExp *)(exp);
    ss << "print(" << to_string(print_exp->exp_.get()) << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::TRY_EXP) {
    const TryExp *try_exp = (TryExp *)(exp);
    ss << "try_(" << to_string(try_exp->exp_.get()) << ",\""
       << try_exp->exception_var_ << "\"," << to_string(try_exp->handler_.get())
       << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::RAISE_EXP) {
    const RaiseExp *raise_exp = (RaiseExp *)(exp);
    ss << "raise(" << to_string(raise_exp->exp_.get()) << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::STR_EXP) {
    const StrExp *str_exp = (StrExp *)(exp);
    ss << "str(\"" << str_exp->str_ << "\")";
    return ss.str();
  }
  CHECK_WITH_INFO(false, "got unkown expression type!!!!")
  return "";
}