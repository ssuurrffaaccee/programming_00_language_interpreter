#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "check.hpp"
#include "ref.hpp"
#include "value.hpp"
class Environment : public std::enable_shared_from_this<Environment> {
 public:
  struct Record {
    Record(const std::string &var, const Ref &ref) : var_{var}, ref_{ref} {}
    std::string var_;
    Ref ref_;
  };
  Ref apply(const std::string &name) {
    if (record_ != nullptr) {
      if (record_->var_ == name) {
        return record_->ref_;
      }
    }
    CHECK(parent_ != nullptr)
    return parent_->apply(name);
  }
  std::shared_ptr<Environment> extend(const std::string &name, const Ref &ref) {
    auto new_env = std::make_shared<Environment>();
    new_env->parent_ = this->shared_from_this();
    new_env->record_ = std::make_unique<Record>(name, ref);
    return new_env;
  }
  std::unique_ptr<Record> record_;
  SPtr<Environment> parent_{nullptr};
};
