#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/interface.hpp>
#include <dlib/finder_interface.hpp>

using Finder_interface = dlib::Finder_interface<int, int>;
template<typename T>
struct Something {
  template<typename U>
  struct Inner {

  };
};

static_assert(dlib::contains_varg<
  ::dlib::Arg<int>::Holder,
  ::dlib::Arg<int>::Holder<void>>);