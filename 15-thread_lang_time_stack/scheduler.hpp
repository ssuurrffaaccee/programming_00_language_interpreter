#pragma once
#include <functional>
#include <queue>
#include <stack>

#include "value.hpp"
using ContStack = std::stack<SPtr<Continuation>>;
class ThreadScheduler {
public:
  static ThreadScheduler &get() {
    static ThreadScheduler thread_scheduler_;
    return thread_scheduler_;
  }
  void place_on_ready_queue(const SPtr<ContStack> &cont_stack) {
    stacks_.push(cont_stack);
  }
  bool empty() { return stacks_.empty(); }
  SPtr<ContStack> get_next_thread_stack() {
    CHECK(!stacks_.empty())
    auto thread = stacks_.front();
    stacks_.pop();
    return thread;
  }
  void wait(const std::string &mutex_name, const SPtr<ContStack> &cont_stack) {
    mutex_wait_stack_[mutex_name].push(cont_stack);
  }
  void signal(const std::string &mutex_name) {
    auto &queue = mutex_wait_stack_[mutex_name];
    while (!queue.empty()) {
      auto thread = queue.front();
      queue.pop();
      stacks_.push(thread);
    }
  }
  bool is_time_expired() { return the_time_remaining_ == 0; }
  SPtr<Value> the_final_answer_;

private:
  std::queue<SPtr<ContStack>> stacks_;
  static size_t THE_MAX_TIM_SLICE;
  void begin_timer() { the_time_remaining_ = THE_MAX_TIM_SLICE; }
  size_t the_time_remaining_{1};
  std::unordered_map<std::string, std::queue<SPtr<ContStack>>>
      mutex_wait_stack_;
};
size_t ThreadScheduler::THE_MAX_TIM_SLICE = 2;