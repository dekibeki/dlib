cmake_minimum_required (VERSION 3.8)

project(dlib CXX)
project(dlibTest CXX)

set(dlibInclude "${CMAKE_CURRENT_SOURCE_DIR}/")
set(dlibSrc "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(dlibTest "${CMAKE_CURRENT_SOURCE_DIR}/test")

#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_DEBUG ON)
find_package(Boost 
  REQUIRED 
  COMPONENTS unit_test_framework)

set(dlibSources
  ${dlibSrc}/raft.cpp
  ${dlibSrc}/strong_type.cpp
  ${dlibSrc}/args.cpp
  ${dlibSrc}/db.cpp
  ${dlibSrc}/math.cpp
  ${dlibSrc}/util.cpp
  ${dlibSrc}/cache.cpp
  ${dlibSrc}/pool.cpp
  ${dlibSrc}/outcome.cpp
  ${dlibSrc}/outcome_orig.cpp
  ${dlibSrc}/meta.cpp
  ${dlibSrc}/unique_value.cpp
  ${dlibSrc}/interface.cpp
  ${dlibSrc}/interface_function.cpp
  ${dlibSrc}/arrays.cpp
  ${dlibSrc}/finder_interface.cpp
  ${dlibSrc}/stmt.cpp
  ${dlibSrc}/serialization.cpp
  ${dlibSrc}/unordered_vector.cpp
)

set(dlibTests
  ${dlibTest}/testMain.cpp
  ${dlibTest}/test_db.cpp
  ${dlibTest}/test_cache.cpp
  ${dlibTest}/test_pool.cpp
  ${dlibTest}/test_finder_interface.cpp
  ${dlibTest}/test_serialization.cpp
  ${dlibTest}/test_arrays.cpp
  ${dlibTest}/test_unordered_vector.cpp
  )

add_library(dlib ${dlibSources})
add_executable(dlibTest ${dlibTests})

target_include_directories(dlib PUBLIC
  ${dlibInclude}
  ${Boost_INCLUDE_DIRS}
  )

target_include_directories(dlibTest PUBLIC 
  ${dlibInclude}
  ${Boost_INCLUDE_DIRS}
  )

target_compile_features(dlib PUBLIC cxx_std_17)
target_compile_features(dlibTest PUBLIC cxx_std_17)

target_link_libraries(dlibTest 
  dlib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})