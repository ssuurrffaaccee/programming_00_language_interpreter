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
    Record(const std::vector<std::string> &name, const SPtr<Type> &v)
        : name_{name}, type_{v} {}
    std::vector<std::string> name_;
    SPtr<Type> type_;
  };
  SPtr<Type> apply(const std::string &name) {
    if (record_ != nullptr) {
      if (record_->name_.size() == 1 && record_->name_[0] == name) {
        return record_->type_;
      }
    }
    CHECK(parent_ != nullptr)
    return parent_->apply(name);
  }
  SPtr<Type> apply(const std::string &module_name, const std::string name) {
    if (record_ != nullptr) {
      if (record_->name_.size() == 2 && record_->name_[0] == module_name &&
          record_->name_[1] == name) {
        return record_->type_;
      }
    }
    CHECK(parent_ != nullptr)
    return parent_->apply(module_name,name);
  }
  std::shared_ptr<TypeEnvironment> extend(const std::string &name,
                                          const SPtr<Type> &type) {
    auto new_env = std::make_shared<TypeEnvironment>();
    new_env->parent_ = this->shared_from_this();
    new_env->record_ =
        std::make_unique<Record>(std::vector<std::string>{name}, type);
    return new_env;
  }
  std::shared_ptr<TypeEnvironment> extend(const std::string &name,
                                          const std::string &sub_name,
                                          const SPtr<Type> &type) {
    auto new_env = std::make_shared<TypeEnvironment>();
    new_env->parent_ = this->shared_from_this();
    new_env->record_ = std::make_unique<Record>(
        std::vector<std::string>{name, sub_name}, type);
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
    for (auto &name : env->record_->name_) {
      ss << name << '.';
    }
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