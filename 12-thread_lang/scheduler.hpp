#pragma once
#include <functional>
#include <queue>

#include "value.hpp"
class ThreadScheduler {
 public:
  using Thread = std::function<SPtr<Value>()>;
  static ThreadScheduler &get() {
    static ThreadScheduler thread_scheduler_;
    return thread_scheduler_;
  }
  void place_on_ready_queue(const Thread &cont_execution) {
    threads_.push(cont_execution);
  }
  Thread get_next_thread() {
    if (threads_.empty()) {
      return [this]() { return the_final_answer_; };
    }
    auto thread = threads_.front();
    threads_.pop();
    return thread;
  }
  SPtr<Value> the_final_answer_;
  std::queue<Thread> threads_;
};