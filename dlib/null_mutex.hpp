#pragma once
#include <mutex>
#include <thread>

namespace dlib {
  struct NullMutex {
    void lock() const noexcept;
    void unlock() const noexcept;
    bool try_lock() const noexcept;
  };

  using NullConditionVariable = std::condition_variable_any;
}