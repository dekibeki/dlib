#include <dlib/strong_type.hpp>

using namespace dlib;

bool test() {

  using T = strongValue::StrongValue<int,
    strongValue::Construct<int>,
    strongValue::Add::Self<>,
    strongValue::Equal::LeftOf<int>>;
  
  T i{ 0 };
  T j{ 1 };

  return (i + j) == 2;
}