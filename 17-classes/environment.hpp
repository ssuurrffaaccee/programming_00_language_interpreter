#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "check.hpp"
#include "value.hpp"

class Environment : public std::enable_shared_from_this<Environment> {
public:
  SPtr<Value> apply(const std::string &name) {
    CHECK(records_ != nullptr)
    auto iter = records_->find(name);
    if (iter != records_->end()) {
      return iter->second;
    }
    CHECK(parent_ != nullptr)
    return parent_->apply(name);
  }
  std::shared_ptr<Environment> extend(const std::string &name,
                                      const SPtr<Value> &value) {
    auto new_env = std::make_shared<Environment>();
    new_env->parent_ = this->shared_from_this();
    new_env->records_ =
        std::make_shared<std::unordered_map<std::string, SPtr<Value>>>();
    new_env->records_->operator[](name) = value;
    return new_env;
  }
  void set(const std::string &name, const SPtr<Value> &value) {
    CHECK(records_ != nullptr)
    auto iter = records_->find(name);
    if (iter != records_->end()) {
      iter->second = value;
      return;
    }
    CHECK(parent_ != nullptr)
    parent_->set(name, value);
  }
  std::shared_ptr<Environment>
  extend(const SPtr<std::unordered_map<std::string, SPtr<Value>>> &records) {
    auto new_env = std::make_shared<Environment>();
    new_env->parent_ = this->shared_from_this();
    new_env->records_ = records;
    return new_env;
  }
  SPtr<std::unordered_map<std::string, SPtr<Value>>> records_;
  SPtr<Environment> parent_{nullptr};
};
std::string to_string(const SPtr<Environment> &env) {
  std::stringstream ss;
  if (env == nullptr) {
    return "END";
  }
  if (env->records_ != nullptr) {
    ss<<"{";
    for (auto &[name, value] : *(env->records_)) {
      ss << name << "=>";
      if (value == nullptr) {
        ss << "NULL_PTR";
        ss << "--->";
        continue;
      }
      ss << to_string(value);
      ss << "--->";
    }
    ss<<"}";
  }
  ss << to_string(env->parent_);
  return ss.str();
}