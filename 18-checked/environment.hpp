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
    Record(const std::string &var, const SPtr<Value> &v)
        : var_{var}, value_{v} {}
    std::string var_;
    SPtr<Value> value_;
  };
  SPtr<Value> apply(const std::string &name) {
    if (record_ != nullptr) {
      if (record_->var_ == name) {
        return record_->value_;
      }
    }
    CHECK(parent_ != nullptr)
    return parent_->apply(name);
  }
  std::shared_ptr<Environment> extend(const std::string &name,
                                      const SPtr<Value> &value) {
    auto new_env = std::make_shared<Environment>();
    new_env->parent_ = this->shared_from_this();
    new_env->record_ = std::make_unique<Record>(name, value);
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
    ss << (env->record_->var_);
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