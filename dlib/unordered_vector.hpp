#pragma once

#include <memory>
#include <vector>
#include <cassert>

namespace dlib {
  template<typename T, typename Allocator = std::allocator<T>>
  class Unordered_vector {
  public:
    using Holder = std::vector<T, Allocator>;
    using value_type = typename Holder::value_type;
    using allocator_type = typename Holder::allocator_type;
    using size_type = typename Holder::size_type;
    using difference_type = typename Holder::difference_type;
    using reference = typename Holder::reference;
    using const_reference = typename Holder::const_reference;
    using pointer = typename Holder::pointer;
    using const_pointer = typename Holder::const_pointer;
    using iterator = typename Holder::iterator;
    using const_iterator = typename Holder::const_iterator;
    using reverse_iterator = typename Holder::reverse_iterator;
    using const_reverse_iterator = typename Holder::const_reverse_iterator;

    reference at(size_type i) {
      return holder_.at(i);
    }

    const_reference at(size_type i) const {
      return holder_.at(i);
    }

    reference operator[](size_type i) {
      return holder_[i];
    }

    const_reference operator[](size_type i) const {
      return holder_[i];
    }

    reference front() {
      return holder_.front();
    }

    const_reference front() const {
      return holder_.front();
    }

    reference back() {
      return holder_.back();
    }

    const_reference back() const {
      return holder_.back();
    }

    pointer data() noexcept {
      return holder_.data();
    }

    const_pointer data() const noexcept {
      return holder_.data();
    }

    iterator begin() noexcept {
      return holder_.begin();
    }

    const_iterator begin() const noexcept {
      return holder_.begin();
    }

    const_iterator cbegin() const noexcept {
      return holder_.cbegin();
    }

    iterator end() noexcept {
      return holder_.end();
    }

    const_iterator end() const noexcept {
      return holder_.end();
    }

    const_iterator cend() const noexcept {
      return holder_.cend();
    }

    reverse_iterator rbegin() noexcept {
      return holder_.rbegin();
    }

    const_reverse_iterator rbegin() const noexcept {
      return holder_.rbegin();
    }

    const_reverse_iterator crbegin() const noexcept {
      return holder_.crbegin();
    }

    reverse_iterator rend() noexcept {
      return holder_.rend();
    }

    const_reverse_iterator rend() const noexcept {
      return holder_.rend();
    }

    const_reverse_iterator crend() const noexcept {
      return holder_.crend();
    }

    bool empty() const noexcept {
      return holder_.empty();
    }

    size_type size() const noexcept {
      return holder_.size();
    }

    size_type max_size() const noexcept {
      return holder_.max_size();
    }

    void reserve(size_type n) {
      return holder_.reserve(n);
    }

    size_type capacity() const noexcept {
      return holder_.capacity();
    }

    void shrink_to_fit() {
      return holder_.shrink_to_fit();
    }

    void clear() noexcept {
      return holder_.clear();
    }

    void insert(const T& value) {
      holder_.push_back(value);
    }

    void insert(T&& value) {
      holder_.push_back(value);
    }

    template<typename ...Args>
    reference emplace(Args&&... args) {
      return holder_.emplace_back(std::forward<Args>(args)...);
    }

    iterator erase(const_iterator pos) {
      const size_type our_size = holder_.size();
      if (our_size == 1) {
        holder_.pop_back();
        return end();
      } else {
        const difference_type index = pos - holder_.cbegin();
        if (index != our_size - 1) {
          //if we arne't the last element, me need to move the last into us
          holder_[index] = std::move(holder_.back());
        }
        holder_.pop_back();
        return begin() + index;
      }
    }
    iterator erase(const_iterator first, const_iterator last) {
      const difference_type range_length = last - first;
      const difference_type our_size = holder_.size();
      if (range_length == our_size) {
        holder_.clear();
        return end();
      } else {
        const difference_type first_index = first - holder_.cbegin();
        const difference_type last_index = last - holder_.cbegin();
        const difference_type copy_start = vMax(last_index, static_cast<difference_type>(our_size - range_length));
        if (first_index != our_size - range_length) {
          /*
            because of how copy_start is set, copy_start + i < our_size includes a check 
            for last_index + i < our_size          
          */
          for (difference_type i = 0; copy_start + i < our_size; ++i) {
            holder_[first_index + i] = std::move(holder_[copy_start + i]);
          }
        }
        holder_.erase(holder_.end() - range_length, holder_.end());
        return begin() + first_index;
      }
    }

    void resize(size_type count) {
      holder_.resize(count);
    }

    void resize(size_type count, value_type const& value) {
      holder_.resize(count, value);
    }

    void swap(Unordered_vector& other) noexcept {
      holder_.swap(other.holder_);
    }
  private:
    Holder holder_;
  };
}