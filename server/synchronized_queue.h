#pragma once

#include <cassert>
#include <mutex>
#include <optional>
#include <queue>

template <typename T, int MAX_SIZE>
class SynchronizedQueue {
 public:
  bool TryPush(const T& value) {
    std::lock_guard lock(mutex_);
    if (q_.size() < MAX_SIZE) {
      q_.push(value);
      return true;
    }
    return false;
  }

  std::optional<T> TryPop() {
    std::lock_guard lock(mutex_);
    if (!q_.empty()) {
      return Pop();
    }
    return std::nullopt;
  }

 private:
  T Pop() {
    T res = q_.front();
    q_.pop();
    return res;
  }

 private:
  std::queue<T> q_;  // Guarded by mutex
  std::mutex mutex_;
};
