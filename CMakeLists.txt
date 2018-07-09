cmake_minimum_required (VERSION 3.8)

project(dlib CXX)
project(dlibTest CXX)

set(dlibInclude "${CMAKE_CURRENT_SOURCE_DIR}/")
set(dlibSource "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(dlibTest "${CMAKE_CURRENT_SOURCE_DIR}/test")

set(dlibSources
  ${dlibSource}/null_mutex.cpp
  ${dlibSource}/raft.cpp
  ${dlibSource}/option.cpp
  ${dlibSource}/utility.cpp
  ${dlibSource}/protocols.cpp
  ${dlibSource}/strong_type.cpp
  ${dlibSource}/args.cpp
)

set(dlibTests
  ${dlibTest}/dev.cpp
)

add_library(dlib STATIC ${dlibSources})
add_executable(dlibTest ${dlibTests})

target_include_directories(dlib PUBLIC ${dlibInclude})

target_include_directories(dlibTest PUBLIC ${dlibInclude})

target_compile_features(dlib PUBLIC cxx_std_17)
target_compile_features(dlibTest PUBLIC cxx_std_17)