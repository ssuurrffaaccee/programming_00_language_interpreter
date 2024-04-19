#pragma once
#include <string>

#include "check.hpp"
#include "sptr.hpp"
#include <vector>
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
  CLASS_DEF_EXP,
  NEW_OBJECT_EXP,
  SET_EXP,
  SEND_EXP,
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
  std::vector<std::string> parameters_;
  SPtr<Expression> body_;
};
SPtr<Expression> proc(const std::vector<std::string> &arg_names,
                      const SPtr<Expression> &body) {
  ProcExp *exp = new ProcExp{};
  exp->parameters_ = arg_names;
  exp->body_ = body;
  return SPtr<Expression>{(Expression *)exp};
}

struct CallExp : public Expression {
  CallExp() { type_ = ExpressionType::CALL_EXP; }
  SPtr<Expression> proc_generator_;
  std::vector<SPtr<Expression>> args_;
};
template <typename... Args>
SPtr<Expression> call(const SPtr<Expression> &proc_generator,
                      const std::vector<SPtr<Expression>> &args) {
  CallExp *exp = new CallExp{};
  exp->proc_generator_ = proc_generator;
  exp->args_ = args;
  return SPtr<Expression>{(Expression *)exp};
}

struct LetRecExp : public Expression {
  LetRecExp() { type_ = ExpressionType::LETREC_EXP; }
  std::string proc_name_;
  std::vector<std::string> parameters_;
  SPtr<Expression> proc_body_;
  SPtr<Expression> apply_body_;
};

SPtr<Expression> let_rec(const std::string &proc_name,
                         const std::vector<std::string> &args,
                         const SPtr<Expression> &proc_body,
                         const SPtr<Expression> &apply_body) {
  LetRecExp *exp = new LetRecExp{};
  exp->proc_name_ = proc_name;
  exp->parameters_ = args;
  exp->proc_body_ = proc_body;
  exp->apply_body_ = apply_body;
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
struct PrintExp : public Expression {
  PrintExp() { type_ = ExpressionType::PRINT_EXP; }
  SPtr<Expression> exp_;
};
SPtr<Expression> print(const SPtr<Expression> &e) {
  PrintExp *exp = new PrintExp{};
  exp->exp_ = e;
  return SPtr<Expression>{(Expression *)exp};
}
// class
namespace class_helper {
enum class ClassMemberType { UNKOWN, FIELD, METHOD, SUPER };
struct ClassMember {
  ClassMemberType type_;
};
struct SuperMember : public ClassMember {
  SuperMember() { type_ = ClassMemberType::SUPER; }
  std::string super_name_;
};
struct FieldMember : public ClassMember {
  FieldMember() { type_ = ClassMemberType::FIELD; }
  std::string name_;
};
struct MethodMember : public ClassMember {
  MethodMember() { type_ = ClassMemberType::METHOD; }
  std::string name_;
  std::vector<std::string> var_names_;
  SPtr<Expression> body_;
};
} // namespace class_helper
SPtr<class_helper::ClassMember> field(const std::string &name) {
  using namespace class_helper;
  FieldMember *member = new FieldMember{};
  member->name_ = name;
  return SPtr<ClassMember>{(ClassMember *)member};
}
SPtr<class_helper::ClassMember> super(const std::string &name) {
  using namespace class_helper;
  SuperMember *member = new SuperMember{};
  member->super_name_ = name;
  return SPtr<ClassMember>{(ClassMember *)member};
}
SPtr<class_helper::ClassMember> method(const std::string &name,
                                       const SPtr<Expression> &body) {
  using namespace class_helper;
  MethodMember *member = new MethodMember{};
  member->name_ = name;
  member->body_ = body;
  return SPtr<ClassMember>{(ClassMember *)member};
}
SPtr<class_helper::ClassMember>
method(const std::string &name, const std::vector<std::string> &var_names,
       const SPtr<Expression> &body) {
  using namespace class_helper;
  MethodMember *member = new MethodMember{};
  member->name_ = name;
  member->var_names_ = var_names;
  member->body_ = body;
  return SPtr<ClassMember>{(ClassMember *)member};
}
struct ClassDefinExp : public Expression {
  struct Method {
    std::string name_;
    std::vector<std::string> var_names_;
    SPtr<Expression> body_;
  };
  ClassDefinExp() { type_ = ExpressionType::CLASS_DEF_EXP; }
  std::string super_class_name_;
  std::vector<std::string> fields_;
  std::vector<Method> methods_;
};
template <typename... Args> SPtr<Expression> class_(const Args &...args) {
  ClassDefinExp *exp = new ClassDefinExp{};
  using namespace class_helper;
  std::vector<SPtr<ClassMember>> members;
  (members.push_back(args), ...);
  if (members.empty()) {
    exp->super_class_name_ = "object";
    return SPtr<Expression>{(Expression *)exp};
  }
  auto iter = members.begin();
  if (iter->get()->type_ != ClassMemberType::SUPER) {
    exp->super_class_name_ = "object";
  } else {
    auto super = (SuperMember *)(iter->get());
    exp->super_class_name_ = super->super_name_;
    iter++;
  }
  for (; iter != members.end(); iter++) {
    CHECK_WITH_INFO(iter->get()->type_ != ClassMemberType::SUPER,
                    "multi super or super not in first place in class define")
    if (iter->get()->type_ == ClassMemberType::FIELD) {
      FieldMember *field = (FieldMember *)iter->get();
      exp->fields_.push_back(field->name_);
      continue;
    }
    if (iter->get()->type_ == ClassMemberType::METHOD) {
      MethodMember *method = (MethodMember *)iter->get();
      exp->methods_.push_back(ClassDefinExp::Method{
          method->name_, method->var_names_, method->body_});
      continue;
    }
    CHECK_WITH_INFO(false, "got unkown class member type!!!!")
  }
  return SPtr<Expression>{(Expression *)exp};
}

struct NewObjectExp : public Expression {
  NewObjectExp() { type_ = ExpressionType::NEW_OBJECT_EXP; }
  std::string class_name_;
};
SPtr<Expression> new_(const std::string &class_name) {
  NewObjectExp *exp = new NewObjectExp{};
  exp->class_name_ = class_name;
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
struct SendExp : public Expression {
  SendExp() { type_ = ExpressionType::SEND_EXP; }
  std::string object_name_;
  std::string method_name_;
  std::vector<SPtr<Expression>> args_;
};
SPtr<Expression> send(const std::string object_name,
                      const std::string method_name_,
                      const std::vector<SPtr<Expression>> &args) {
  SendExp *exp = new SendExp{};
  exp->object_name_ = object_name;
  exp->method_name_ = method_name_;
  exp->args_ = args;
  return SPtr<Expression>{(Expression *)exp};
}
template <typename T> T *get(const SPtr<Expression> &exp) {
  return (T *)exp.get();
}