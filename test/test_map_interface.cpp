#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/map_interface.hpp>

BOOST_AUTO_TEST_CASE(default_methods) {
  struct Dummy {
    int& get(int const& i) noexcept {
      return v_;
    }
    int& set(int const&, int v) noexcept {
      return v;
    }
    void erase(int const&) noexcept {

    }
    bool contains(int const&) const noexcept {
      return true;
    }
    int v_ = 0;
  };

  dlib::Map_interface<int, int> test{ Dummy{} };
  test.get(0);
  test.set(0, 0);
  test.erase(0);
  test.contains(0);
}

BOOST_AUTO_TEST_CASE(custom_method) {
  struct Dummy {
    int& get(int const& i) noexcept {
      return v_;
    }
    int& set(int const&, int v) noexcept {
      return v;
    }
    void erase(int const&) const noexcept {

    }
    //bool contains(int const&) noexcept {
    //  return true;
    //}
    int v_ = 0;
  };

  using Interface = dlib::Map_interface<int, int>;

  Interface::Contains_function contains = +[](const void* p, int const& v) noexcept -> dlib::Result<bool> {
    return true;
  };

  Interface test{ Dummy{}, Interface::Contains{ contains } };
  test.get(0);
  test.set(0, 0);
  test.erase(0);
  test.contains(0);
}