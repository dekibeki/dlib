#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/strong_type.hpp>

using namespace dlib;
namespace {
  static_assert(strong_type::is_strong_type<strong_type::Strong_type<int>> == true);

  struct Struct_wrapper_test :
    public strong_type::Strong_type<int> {

  };

  static_assert(strong_type::is_strong_type<Struct_wrapper_test> == true);

  struct Bad_struct_test {

  };

  static_assert(strong_type::is_strong_type<Bad_struct_test> == false);
}

BOOST_AUTO_TEST_CASE(strong_type_default_construct) {
  strong_type::Strong_type<int, strong_type::Construct<>> test1;
}

BOOST_AUTO_TEST_CASE(strong_type_add_same) {
  struct Sv :
    public strong_type::Strong_type<int, strong_type::Construct<int>, strong_type::Add::Self> {
    using Strong_type::Strong_type;
  };

  

  Sv sv1{ 0 };
  Sv sv2{ 1 };
  Sv sv3{ sv1 + sv2 };
}

BOOST_AUTO_TEST_CASE(strong_type_add_sided) {
  struct Sv_left :
    public strong_type::Strong_type<int, strong_type::Construct<int>> {
    using Strong_type::Strong_type;
  };
  struct Sv_right :
    public strong_type::Strong_type<int, strong_type::Construct<int>, strong_type::Add::RightOf<Sv_left>> {
    using Strong_type::Strong_type;
  };

  Sv_left left{ 0 };
  Sv_right right{ 1 };
  Sv_left result{ left + right };
}

BOOST_AUTO_TEST_CASE(strong_type_increment) {
  struct Sv :
    public strong_type::Strong_type<int, strong_type::Construct<int>, strong_type::Pre_increment::Allow> {
    using Strong_type::Strong_type;
  };

  

  Sv sv{ 1 };
  ++sv;
  Sv* test = &sv;
}

BOOST_AUTO_TEST_CASE(strong_type_const_ref_equals) {
  struct Sv :
    public strong_type::Strong_type<int, strong_type::Construct<int>,
    strong_type::Equal::Self> {
    using Strong_type::Strong_type;
    using Strong_type::operator=;
  };

  Sv sv{ 1 };

  BOOST_TEST((sv == sv));

  Sv const& sv_cr = sv;

  BOOST_TEST((sv_cr == sv_cr));
  BOOST_TEST((sv == sv_cr));
  BOOST_TEST((sv_cr == sv));
}

BOOST_AUTO_TEST_CASE(strong_type_hash) {
  strong_type::Strong_type_hash hash;

  struct Sv :
    public strong_type::Strong_type<int, strong_type::Construct<int>,
    strong_type::Equal::Self> {
    using Strong_type::Strong_type;
    using Strong_type::operator=;
  };

  struct Non_sv {
    int i;
  };

  static_assert(std::is_invocable_v<strong_type::Strong_type_hash, Sv>);
  static_assert(!std::is_invocable_v<strong_type::Strong_type_hash, Non_sv>);

  hash(Sv{ 0 });
}