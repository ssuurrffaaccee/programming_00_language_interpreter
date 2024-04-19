#pragma once
#include <sstream>
#include <string>
#include <vector>

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
  BEGIN_EXP,
  SET_EXP,
  SPAWN_EXP,
  YIELD_EXP,
  MUTEX_EXP,
  WAIT_EXP,
  SIGNAL_EXP
};
struct Expression {
  ExpressionType type_;
  virtual ~Expression() {}
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
struct PrintExp : public Expression {
  PrintExp() { type_ = ExpressionType::PRINT_EXP; }
  SPtr<Expression> exp_;
};
SPtr<Expression> print(const SPtr<Expression> &e) {
  PrintExp *exp = new PrintExp{};
  exp->exp_ = e;
  return SPtr<Expression>{(Expression *)exp};
}
struct BeginExp : public Expression {
  BeginExp() { type_ = ExpressionType::BEGIN_EXP; }
  std::vector<SPtr<Expression>> exps_;
};
template <typename... Args> SPtr<Expression> begin(const Args &...args) {
  BeginExp *exp = new BeginExp{};
  (exp->exps_.push_back(args), ...);
  return SPtr<Expression>{(Expression *)exp};
}
struct SpawnExp : public Expression {
  SpawnExp() { type_ = ExpressionType::SPAWN_EXP; }
  SPtr<Expression> exp_;
};
SPtr<Expression> spawn(const SPtr<Expression> &e) {
  SpawnExp *exp = new SpawnExp{};
  exp->exp_ = e;
  return SPtr<Expression>{(Expression *)exp};
}

struct YieldExp : public Expression {
  YieldExp() { type_ = ExpressionType::YIELD_EXP; }
};
SPtr<Expression> yield() {
  YieldExp *exp = new YieldExp{};
  return SPtr<Expression>{(Expression *)exp};
}

struct MutexExp : public Expression {
  MutexExp() { type_ = ExpressionType::MUTEX_EXP; }
};
SPtr<Expression> mutex() {
  MutexExp *exp = new MutexExp{};
  return SPtr<Expression>{(Expression *)exp};
}

struct WaitExp : public Expression {
  WaitExp() { type_ = ExpressionType::WAIT_EXP; }
  std::string mutex_name_;
};
SPtr<Expression> wait(const std::string mutex_name) {
  WaitExp *exp = new WaitExp{};
  exp->mutex_name_ = mutex_name;
  return SPtr<Expression>{(Expression *)exp};
}

struct SignalExp : public Expression {
  SignalExp() { type_ = ExpressionType::SIGNAL_EXP; }
  std::string mutex_name_;
};
SPtr<Expression> signal(const std::string mutex_name) {
  SignalExp *exp = new SignalExp{};
  exp->mutex_name_ = mutex_name;
  return SPtr<Expression>{(Expression *)exp};
}
struct SetExp : public Expression {
  SetExp() { type_ = ExpressionType::SET_EXP; }
  std::string var_name_;
  SPtr<Expression> exp_;
};
SPtr<Expression> set(const std::string &var_name, const SPtr<Expression> &exp) {
  SetExp *set_exp = new SetExp{};
  set_exp->var_name_ = var_name;
  set_exp->exp_ = exp;
  return SPtr<Expression>{(Expression *)set_exp};
}
template <typename T> T *get(const SPtr<Expression> &exp) {
  return (T *)exp.get();
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
  if (exp->type_ == ExpressionType::BEGIN_EXP) {
    const BeginExp *begin_exp = (BeginExp *)(exp);
    ss << "begin(" << to_string(begin_exp->exps_.begin()->get());
    for (auto iter = begin_exp->exps_.begin() + 1;
         iter != begin_exp->exps_.end(); iter++) {
      ss << "," << to_string(iter->get());
    }
    ss << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::SPAWN_EXP) {
    const SpawnExp *spawn_exp = (SpawnExp *)(exp);
    ss << "spawn(" << to_string(spawn_exp->exp_.get()) << ")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::YIELD_EXP) {
    const YieldExp *yield_exp = (YieldExp *)(exp);
    ss << "yield()";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::MUTEX_EXP) {
    const MutexExp *mutex_exp = (MutexExp *)(exp);
    ss << "mutex()";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::WAIT_EXP) {
    const WaitExp *wait_exp = (WaitExp *)(exp);
    ss << "wait(\"" << wait_exp->mutex_name_ << "\")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::SIGNAL_EXP) {
    const SignalExp *singal_exp = (SignalExp *)(exp);
    ss << "signal(\"" << singal_exp->mutex_name_ << "\")";
    return ss.str();
  }
  if (exp->type_ == ExpressionType::SET_EXP) {
    const SetExp *set_exp = (SetExp *)(exp);
    ss << "set(\"" << set_exp->var_name_ << "\","
       << to_string(set_exp->exp_.get()) << ")";
    return ss.str();
  }
  CHECK_WITH_INFO(false, "got unkown expression type!!!!" +
                             std::to_string(int(exp->type_)));
  return "";
}