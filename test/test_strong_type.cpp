#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/strong_type.hpp>
#include <dlib/vector.hpp>

using namespace dlib;

BOOST_AUTO_TEST_CASE(strong_value_default_construct) {
  strongValue::StrongValue<int, strongValue::Construct<>> test1;
}

BOOST_AUTO_TEST_CASE(strong_value_add_same) {
  using Sv = strongValue::StrongValue<int, strongValue::Construct<int>,  strongValue::Add::Self<>>;

  Sv sv1{ 0 };
  Sv sv2{ 1 };
  Sv sv3{ sv1 + sv2 };
}

BOOST_AUTO_TEST_CASE(strong_value_add_sided) {
  using Sv_left = strongValue::StrongValue<int, strongValue::Construct<int>>;
  using Sv_right = strongValue::StrongValue<int, strongValue::Construct<int>, strongValue::Add::RightOf<Sv_left>>;

  Sv_left left{ 0 };
  Sv_right right{ 1 };
  Sv_left result{ left + right };
}