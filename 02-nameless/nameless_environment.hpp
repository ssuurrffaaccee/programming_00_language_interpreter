#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "check.hpp"
#include "value.hpp"
class NamelessEnvironment
    : public std::enable_shared_from_this<NamelessEnvironment> {
 public:
  struct Record {
    Record(int index, const SPtr<Value> &v) : index_{index}, value_{v} {}
    int index_;
    SPtr<Value> value_;
  };
  SPtr<Value> apply(int index) {
    if (record_ != nullptr) {
      if (record_->index_ == index) {
        return record_->value_;
      }
    }
    CHECK(parent_ != nullptr)
    return parent_->apply(index);
  }
  std::shared_ptr<NamelessEnvironment> extend(const SPtr<Value> &value) {
    auto new_env = std::make_shared<NamelessEnvironment>();
    new_env->parent_ = this->shared_from_this();
    if (record_ == nullptr) {
      new_env->record_ = std::make_unique<Record>(0, value);
    } else {
      new_env->record_ = std::make_unique<Record>(++(record_->index_), value);
    }
    return new_env;
  }
  std::unique_ptr<Record> record_{nullptr};
  SPtr<NamelessEnvironment> parent_{nullptr};
};