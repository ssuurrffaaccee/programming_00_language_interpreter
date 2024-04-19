#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "check.hpp"
#include "type.hpp"
class TypeEnvironment : public std::enable_shared_from_this<TypeEnvironment> {
 public:
  struct Record {
    Record(const std::string &var, const SPtr<Type> &t) : var_{var}, type_{t} {}
    std::string var_;
    SPtr<Type> type_;
  };
  SPtr<Type> apply(const std::string &name) {
    if (record_ != nullptr) {
      if (record_->var_ == name) {
        return record_->type_;
      }
    }
    CHECK(parent_ != nullptr)
    return parent_->apply(name);
  }
  std::shared_ptr<TypeEnvironment> extend(const std::string &name,
                                          const SPtr<Type> &t) {
    auto new_env = std::make_shared<TypeEnvironment>();
    new_env->parent_ = this->shared_from_this();
    new_env->record_ = std::make_unique<Record>(name, t);
    return new_env;
  }
  std::unique_ptr<Record> record_;
  SPtr<TypeEnvironment> parent_{nullptr};
};

std::string to_string(const SPtr<TypeEnvironment> &env) {
  std::stringstream ss;
  if (env == nullptr) {
    return "END";
  }
  if (env->record_ != nullptr) {
    ss << (env->record_->var_);
    ss << "=>";
    if (env->record_->type_ != nullptr) {
    //   ss << to_string(env->record_->type_->type_);
        ss << int(env->record_->type_->type_);
    }
  }
  ss << "--->";
  ss << to_string(env->parent_);
  return ss.str();
}