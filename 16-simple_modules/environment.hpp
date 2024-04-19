#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "check.hpp"
#include "value.hpp"
class Environment : public std::enable_shared_from_this<Environment> {
 public:
  struct Record {
    Record(const std::vector<std::string> &name, const SPtr<Value> &v)
        : name_{name}, value_{v} {}
    std::vector<std::string> name_;
    SPtr<Value> value_;
  };
  SPtr<Value> apply(const std::string &name) {
    if (record_ != nullptr) {
      if (record_->name_.size() == 1 && record_->name_[0] == name) {
        return record_->value_;
      }
    }
    CHECK(parent_ != nullptr)
    return parent_->apply(name);
  }
  SPtr<Value> apply(const std::string &module_name, const std::string name) {
    if (record_ != nullptr) {
      if (record_->name_.size() == 2 && record_->name_[0] == module_name &&
          record_->name_[1] == name) {
        return record_->value_;
      }
    }
    CHECK(parent_ != nullptr)
    return parent_->apply(module_name, name);
  }
  std::shared_ptr<Environment> extend(const std::string &name,
                                      const SPtr<Value> &value) {
    auto new_env = std::make_shared<Environment>();
    new_env->parent_ = this->shared_from_this();
    new_env->record_ =
        std::make_unique<Record>(std::vector<std::string>{name}, value);
    return new_env;
  }
  std::shared_ptr<Environment> extend(const std::string &name,
                                      const std::string &sub_name,
                                      const SPtr<Value> &value) {
    auto new_env = std::make_shared<Environment>();
    new_env->parent_ = this->shared_from_this();
    new_env->record_ = std::make_unique<Record>(
        std::vector<std::string>{name, sub_name}, value);
    return new_env;
  }
  std::unique_ptr<Record> record_;
  SPtr<Environment> parent_{nullptr};
};
std::string to_string(const SPtr<Environment> &env) {
  std::stringstream ss;
  if (env == nullptr) {
    return "--->END\n";
  }
  if (env->record_ != nullptr) {
    for (auto &name : env->record_->name_) {
      ss << name << '.';
    }
    ss << "=>";
    if (env->record_->value_ != nullptr) {
      ss << to_string(env->record_->value_);
    }
  }
  if (env->parent_ != nullptr) {
    ss << "--->";
    ss << to_string(env->parent_);
  } else {
    ss << "--->END\n";
  }
  return ss.str();
}