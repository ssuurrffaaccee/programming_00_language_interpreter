#pragma once
#include "check.hpp"
#include "value.hpp"

class Store {
  public:
  Store() : next_index_{0} {}
  Ref new_ref(const SPtr<Value> &value) {
    Ref new_ref = Ref{next_index_};
    next_index_++;
    store_map_[new_ref.index_] = value;
    return new_ref;
  }
  SPtr<Value> deref(const Ref &ref) {
    auto iter = store_map_.find(ref.index_);
    CHECK_WITH_INFO(iter != store_map_.end(),
                    std::string("deref failed!!! ref:") +
                        std::to_string(ref.index_))
    return iter->second;
  }
  void set_ref(const Ref &ref, const SPtr<Value> &value) {
    auto iter = store_map_.find(ref.index_);
    CHECK_WITH_INFO(iter != store_map_.end(),
                    std::string("deref failed!!! ref:") +
                        std::to_string(ref.index_))
    iter->second = value;
  }

private:
  int next_index_;
  std::unordered_map<int, SPtr<Value>> store_map_;
};