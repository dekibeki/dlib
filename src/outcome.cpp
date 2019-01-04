#include <dlib/outcome.hpp>

bool dlib::isSuccess(ResultVal res) {
    return res == ResultVal::Success;
}