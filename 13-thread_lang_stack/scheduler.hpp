#pragma once
#include <functional>
#include <queue>
#include <stack>

#include "value.hpp"
using ContStack = std::stack<SPtr<Continuation>>;
class ThreadScheduler {
 public:
  static ThreadScheduler& get() {
    static ThreadScheduler thread_scheduler_;
    return thread_scheduler_;
  }
  void place_on_ready_queue(const SPtr<ContStack>& cont_stack) {
    stacks_.push(cont_stack);
  }
  bool empty() { return stacks_.empty(); }
  SPtr<ContStack> get_next_thread_stack() {
    CHECK(!stacks_.empty())
    auto thread = stacks_.front();
    stacks_.pop();
    return thread;
  }
  SPtr<Value> the_final_answer_;
  private:
  std::queue<SPtr<ContStack>> stacks_;
};