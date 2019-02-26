cmake_minimum_required (VERSION 3.8)

project(dlib CXX)
project(dlibTest CXX)

set(dlibInclude "${CMAKE_CURRENT_SOURCE_DIR}/")
set(dlibSrc "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(dlibTest "${CMAKE_CURRENT_SOURCE_DIR}/test")

#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_DEBUG ON)
set(Boost_COMPILER -vc141)

find_package(Boost 
  REQUIRED 
  COMPONENTS unit_test_framework)

add_library(dlib
  ${dlibSrc}/args.cpp
  ${dlibSrc}/arrays.cpp
  ${dlibSrc}/cache.cpp
  ${dlibSrc}/db.cpp
  ${dlibSrc}/finder_interface.cpp
  ${dlibSrc}/interface.cpp
  ${dlibSrc}/iterators.cpp
  ${dlibSrc}/meta.cpp
  ${dlibSrc}/math.cpp
  ${dlibSrc}/outcome.cpp
  ${dlibSrc}/pool.cpp
  ${dlibSrc}/quaternion.cpp
  ${dlibSrc}/raft.cpp
  ${dlibSrc}/serialization.cpp
  ${dlibSrc}/soa.cpp
  ${dlibSrc}/soa_reference.cpp
  ${dlibSrc}/strong_type.cpp
  ${dlibSrc}/stmt.cpp
  ${dlibSrc}/unique_value.cpp
  ${dlibSrc}/util.cpp
  ${dlibSrc}/vector.cpp
  ${dlibSrc}/vector_adaptors.cpp
)

add_executable(dlibTest 
  ${dlibTest}/testMain.cpp
  ${dlibTest}/test_arrays.cpp
  ${dlibTest}/test_cache.cpp
  ${dlibTest}/test_db.cpp
  ${dlibTest}/test_finder_interface.cpp
  ${dlibTest}/test_interface.cpp
  ${dlibTest}/test_iterators.cpp
  ${dlibTest}/test_pool.cpp
  ${dlibTest}/test_quaternion.cpp
  ${dlibTest}/test_serialization.cpp
  ${dlibTest}/test_soa.cpp
  ${dlibTest}/test_strong_type.cpp
  ${dlibTest}/test_vector_adaptors.cpp
  )

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

target_link_libraries(dlib
  )

target_link_libraries(dlibTest 
  dlib
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

find_package(postgresql)

if(${PostgreSQL_FOUND})
  message("found postgresql")

  project(dlib_postgresql CXX)
  
  add_library(dlib_postgresql
    ${dlibSrc}/postgresql.cpp
    )

  target_include_directories(dlib_postgresql
    PUBLIC
    ${dlibInclude}
    PRIVATE
    ${PostgreSQL_INCLUDE_DIRS}
    )

  target_compile_features(dlib_postgresql PUBLIC cxx_std_17)

  target_link_libraries(dlib_postgresql
    PRIVATE
    ${PostgreSQL_LIBRARIES})
else()
  message("did not find postgresql")
endif()

find_package(Sqlite3)

if(${Sqlite3_FOUND})
  message("found sqlite3")

  find_path(Sqlite3_INCLUDE_DIRS sqlite3.h)

  project(dlib_sqlite CXX)

  add_library(dlib_sqlite
    ${dlibSrc}/sqlite.cpp
    )

  target_include_directories(dlib_sqlite
    PUBLIC
    ${dlibInclude}
    PRIVATE
    ${Sqlite3_INCLUDE_DIRS}
    )

  target_compile_features(dlib_sqlite PUBLIC cxx_std_17)

  target_link_libraries(dlib_sqlite
    PRIVATE
    ${sqlite3})

else()
  message("did not find sqlite3")
endif()