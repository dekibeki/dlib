#include <dlib/null_mutex.hpp>

void dlib::NullMutex::lock() const noexcept {

}

void dlib::NullMutex::unlock() const noexcept {

}

bool dlib::NullMutex::try_lock() const noexcept {
  return true;
}