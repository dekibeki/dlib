#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/finder_interface.hpp>

namespace {
  struct Defaulted_dummy {
    int get_different(int i) noexcept {
      return i;
    }
    bool contains_different(int i) noexcept {
      return true;
    }
  };
}

namespace dlib {
  template<>
  struct Finder_default_get<Defaulted_dummy> {
    int operator()(Defaulted_dummy* ptr, int i) const noexcept {
      return ptr->get_different(i);
    }
  };

  template<>
  struct Finder_default_contains<Defaulted_dummy> {
    bool operator()(Defaulted_dummy* ptr, int i) const noexcept {
      return ptr->contains_different(i);
    }
  };
}

BOOST_AUTO_TEST_CASE(finder_interface_custom_defaults) {
  dlib::Finder_interface<int, int> test{ Defaulted_dummy{} };
  test.get(1);
  test.contains(2);
}

BOOST_AUTO_TEST_CASE(finder_interface_defaults) {
  struct Dummy {
    int get(int const& i) noexcept {
      return v_;
    }
    bool contains(int const&) const noexcept {
      return true;
    }
    int v_ = 0;
  };

  dlib::Finder_interface<int, int> test{ Dummy{} };
  test.get(0);
  test.contains(0);
}

BOOST_AUTO_TEST_CASE(finder_interface_custom) {
  struct Dummy {
    int get(int const& i) noexcept {
      return v_;
    }
    //bool contains(int const&) noexcept {
    //  return true;
    //}
    int v_ = 0;
  };

  using Interface = dlib::Finder_interface<int, int>;

  struct Shitty_lambda {
    bool operator()(Dummy* me, int k)const noexcept {
      return false;
    }
  };

  Interface test{ Dummy{}, dlib::Finder_contains{Shitty_lambda{}} };
  test.get(0);
  test.contains(0);
}