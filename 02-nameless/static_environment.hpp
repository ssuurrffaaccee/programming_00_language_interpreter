#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "check.hpp"
#include "value.hpp"
class StaticEnvironment : public std::enable_shared_from_this<StaticEnvironment> {
 public:
  struct Record {
    Record(const std::string &var, int index) : var_{var}, index_{index} {}
    std::string var_;
    int index_;
  };
  int apply(const std::string &name) {
    if (record_ != nullptr) {
      if (record_->var_ == name) {
        return record_->index_;
      }
    }
    CHECK(parent_ != nullptr)
    return parent_->apply(name);
  }
  std::shared_ptr<StaticEnvironment> extend(const std::string &name) {
    auto new_env = std::make_shared<StaticEnvironment>();
    new_env->parent_ = this->shared_from_this();
    if (record_ == nullptr) {
      new_env->record_ = std::make_unique<Record>(name, 0);
    } else {
      new_env->record_ = std::make_unique<Record>(name, ++(record_->index_));
    }
    return new_env;
  }
  std::unique_ptr<Record> record_{nullptr};
  SPtr<StaticEnvironment> parent_{nullptr};
};