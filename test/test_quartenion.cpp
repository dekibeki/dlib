#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/quartenion.hpp>
#include <dlib/math.hpp>

namespace {

  using Distance = double;
  using Vector = dlib::Vector<Distance, 3>;
  using Quartenion = dlib::Quartenion<Distance>;

  const Vector forwards = Vector{ 1,0,0 };
  const Vector up = Vector{ 0,1,0 };
  const Vector left = Vector{ 0,0,1 };

  bool equal(Vector v1, Vector v2) {
    return std::abs(v1[0] - v2[0]) < std::numeric_limits<Distance>::epsilon()
      && std::abs(v1[1] - v2[1]) < std::numeric_limits<Distance>::epsilon()
      && std::abs(v1[2] - v2[2]) < std::numeric_limits<Distance>::epsilon();
  }
}
BOOST_AUTO_TEST_CASE(quartenion_rotation) {
  {
    Quartenion rotation = dlib::make_rotation(forwards, dlib::pi);
    {
      Vector rotated = dlib::rotate(forwards, rotation);
      BOOST_TEST((equal(rotated, forwards)));
    }
    {
      Vector rotated = dlib::rotate(up, rotation);
      BOOST_TEST((equal(rotated, -up)));
    }
    {
      Vector rotated = dlib::rotate(left, rotation);
      BOOST_TEST((equal(rotated, -left)));
    }
  }
  {
    Quartenion rotation = dlib::make_rotation(up, dlib::pi);
    {
      Vector rotated = dlib::rotate(forwards, rotation);
      BOOST_TEST((equal(rotated, -forwards)));
    }
    {
      Vector rotated = dlib::rotate(up, rotation);
      BOOST_TEST((equal(rotated, up)));
    }
    {
      Vector rotated = dlib::rotate(left, rotation);
      BOOST_TEST((equal(rotated, -left)));
    }
  }
  {
    Quartenion rotation = dlib::make_rotation(left, dlib::pi);
    {
      Vector rotated = dlib::rotate(forwards, rotation);
      BOOST_TEST((equal(rotated, -forwards)));
    }
    {
      Vector rotated = dlib::rotate(up, rotation);
      BOOST_TEST((equal(rotated, -up)));
    }
    {
      Vector rotated = dlib::rotate(left, rotation);
      BOOST_TEST((equal(rotated, left)));
    }
  }
}