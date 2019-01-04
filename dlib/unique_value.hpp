#pragma once

#include <optional>

#include <boost/compressed_pair.hpp>

namespace dlib {

  template<typename T>
  struct Null_destroyer {
    constexpr void operator()(T& t) const noexcept {}
  };

  template<typename T, typename Destroyer = Null_destroyer<T>>
  class Unique_value {
  public:
    using Element_type = T;
    using Deleter_type = Destroyer;

    explicit Unique_value(Element_type val) noexcept :
      data_(std::move(val), Destroyer()){

    }

    Unique_value(Element_type val, Deleter_type d) noexcept :
      data_(std::move(val), std::move(d)) {

    }

    Unique_value(Unique_value&&) noexcept = default;
    Unique_value(Unique_value const&) = delete;
    Unique_value& operator=(Unique_value&&) noexcept = default;
    Unique_value& operator=(Unique_value const&) = delete;

    ~Unique_value() {
      get_deleter()(get());
    }

    template<typename U>
    void reset(U&& u) noexcept {
      Element_type saved = std::move(get());
      get() = Element_type(std::forward<U>(u));
      get_deleter()(saved);
    }

    Element_type& operator*() noexcept {
      return get();
    }
    Element_type const& operator*() const noexcept {
      return get();
    }

    Element_type* operator->() noexcept {
      return &get();
    }
    const Element_type* operator->() const noexcept {
      return &get();
    }

    Element_type& get() noexcept {
      return data_.first();
    }
    Element_type const& get() const noexcept {
      return data_.first();
    }

    Deleter_type& get_deleter() noexcept {
      return data_.second();
    }
    Deleter_type const& get_deleter() const noexcept {
      return data_.second();
    }
  private:
    boost::compressed_pair<Element_type, Deleter_type> data_;
  };

  template<typename T, typename Destroyer>
  class Unique_value<std::optional<T>, Destroyer> {
    using Element_type = std::optional<T>;
    using Deleter_type = Destroyer;

    constexpr Unique_value() noexcept :
      data_(std::nullopt, Destroyer()) {

    }

    explicit Unique_value(T val) noexcept :
      data_(std::move(val), Destroyer()) {

    }

    explicit Unique_value(Element_type val) noexcept :
      data_(std::move(val), Destroyer()) {

    }

    Unique_value(Element_type val, Deleter_type d) noexcept :
      data_(std::move(val), std::move(d)) {

    }

    Unique_value(T val, Deleter_type d) noexcept :
      data_(std::move(val), std::move(d)) {

    }

    Unique_value(Unique_value&&) noexcept = default;
    Unique_value(Unique_value const&) = delete;
    Unique_value& operator=(Unique_value&&) noexcept = default;
    Unique_value& operator=(Unique_value const&) = delete;

    ~Unique_value() {
      get_destroyer()(get());
    }

    template<typename U>
    void reset(U&& u = std::nullopt) noexcept {
      Element_type saved = std::move(get());
      get() = Element_type(std::forward<U>(u));
      get_deleter()(saved);
    }

    Element_type release() noexcept {
      Element_type returning = std::move(get());
      get() = std::nullopt;
      return returning;
    }

    Element_type& operator*() noexcept {
      return get();
    }
    Element_type const& operator*() const noexcept {
      return get();
    }

    Element_type* operator->() noexcept {
      return &get();
    }
    const Element_type* operator->() const noexcept {
      return &get();
    }

    Element_type& get() noexcept {
      return data_.first();
    }
    Element_type const& get() const noexcept {
      return data_.first();
    }

    Deleter_type& get_deleter() noexcept {
      return data_.second();
    }
    Deleter_type const& get_deleter() const noexcept {
      return data_.second();
    }
  private:
    boost::compressed_pair<Element_type, Deleter_type> data_;
  };
}