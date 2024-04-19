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
  ThreadScheduler() { begin_timer(); }
  void place_on_ready_queue(const Thread &cont_execution) {
    threads_.push(cont_execution);
  }
  void wait(const std::string &mutex_name, const Thread &cont_execution) {
    mutex_wait_threads_[mutex_name].push(cont_execution);
  }
  void signal(const std::string &mutex_name) {
    auto& queue = mutex_wait_threads_[mutex_name];
    while (!queue.empty()) {
      auto thread = queue.front();
      queue.pop();
      threads_.push(thread);
    }
  }
  Thread get_next_thread() {
    // std::cout<<"scheduler active...\n";
    begin_timer();
    if (threads_.empty()) {
      return [this]() { return the_final_answer_; };
    }
    auto thread = threads_.front();
    threads_.pop();
    return thread;
  }
  void decrement_timer() {
    if (the_time_remaining_ >= 1) {
      the_time_remaining_--;
    }
  }

  bool is_time_expired() { return the_time_remaining_ == 0; }
  static size_t THE_MAX_TIM_SLICE;
  SPtr<Value> the_final_answer_;
  std::queue<Thread> threads_;

private:
  void begin_timer() { the_time_remaining_ = THE_MAX_TIM_SLICE; }
  size_t the_time_remaining_{1};
  std::unordered_map<std::string, std::queue<Thread>> mutex_wait_threads_;
};
size_t ThreadScheduler::THE_MAX_TIM_SLICE = 2;