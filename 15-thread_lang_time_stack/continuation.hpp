#pragma once
#include <string>

#include "environment.hpp"
#include "expression.hpp"
#include "sptr.hpp"
#include "value.hpp"
// Continuation
enum class ContinuationType {
  UNKOWN_CONT,
  END_CONT,
  ZERO1_CONT,
  LET_CONT,
  IF_TEST_CONT,
  DIFF1_CONT,
  DIFF2_CONT,
  ADD1_CONT,
  ADD2_CONT,
  OPRATOR_CONT,
  OPRAND_CONT,
  PRINT_CONT,
  EXP_CONT,
  BEGIN_CONT,
  SPAWN_CONT,
  SET_CONT,
  WAIT_CONT,
  END_SUBTHREAD_CONT
};
struct Continuation {
  ContinuationType type_;
};

struct EndCont : public Continuation {
  EndCont() { type_ = ContinuationType::END_CONT; }
};
SPtr<Continuation> make_end_cont() {
  EndCont *this_cont = new EndCont{};
  return SPtr<Continuation>{(Continuation *)this_cont};
}

struct Zero1Cont : public Continuation {
  Zero1Cont() { type_ = ContinuationType::ZERO1_CONT; }
  // SPtr<Continuation> cont_;
};
SPtr<Continuation> make_zero1_cont() {
  Zero1Cont *this_cont = new Zero1Cont{};
  return SPtr<Continuation>{(Continuation *)this_cont};
}
struct LetCont : public Continuation {
  LetCont() { type_ = ContinuationType::LET_CONT; }
  std::string var_;
  SPtr<Expression> body_;
  SPtr<Environment> the_time_env_;
  // SPtr<Continuation> cont_;
};
SPtr<Continuation> make_let_cont(const std::string &var,
                                 const SPtr<Expression> &body,
                                 const SPtr<Environment> &the_time_env) {
  LetCont *this_cont = new LetCont{};
  this_cont->var_ = var;
  this_cont->body_ = body;
  this_cont->the_time_env_ = the_time_env;
  return SPtr<Continuation>{(Continuation *)this_cont};
}
struct IfTestCont : public Continuation {
  IfTestCont() { type_ = ContinuationType::IF_TEST_CONT; }
  SPtr<Expression> true_exp_;
  SPtr<Expression> false_exp_;
  SPtr<Environment> the_time_env_;
};
SPtr<Continuation> make_if_test_cont(const SPtr<Expression> &true_exp,
                                     const SPtr<Expression> &false_exp,
                                     const SPtr<Environment> &the_time_env) {
  IfTestCont *this_cont = new IfTestCont{};
  this_cont->true_exp_ = true_exp;
  this_cont->false_exp_ = false_exp;
  this_cont->the_time_env_ = the_time_env;
  return SPtr<Continuation>{(Continuation *)this_cont};
}

struct Diff1Cont : public Continuation {
  Diff1Cont() { type_ = ContinuationType::DIFF1_CONT; }
  SPtr<Expression> exp2_;
  SPtr<Environment> the_time_env_;
};
SPtr<Continuation> make_diff1_cont(const SPtr<Expression> &exp2,
                                   const SPtr<Environment> &the_time_env) {
  Diff1Cont *this_cont = new Diff1Cont{};
  this_cont->exp2_ = exp2;
  this_cont->the_time_env_ = the_time_env;
  return SPtr<Continuation>{(Continuation *)this_cont};
}

struct Diff2Cont : public Continuation {
  Diff2Cont() { type_ = ContinuationType::DIFF2_CONT; }
  SPtr<Value> exp1_value_;
};
SPtr<Continuation> make_diff2_cont(const SPtr<Value> &exp1_value) {
  Diff2Cont *this_cont = new Diff2Cont{};
  this_cont->exp1_value_ = exp1_value;
  return SPtr<Continuation>{(Continuation *)this_cont};
}

struct Add1Cont : public Continuation {
  Add1Cont() { type_ = ContinuationType::ADD1_CONT; }
  SPtr<Expression> exp2_;
  SPtr<Environment> the_time_env_;
};
SPtr<Continuation> make_add1_cont(const SPtr<Expression> &exp2,
                                  const SPtr<Environment> &the_time_env) {
  Add1Cont *this_cont = new Add1Cont{};
  this_cont->exp2_ = exp2;
  this_cont->the_time_env_ = the_time_env;
  return SPtr<Continuation>{(Continuation *)this_cont};
}

struct Add2Cont : public Continuation {
  Add2Cont() { type_ = ContinuationType::ADD2_CONT; }
  SPtr<Value> exp1_value_;
};

SPtr<Continuation> make_add2_cont(const SPtr<Value> &exp1_value) {
  Add2Cont *this_cont = new Add2Cont{};
  this_cont->exp1_value_ = exp1_value;
  return SPtr<Continuation>{(Continuation *)this_cont};
}

struct OperatorCont : public Continuation {
  OperatorCont() { type_ = ContinuationType::OPRATOR_CONT; }
  SPtr<Expression> arg_exp_;
  SPtr<Environment> the_time_env_;
};
SPtr<Continuation> make_operator_cont(const SPtr<Expression> &arg_exp,
                                      const SPtr<Environment> &the_time_env) {
  OperatorCont *this_cont = new OperatorCont{};
  this_cont->arg_exp_ = arg_exp;
  this_cont->the_time_env_ = the_time_env;
  return SPtr<Continuation>{(Continuation *)this_cont};
}

