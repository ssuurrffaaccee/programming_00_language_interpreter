#pragma once
#include "check.hpp"
#include "environment.hpp"
#include "expression.hpp"
#include "value.hpp"

SPtr<std::unordered_map<std::string, SPtr<Value>>>
new_object_states(const ClassDefine &class_define, SPtr<Environment> &env);
SPtr<Value> value_of(const SPtr<Expression> &exp, SPtr<Environment> &env) {
  if (exp->type_ == ExpressionType::CONST_EXP) {
    auto const_exp = get<ConstExp>(exp);
    return to_value(const_exp->num_);
  }
  if (exp->type_ == ExpressionType::VAR_EXP) {
    const VarExp *var_exp = get<VarExp>(exp);
    return env->apply(var_exp->var_);
  }
  if (exp->type_ == ExpressionType::DIFF_EXP) {
    const DiffExp *diff_exp = get<DiffExp>(exp);
    SPtr<Value> left_value = value_of(diff_exp->left_exp_, env);
    SPtr<Value> right_value = value_of(diff_exp->right_exp_, env);
    CHECK(left_value->type_ == ValueType::INT)
    CHECK(right_value->type_ == ValueType::INT)
    return to_value(value_to<Int>(left_value) - value_to<Int>(right_value));
  }
  if (exp->type_ == ExpressionType::ADD_EXP) {
    const AddExp *add_exp = get<AddExp>(exp);
    SPtr<Value> left_value = value_of(add_exp->left_exp_, env);
    SPtr<Value> right_value = value_of(add_exp->right_exp_, env);
    CHECK(left_value->type_ == ValueType::INT)
    CHECK(right_value->type_ == ValueType::INT)
    return to_value(value_to<Int>(left_value) + value_to<Int>(right_value));
  }
  if (exp->type_ == ExpressionType::IS_ZERO_EXP) {
    const IsZeroExp *is_zero_exp = get<IsZeroExp>(exp);
    SPtr<Value> value = value_of(is_zero_exp->exp_, env);
    if (0 == value_to<Int>(value)) {
      return to_value(true);
    } else {
      return to_value(false);
    }
  }
  if (exp->type_ == ExpressionType::IF_EXP) {
    const IfExp *if_exp = get<IfExp>(exp);
    SPtr<Value> value = value_of(if_exp->condition_exp_, env);
    CHECK(value->type_ == ValueType::BOOL)
    if (value_to<Bool>(value)) {
      return value_of(if_exp->true_exp_, env);
    } else {
      return value_of(if_exp->false_exp_, env);
    }
  }
  if (exp->type_ == ExpressionType::LET_EXP) {
    const LetExp *let_exp = get<LetExp>(exp);
    SPtr<Value> value = value_of(let_exp->exp_, env);
    auto new_env = env->extend(let_exp->var_, value);
    return value_of(let_exp->body_, new_env);
  }
  if (exp->type_ == ExpressionType::PROC_EXP) {
    const ProcExp *proc_exp = get<ProcExp>(exp);
    Procedure procedure;
    procedure.parameters_ = proc_exp->parameters_;
    procedure.body_ = proc_exp->body_;
    procedure.env_ = env;
    return to_value(procedure);
  }
  if (exp->type_ == ExpressionType::CALL_EXP) {
    const CallExp *call_exp = get<CallExp>(exp);
    auto proc_value = value_of(call_exp->proc_generator_, env);
    std::vector<SPtr<Value>> arg_values;
    for (auto &arg_exp : call_exp->args_) {
      auto arg_value = value_of(arg_exp, env);
      arg_values.push_back(arg_value);
    }
    if (proc_value->type_ == ValueType::PROC) {
      auto proc = value_to<ProcedureValue>(proc_value);
      CHECK(proc.env_ != nullptr)
      CHECK(arg_values.size() == proc.parameters_.size());
      SPtr<Environment> env = proc.env_;
      for (int i = 0; i < arg_values.size(); i++) {
        env = env->extend(proc.parameters_[i], arg_values[i]);
      }
      return value_of(proc.body_, env);
    }
    if (proc_value->type_ == ValueType::REC_PROC) {
      auto proc = value_to<RecursiveProcedureValue>(proc_value);
      CHECK(proc.env_ != nullptr)

      SPtr<Environment> env = proc.env_;
      env = env->extend(proc.proc_name_, proc_value);
      CHECK(arg_values.size() == proc.parameters_.size());
      for (int i = 0; i < arg_values.size(); i++) {
        env = env->extend(proc.parameters_[i], arg_values[i]);
      }
      return value_of(proc.proc_body_, env);
    }
    CHECK_WITH_INFO(false, "got unkown proc type");
  }
  if (exp->type_ == ExpressionType::LETREC_EXP) {
    const LetRecExp *let_rec_exp = get<LetRecExp>(exp);
    RecursiveProcedure procedure;
    procedure.proc_name_ = let_rec_exp->proc_name_;
    procedure.parameters_ = let_rec_exp->parameters_;
    procedure.proc_body_ = let_rec_exp->proc_body_;
    procedure.env_ = env;
    auto procedure_value = to_value(procedure);
    auto new_env = env->extend(let_rec_exp->proc_name_, procedure_value);
    return value_of(let_rec_exp->apply_body_, new_env);
  }
  if (exp->type_ == ExpressionType::PRINT_EXP) {
    const PrintExp *print_exp = get<PrintExp>(exp);
    auto value = value_of(print_exp->exp_, env);
    std::cout << "print " << to_string(value) << "\n";
    return value;
  }
  if (exp->type_ == ExpressionType::BEGIN_EXP) {
    const BeginExp *begin_exp = get<BeginExp>(exp);
    SPtr<Value> res;
    for (auto &exp : begin_exp->exps_) {
      res = value_of(exp, env);
    }
    return res;
  }
  if (exp->type_ == ExpressionType::SET_EXP) {
    const SetExp *set_exp = get<SetExp>(exp);
    SPtr<Value> value = value_of(set_exp->exp_, env);
    env->set(set_exp->var_name_, value);
    return value;
  }
  if (exp->type_ == ExpressionType::CLASS_DEF_EXP) {
    const ClassDefinExp *class_define_exp = get<ClassDefinExp>(exp);
    ClassDefine class_define;
    class_define.super_name_ = class_define_exp->super_class_name_;
    for (auto &field_name : class_define_exp->fields_) {
      ClassDefine::Field *field = new ClassDefine::Field{};
      field->var_name_ = field_name;
      class_define.members_[field_name] =
          SPtr<ClassDefine::Member>{(ClassDefine::Member *)field};
    }
    for (auto &method_def : class_define_exp->methods_) {
      ClassDefine::Method *method = new ClassDefine::Method{};
      method->name_ = method_def.name_;
      method->var_names_ = method_def.var_names_;
      method->body_ = method_def.body_;
      class_define.members_[method_def.name_] =
          SPtr<ClassDefine::Member>{(ClassDefine::Member *)method};
    }
    return to_value(class_define);
  }
  if (exp->type_ == ExpressionType::NEW_OBJECT_EXP) {
    const NewObjectExp *new_object_exp = get<NewObjectExp>(exp);
    std::vector<ClassDefine> class_define_chain;
    {
      std::string cur_class_name = new_object_exp->class_name_;
      auto value = env->apply(cur_class_name);
      CHECK(value->type_ == ValueType::CLASS_DEFINE)
      ClassDefine cur_class_define = value_to<ClassDefineValue>(value);
      class_define_chain.push_back(cur_class_define);
      while (cur_class_define.super_name_ != cur_class_name) {
        cur_class_name = cur_class_define.super_name_;
        value = env->apply(cur_class_name);
        CHECK(value->type_ == ValueType::CLASS_DEFINE)
        cur_class_define = value_to<ClassDefineValue>(value);
        class_define_chain.push_back(cur_class_define);
      }
    }
    Object object;
    for (auto &class_define : class_define_chain) {
      auto object_states = new_object_states(class_define, env);
      object.states_.push_back(object_states);
    }
    return to_value(object);
  }
  if (exp->type_ == ExpressionType::SEND_EXP) {
    const SendExp *send_exp = get<SendExp>(exp);
    //
    auto object_value = env->apply(send_exp->object_name_);
    CHECK(object_value->type_ == ValueType::OBJECT)
    auto object = value_to<ObjectValue>(object_value);
    auto new_env = env;
    auto object_chain_env = std::make_shared<Environment>();
    for (auto iter = object.states_.rbegin(); iter != object.states_.rend();
         iter++) {
      object_chain_env = object_chain_env->extend(*iter);
      new_env = new_env->extend(*iter);
    }
    new_env = new_env->extend("@this", object_value);
    std::vector<SPtr<Value>> arg_values;
    for (auto &arg_exp : send_exp->args_) {
      auto arg_value = value_of(arg_exp, env);
      arg_values.push_back(arg_value);
    }
    auto method_proc_value = object_chain_env->apply(send_exp->method_name_);
    CHECK(method_proc_value->type_ == ValueType::PROC)
    auto method_procedure = value_to<ProcedureValue>(method_proc_value);
    CHECK(method_procedure.parameters_.size() == arg_values.size())
    for (int i = 0; i < method_procedure.parameters_.size(); i++) {
      new_env = new_env->extend(method_procedure.parameters_[i], arg_values[i]);
    }
    return value_of(method_procedure.body_, new_env);
  }
  CHECK_WITH_INFO(false, "got unkown expression type!!!!")
  return nullptr;
}

