#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <dlib/serialization.hpp>

using namespace dlib;

BOOST_AUTO_TEST_CASE(serialization_size) {
  const auto int_size = serialization::serialization_size(int{ 0 });
  BOOST_TEST((int_size == sizeof(int)));
  const auto char_size = serialization::serialization_size(char{ 0 });
  BOOST_TEST((char_size == sizeof(char)));
  const auto compound_size = serialization::serialization_size(int{ 0 }, double{ 0 });
  BOOST_TEST((compound_size == sizeof(int) + sizeof(double)));
}

BOOST_AUTO_TEST_CASE(serialization_compound) {
  std::vector<std::byte> writing;
  auto write_size = serialization::serialization_size(uint32_t{ 0xdeadbeef }, double{ -5.5 });
  writing.reserve(write_size);
  auto write_res = serialization::serialize(std::back_insert_iterator{ writing }, uint32_t{ 0xdeadbeef }, double{ -5.5 });
}

namespace {
  struct NonDefaultConstructable {
  public:
    NonDefaultConstructable(std::byte i_) noexcept :
      i{ i_ } {
    }
    NonDefaultConstructable(NonDefaultConstructable const& me) noexcept :
      i{ me.i },
      copy_count{ me.copy_count + 1 },
      move_count{ me.move_count } {

    }
    NonDefaultConstructable(NonDefaultConstructable&& me) noexcept :
      i{ me.i },
      copy_count{ me.copy_count },
      move_count{ me.move_count + 1 } {

    }
    std::byte i;
    int copy_count = 0;
    int move_count = 0;
  };

  template<typename ...Ts>
  constexpr auto to_byte_array(Ts&&... ts) noexcept {
    return std::array{ static_cast<std::byte>(ts)... };
  }
}

namespace dlib::serialization {
  template<typename InputIterator, typename EndIterator>
  dlib::Result<Deserialization<NonDefaultConstructable, InputIterator>> deserialize(InputIterator begin, EndIterator end, dlib::Type_arg<NonDefaultConstructable>) {
    DLIB_TRY(byte, (deserialize<std::byte>(std::move(begin), end)));
    return dlib::Deserialization<NonDefaultConstructable, InputIterator>{ { byte.val }, std::move(byte.iter)};
  }
}

BOOST_AUTO_TEST_CASE(deserialize_arrays_no_default_constructor) {
  std::array input = to_byte_array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  auto deserialize_std_array = serialization::deserialize<std::array<NonDefaultConstructable, 10>>(input.begin(), input.end());
  BOOST_TEST((!!deserialize_std_array));
  auto deserialize_raw_array = serialization::deserialize<NonDefaultConstructable[10]>(input.begin(), input.end());
  BOOST_TEST((!!deserialize_raw_array));
  auto too_short_deserialize_raw_array = serialization::deserialize<NonDefaultConstructable[10]>(++input.begin(), input.end());
  BOOST_TEST((!too_short_deserialize_raw_array));
}
