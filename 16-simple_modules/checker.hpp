#pragma once
#include "check.hpp"
#include "expression.hpp"
#include "type.hpp"
#include "type_environment.hpp"

bool is_subtype(const std::unordered_map<std::string, SPtr<Type>> &may_subtype,
                const std::unordered_map<std::string, SPtr<Type>> &super_type,
                SPtr<TypeEnvironment> &env);
SPtr<TypeEnvironment> check_module_type(const SPtr<Expression> &exp,
                                        SPtr<TypeEnvironment> &env);
SPtr<Type> type_of(const SPtr<Expression> &exp, SPtr<TypeEnvironment> &env) {
  if (exp->type_ == ExpressionType::CONST_EXP) {
    auto const_exp = get<ConstExp>(exp);
    return make_int_type();
  }
  if (exp->type_ == ExpressionType::VAR_EXP) {
    const VarExp *var_exp = get<VarExp>(exp);
    return env->apply(var_exp->var_);
  }
  if (exp->type_ == ExpressionType::DIFF_EXP) {
    const DiffExp *diff_exp = get<DiffExp>(exp);
    SPtr<Type> left_type = type_of(diff_exp->left_exp_, env);
    SPtr<Type> right_type = type_of(diff_exp->right_exp_, env);
    CHECK(left_type->type_ == TypeType::INT_TYPE)
    CHECK(right_type->type_ == TypeType::INT_TYPE)
    return make_int_type();
  }
  if (exp->type_ == ExpressionType::ADD_EXP) {
    const AddExp *add_exp = get<AddExp>(exp);
    SPtr<Type> left_type = type_of(add_exp->left_exp_, env);
    SPtr<Type> right_type = type_of(add_exp->right_exp_, env);
    CHECK(left_type->type_ == TypeType::INT_TYPE)
    CHECK(right_type->type_ == TypeType::INT_TYPE)
    return make_int_type();
  }
  if (exp->type_ == ExpressionType::IS_ZERO_EXP) {
    const IsZeroExp *is_zero_exp = get<IsZeroExp>(exp);
    SPtr<Type> value_type = type_of(is_zero_exp->exp_, env);
    CHECK(value_type->type_ == TypeType::INT_TYPE);
    return make_bool_type();
  }
  if (exp->type_ == ExpressionType::IF_EXP) {
    const IfExp *if_exp = get<IfExp>(exp);
    SPtr<Type> value_type = type_of(if_exp->condition_exp_, env);
    CHECK(value_type->type_ == TypeType::BOOL_TYPE)
    SPtr<Type> true_exp_type = type_of(if_exp->true_exp_, env);
    SPtr<Type> false_exp_type = type_of(if_exp->true_exp_, env);
    CHECK(true_exp_type == false_exp_type)
    return true_exp_type;
  }
  if (exp->type_ == ExpressionType::LET_EXP) {
    const LetExp *let_exp = get<LetExp>(exp);
    SPtr<Type> value_type = type_of(let_exp->exp_, env);
    auto new_env = env->extend(let_exp->var_, value_type);
    return type_of(let_exp->body_, new_env);
  }
  if (exp->type_ == ExpressionType::PROC_EXP) {
    const ProcExp *proc_exp = get<ProcExp>(exp);
    auto new_env = env->extend(proc_exp->parameter_, proc_exp->parameter_type_);
    auto result_type = type_of(proc_exp->body_, new_env);
    return make_proc_type(proc_exp->parameter_type_, result_type);
  }
  if (exp->type_ == ExpressionType::CALL_EXP) {
    const CallExp *call_exp = get<CallExp>(exp);
    auto proc_type = type_of(call_exp->proc_generator_, env);
    CHECK(proc_type->type_ == TypeType::PROC_TYPE)
    auto arg_type = type_of(call_exp->arg_, env);
    auto proc_type_ptr = (ProcType *)proc_type.get();
    CHECK(proc_type_ptr->parameter_type_->type_ == arg_type->type_);
    return proc_type_ptr->result_type_;
  }
  if (exp->type_ == ExpressionType::LETREC_EXP) {
    const LetRecExp *let_rec_exp = get<LetRecExp>(exp);
    auto rec_proc_type =
        make_proc_type(let_rec_exp->parameter_type_, let_rec_exp->result_type_);
    //std::cout << to_string(rec_proc_type) << "\n";
    auto new_env = env->extend(let_rec_exp->proc_name_, rec_proc_type);
    return type_of(let_rec_exp->apply_body_, new_env);
  }
  if (exp->type_ == ExpressionType::PRINT_EXP) {
    const PrintExp *print_exp = get<PrintExp>(exp);
    return type_of(print_exp->exp_, env);
  }
  if (exp->type_ == ExpressionType::BEGIN_EXP) {
    const BeginExp *begin_exp = get<BeginExp>(exp);
    SPtr<Type> res;
    for (auto exp : begin_exp->exps_) {
      if (exp->type_ == ExpressionType::MODULE_EXP) {
        env = check_module_type(exp, env);
      } else {
        res = type_of(exp, env);
      }
    }
    return res;
  }
  if (exp->type_ == ExpressionType::IMPORT_EXP) {
    const ImportExp *import_exp = get<ImportExp>(exp);
    return env->apply(import_exp->model_name_, import_exp->component_name_);
  }
  CHECK_WITH_INFO(false, "got unkown expression type!!!!")
  return nullptr;
}
SPtr<TypeEnvironment> check_module_type(const SPtr<Expression> &exp,
                                        SPtr<TypeEnvironment> &env) {
  const ModuleExp *module_exp = get<ModuleExp>(exp);
  std::unordered_map<std::string, SPtr<Type>> actual_types;
  auto new_env = env;
  for (auto &[name, impl_exp] : module_exp->body_) {
    auto actual_type = type_of(impl_exp, new_env);
    new_env = new_env->extend(name,actual_type); // body type dedpent on internal type, maybe
    CHECK(actual_types.find(name) == actual_types.end())
    actual_types[name] = actual_type;
  }
  std::unordered_map<std::string, SPtr<Type>> expected_types;
  for (auto &[name, type] : module_exp->interface_) {
    CHECK(expected_types.find(name) == expected_types.end())
    expected_types[name] = type;
  }
  CHECK(is_subtype(actual_types, expected_types, env))
  std::string module_name = module_exp->name_;
  for (auto &[name, type] : module_exp->interface_) {
    env = env->extend(module_name, name, type);
  }
  return env;
}
bool is_subtype(const std::unordered_map<std::string, SPtr<Type>> &may_subtype,
                const std::unordered_map<std::string, SPtr<Type>> &super_type,
                SPtr<TypeEnvironment> &env) {
  // for (auto &[name, type] : super_type) {
  //   std::cout << name << "->" << to_string(type) << "\n";
  // }
  // std::cout << "====\n";
  // for (auto &[name, type] : may_subtype) {
  //   std::cout << name << "->" << to_string(type) << "\n";
  // }
  for (auto &[name, type] : super_type) {
    auto iter = may_subtype.find(name);
    if (iter == may_subtype.end()) {
      return false;
    };
    if (!is_type_equal(type, iter->second)) {
      return false;
    };
  }
  return true;
}