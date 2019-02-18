#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/strong_type.hpp>

#include <unordered_map>

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
  BOOST_TEST((strong_type::unwrap(test1) == 0));
}

BOOST_AUTO_TEST_CASE(strong_type_add_same) {
  struct Sv :
    public strong_type::Strong_type<int, strong_type::Construct<int>, strong_type::Add::Self> {
    using Strong_type::Strong_type;
  };

  

  Sv sv1{ 0 };
  Sv sv2{ 1 };
  Sv sv3{ sv1 + sv2 };
  BOOST_TEST((strong_type::unwrap(sv3) == 1));
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
  BOOST_TEST((strong_type::unwrap(result) == 1));
}

BOOST_AUTO_TEST_CASE(strong_type_increment) {
  struct Sv :
    public strong_type::Strong_type<int, strong_type::Construct<int>, strong_type::Pre_increment::Allow> {
    using Strong_type::Strong_type;
  };

  

  Sv sv{ 1 };
  ++sv;
  BOOST_TEST((strong_type::unwrap(sv) == 2));
  Sv* test = &sv;
  (void)test;
}

BOOST_AUTO_TEST_CASE(strong_type_const_ref_equals) {
  struct Sv :
    public strong_type::Strong_type<int, strong_type::Construct<int>,
    strong_type::Equal::Self> {
    using Strong_type::Strong_type;
    using Strong_type::operator=;
  };

  Sv sv{ 1 };
  (void)sv;
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

  Sv testing{ 0 };

  BOOST_TEST((hash(testing) == std::hash<int>{}(strong_type::unwrap(testing))));
}

BOOST_AUTO_TEST_CASE(strong_type_unordered_map_key) {
  struct Sv :
    public strong_type::Strong_type<int,
    strong_type::Construct<int>,
    strong_type::Regular> {
    using Strong_type::Strong_type;
    using Strong_type::operator=;
  };

  std::unordered_map<Sv, int, strong_type::Strong_type_hash> map;
  map.emplace(Sv{ 0 }, 1);
  BOOST_TEST((map.find(Sv{ 1 }) == map.end()));
  auto found = map.find(Sv{ 0 });
  BOOST_TEST((found != map.end()));
  BOOST_TEST((found->first == Sv{ 0 }));
  BOOST_TEST((found->second == 1));
}