struct OperandCont : public Continuation {
  OperandCont() { type_ = ContinuationType::OPRAND_CONT; }
  SPtr<Value> proc_value_;
};
SPtr<Continuation> make_oprand_cont(const SPtr<Value> &proc_value) {
  OperandCont *this_cont = new OperandCont{};
  this_cont->proc_value_ = proc_value;
  return SPtr<Continuation>{(Continuation *)this_cont};
}
struct PrintCont : public Continuation {
  PrintCont() { type_ = ContinuationType::PRINT_CONT; }
};
SPtr<Continuation> make_print_cont() {
  PrintCont *this_cont = new PrintCont{};
  return SPtr<Continuation>{(Continuation *)this_cont};
}

struct ExpCont : public Continuation {
  ExpCont() { type_ = ContinuationType::EXP_CONT; }
  SPtr<Expression> exp_;
  SPtr<Environment> env_;
};
SPtr<Continuation> make_exp_cont(const SPtr<Expression> &exp,
                                 const SPtr<Environment> &env) {
  ExpCont *this_cont = new ExpCont{};
  this_cont->exp_ = exp;
  this_cont->env_ = env;
  return SPtr<Continuation>{(Continuation *)this_cont};
}
struct BeginCont : public Continuation {
  BeginCont() { type_ = ContinuationType::BEGIN_CONT; }
  std::vector<SPtr<Expression>> exps_;
  SPtr<Environment> the_time_env_;
  int next_exp_idx_;
};
SPtr<Continuation> make_begin_cont(const std::vector<SPtr<Expression>> &exps,
                                   const int next_exp_idx,
                                   const SPtr<Environment> &the_time_env) {
  BeginCont *this_cont = new BeginCont{};
  this_cont->exps_ = exps;
  this_cont->next_exp_idx_ = next_exp_idx;
  this_cont->the_time_env_ = the_time_env;
  return SPtr<Continuation>{(Continuation *)this_cont};
}

struct SpawnCont : public Continuation {
  SpawnCont() { type_ = ContinuationType::SPAWN_CONT; }
};
SPtr<Continuation> make_spawn_cont() {
  SpawnCont *this_cont = new SpawnCont{};
  return SPtr<Continuation>{(Continuation *)this_cont};
}

struct EndSubthreadCont : public Continuation {
  EndSubthreadCont() { type_ = ContinuationType::END_SUBTHREAD_CONT; }
};
SPtr<Continuation> make_end_subthread_cont() {
  EndSubthreadCont *this_cont = new EndSubthreadCont{};
  return SPtr<Continuation>{(Continuation *)this_cont};
}
struct SetCont : public Continuation {
  SetCont() { type_ = ContinuationType::SET_CONT; }
  std::string var_name_;
  SPtr<Environment> the_time_env_;
};
SPtr<Continuation> make_set_cont(const std::string &var_name, const SPtr<Environment> &the_time_env) {
  SetCont *this_cont = new SetCont{};
  this_cont->var_name_ = var_name;
  this_cont->the_time_env_ = the_time_env;
  return SPtr<Continuation>{(Continuation *)this_cont};
}
struct WaitCont : public Continuation {
  WaitCont() { type_ = ContinuationType::WAIT_CONT; }
  std::string mutex_name_;
  SPtr<Environment> the_time_env_;
};
SPtr<Continuation> make_wait_cont(
                                   const std::string& mutex_name,
                                   const SPtr<Environment> &the_time_env) {
  WaitCont *this_cont = new WaitCont{};
  this_cont->mutex_name_ = mutex_name;
  this_cont->the_time_env_ = the_time_env;
  return SPtr<Continuation>{(Continuation *)this_cont};
}

template <typename T> T *get(const SPtr<Continuation> &cont) {
  return (T *)cont.get();
}

// std::string to_string(const SPtr<Continuation> &cont) {
//   std::stringstream ss;
//   auto cont_ptr = cont.get();
// #define PRINT_OUT_CONT(CONT_ENUM_NAME) \
//   if (cont_ptr->type_ == ContinuationType::CONT_ENUM_NAME) { \
//     ss << "--->" << #CONT_ENUM_NAME; \
//     cont_ptr = cont_ptr->cont_.get(); \
//     continue; \
//   }
//   while (cont_ptr != nullptr) {
//     PRINT_OUT_CONT(END_CONT)
//     PRINT_OUT_CONT(ZERO1_CONT)
//     PRINT_OUT_CONT(LET_CONT)
//     PRINT_OUT_CONT(IF_TEST_CONT)
//     PRINT_OUT_CONT(DIFF1_CONT)
//     PRINT_OUT_CONT(DIFF2_CONT)
//     PRINT_OUT_CONT(ADD1_CONT)
//     PRINT_OUT_CONT(ADD2_CONT)
//     PRINT_OUT_CONT(OPRATOR_CONT)
//     PRINT_OUT_CONT(OPRAND_CONT)
//     PRINT_OUT_CONT(PRINT_CONT)
//     CHECK_WITH_INFO(false, "got unkown expression type!!!!")
//     return "";
//   }
//   return ss.str();
// }