SPtr<std::unordered_map<std::string, SPtr<Value>>>
new_object_states(const ClassDefine &class_define, SPtr<Environment> &env) {
  auto object_states =
      std::make_shared<std::unordered_map<std::string, SPtr<Value>>>();
  std::vector<std::string> field_names;
  for (auto &[name, member] : class_define.members_) {
    if (member->type_ == ClassDefine::MemmberType::FIELD) {
      auto field_ptr = (ClassDefine::Field *)member.get();
      field_names.push_back(field_ptr->var_name_);
      object_states->operator[](field_ptr->var_name_) = to_value(None{});
      continue;
    }
    if (member->type_ == ClassDefine::MemmberType::METHOD) {
      auto method_ptr = (ClassDefine::Method *)member.get();
      Procedure procedure;
      procedure.body_ = method_ptr->body_;
      procedure.parameters_ = method_ptr->var_names_;
      procedure.env_ = env;
      object_states->operator[](method_ptr->name_) = to_value(procedure);
      continue;
    }
    CHECK_WITH_INFO(false, "got unkown class member type!!!")
  }
  {
    auto iter = class_define.members_.find("constructor");
    if (iter != class_define.members_.end()) { // we have contructor
      CHECK(iter->second->type_ == ClassDefine::MemmberType::METHOD)
      auto new_env = env;
      for (auto &field_name : field_names) {
        new_env = new_env->extend(field_name, to_value(None{}));
      }
      ClassDefine::Method *method_define =
          (ClassDefine::Method *)iter->second.get();
      value_of(method_define->body_, new_env);
      for (auto &field_name : field_names) {
        object_states->operator[](field_name) = new_env->apply(field_name);
      }
    }
  }
  return object_states;
}